#include "pch.h"
#include "OdColumn.h"
#include "OdStatement.h"
#include "OdUtil.h"
#include "IStatement.h"
#include "Constants.h"
#include "DateUtils.h"

#include <sql.h>  
#include <sqlext.h>  
#include <sqltypes.h>
#include <stdexcept>
#include <tchar.h>

// Constructor for a CODColumn object.  The pStatement is a pointer to the sql statement that the column is a part of.
// The ordinal is the location in the result set of that columns and the bulk rows is how many rows are being fetched at
// a time.  The column needs to know how many rows will be retrieved so it can allocate space for the data.
// For example if 10 rows are being bulk fetched and the column is an SQLINTEGER the a buffer of 10 * sizeof(int) needs to be allocated.
CODColumn::CODColumn( CIStatement* pStatement, SQLSMALLINT nOrdinal, SQLINTEGER nBulkRows )
{
	m_pStatement = pStatement;

	m_Ordinal = nOrdinal + 1;

	m_Name.Empty();
	m_SqlType = eSqlUnknown;
	m_NativeType = eNativeUnknown;
	m_TypeName.Empty();
	m_SqlLength = 0;
	m_NativeLength = 0;
	m_SqlScale = 0;
	m_NativeScale = 0;
	m_OdbcPrecision = 0;
	m_NativePrecision = 0;
	m_Nullable = 0;
	m_IsNull = true;
	m_BulkFetchCount = nBulkRows;
	m_pData = nullptr;
	m_pCurrentData = nullptr;
	m_pIndicator = nullptr;
	m_bMSJet = false;
}

// Destructor for a column object.  It release any of the delete any of the data it allocated.
CODColumn::~CODColumn()
{
	if( m_pData )
		delete[] m_pData;

	if( m_pIndicator )
		delete[] m_pIndicator;

	if( m_pCurrentData )
		delete[] m_pCurrentData;
}

// Return the name of the column
const CString& CODColumn::GetName() const
{
	return m_Name;
}

// Return the name of the column type
CString CODColumn::GetTypeName() const
{
	return m_TypeName;
}

// Return the SQL/ODBC data type
short CODColumn::GetDataType() const
{
	short OdbcType = 0;

	OdbcType = CODUtil::GetSqlBindType( m_SqlType );

	return OdbcType;
}

// Return the native 'c' data type
short CODColumn::GetNativeType() const
{
	short NativeBindType = 0;

	NativeBindType = CODUtil::GetNativeBindType( m_NativeType );

	return NativeBindType;
}

EnumSqlType CODColumn::GetSqlDataType() const
{
	return m_SqlType;
}

EnumNativeType CODColumn::GetNativeDataType() const
{
	return m_NativeType;
}

// Return the scale
short CODColumn::GetScale() const
{
	return m_SqlScale;
}

// Return the native sql length
int CODColumn::GetLength() const
{
	return m_SqlLength;
}

// Return the native 'c' length
int CODColumn::GetNativeLength() const
{
	return m_NativeLength;
}

// Retrieve the ODBC or SQL type of the column	SQL_CHAR, SQL_VARCHAR
bool CODColumn::GetDataType( short* pDatatype ) const
{
	bool bRetval = false;

	if( pDatatype && m_SqlType != eSqlUnknown )
	{
		*pDatatype = ( short ) m_SqlType;

		bRetval = true;
	}

	return bRetval;
}

// Retrieve the native or 'C' data type of the column	SQL_C_CHAR, SQL_C_LONG, SQL_C_SHORT etc
bool CODColumn::GetNativeType( short* pNativeType ) const
{
	bool bRetval = false;

	if( m_NativeType )
	{
		*pNativeType = CODUtil::GetNativeBindType( m_NativeType );
		bRetval = true;
	}

	return bRetval;
}

// Retrieve nullablity of the column.
//   - true if nulls are allowed
//   - false if nulls are not allowed
// this function should be const, and only return the value of the current m_IsNull variable can have another function to query from the DB.
bool CODColumn::IsNull() const
{
	return m_IsNull;
}

// Is the column not null
bool CODColumn::NotNull() const
{
	return !IsNull();
}

// Describe the column
// This function queries the ODBC driver for the column's meta data type, name, size, precision, scale, typename
bool CODColumn::Describe()
{
	bool bRetval = false;

	// With Oracle ODBC driver use SQLDescribeCol instead of SQLGetColAttribute
	SQLTCHAR Name[ 64 ] = { 0 };
	SQLSMALLINT NameLength = 0;
	SQLTCHAR TypeName[ 64 ] = { 0 };
	SQLSMALLINT Type = 0;
	SQLLEN Precision = 0;
	SQLSMALLINT Scale = 0;
	SQLULEN Length = 0;
	SQLSMALLINT Nullable = 0;
	SQLRETURN rc = SQL_SUCCESS;

	// 	The Column type name cannot be retrieved with SQLDescribeCol so set it to nulls
	rc = ::SQLDescribeCol( m_pStatement->GetHandle(),
		m_Ordinal,
		Name,
		64,
		&NameLength,
		&Type,
		&Length,
		&Scale,
		&Nullable );

	if( !SQL_SUCCEEDED( rc ) )
	{
		m_pStatement->HandleError();
	}
	else
	{
		// At this point we have retrieved all the columns meta data
		m_Name = Name;
		m_SqlType = CODUtil::GetSqlEnum( Type );
		m_SqlLength = ( int ) Length;
		m_SqlScale = Scale;
		m_OdbcPrecision = ( SQLSMALLINT ) Precision;
		m_Nullable = ( int ) Nullable;

		EnumNativeType NativeType = eNativeUnknown;
		SQLINTEGER nNativeLength = 0;

		// Now convert the SQL_... data type in to the SQL_C... data type
		if( CODUtil::SqlToNative( m_SqlType, m_SqlLength, m_SqlScale, &NativeType, &nNativeLength ) )
		{
			bRetval = true;

			// save the native type and size
			m_NativeType = NativeType;
			m_NativeLength = nNativeLength;
			m_NativePrecision = m_OdbcPrecision;
			m_NativeScale = m_SqlScale;
		}
	}

	return bRetval;
}

// This function allocates memory for the column's data storage.
// THe ODBC driver binds to these buffers and places the columns data in this memory as data is fetched from the database.
bool CODColumn::Allocate()
{
	bool bRetval = false;

	// Allocate the column storage.
	//     - storage allocated for the bulk data and the current data

	// If the column hold character data
	//     - allocate is as type TCHAR or wchar
	//     - add space for the null terminator in the m_pCurrentData
	if( m_NativeType == eSqlChar )
	{
		m_pData = new TCHAR[ ( m_NativeLength + 1 ) * m_BulkFetchCount ];
		m_pCurrentData = new TCHAR[ m_NativeLength + 1 ];
	}
	else
	{
		m_pData = new BYTE[ m_NativeLength * m_BulkFetchCount ];
		m_pCurrentData = new BYTE[ m_NativeLength ];
	}

	// Allocate the indicator array
	m_pIndicator = new SQLLEN[ m_BulkFetchCount ];

	if( m_pData == nullptr || m_pCurrentData == nullptr || m_pIndicator == nullptr )
	{
		if( m_pData )
			delete m_pData;

		if( m_pCurrentData )
			delete m_pCurrentData;

		if( m_pIndicator )
			delete m_pIndicator;

		return false;
	}
	else
	{
		memset( m_pIndicator, 0, sizeof( SQLLEN ) * m_BulkFetchCount );
		bRetval = true;
	}

	return bRetval;
}

// Bind the memory that was allocated for the column.  This tells the ODBC driver where to put data as it is fetched from the database
bool CODColumn::Bind()
{
	bool bRetval = false;
	SQLRETURN rc = SQL_ERROR;

	int BindLength = m_NativeLength;
	if( m_NativeType == eSqlChar )
		BindLength++;

	// Bind the column to the statement
	SQLSMALLINT NativeBindType = CODUtil::GetNativeBindType( m_NativeType );

	rc = ::SQLBindCol( m_pStatement->GetHandle(),
		m_Ordinal,
		NativeBindType,
		m_pData,
		BindLength,
		m_pIndicator );

	if( m_NativeType == eNativeNumeric )
	{
		// Numeric requires that we specify scale and precision, otherwise
		// the value will be returned as the driver default for scale and
		// precision.  This was observed to be NUMERIC(38,0)

		// To do this get application row descriptor handle and using
		// that handle set the type, scale, precision, data and indicator

		// We could do this for other column types but SQLBindCol works	with other types.
		SQLHANDLE hdesc = SQL_NULL_HANDLE;

		rc = ::SQLGetStmtAttr( m_pStatement->GetHandle(),
			SQL_ATTR_APP_ROW_DESC,
			&hdesc,
			0,
			nullptr );

		if( !SQL_SUCCEEDED( rc ) )
		{
			m_pStatement->HandleError();
		}
		else
		{
			// Set the columns type SQL_NUMERIC or SQL_DECIMAL
			rc = ::SQLSetDescField( hdesc,
				m_Ordinal,
				SQL_DESC_TYPE,
				( SQLPOINTER ) NativeBindType,
				0 );

			if( !SQL_SUCCEEDED( rc ) )
			{
				m_pStatement->HandleError();
				return false;
			}

			// Set the numeric precision
			rc = ::SQLSetDescField( hdesc,
				m_Ordinal,
				SQL_DESC_PRECISION,
				( SQLPOINTER ) m_NativePrecision,
				0 );

			if( !SQL_SUCCEEDED( rc ) )
			{
				m_pStatement->HandleError();
				return false;
			}

			// Set the numeric scale
			rc = ::SQLSetDescField( hdesc,
				m_Ordinal,
				SQL_DESC_SCALE,
				( SQLPOINTER ) m_NativeScale,
				0 );

			if( !SQL_SUCCEEDED( rc ) )
			{
				m_pStatement->HandleError();
				return false;
			}

			// Set the numeric data
			rc = ::SQLSetDescField( hdesc,
				m_Ordinal,
				SQL_DESC_DATA_PTR,
				( SQLPOINTER ) m_pData,
				0 );

			if( !SQL_SUCCEEDED( rc ) )
			{
				m_pStatement->HandleError();
				return false;
			}

			// Set the numeric indicator
			rc = ::SQLSetDescField( hdesc,
				m_Ordinal,
				SQL_DESC_INDICATOR_PTR,
				( SQLPOINTER ) m_pIndicator,
				0 );

			if( !SQL_SUCCEEDED( rc ) )
			{
				m_pStatement->HandleError();
				return false;
			}
		}
	}

	bRetval = true;

	return bRetval;
}

// Fetch the data from the driver.
bool CODColumn::Fetch( int nRow )
{
	bool bRetval = true;

	// Assert if there is no indicator. We did not allocate an indicator for blobs.
	ATLASSERT( m_pIndicator );

	if( m_pIndicator[ nRow ] == SQL_NULL_DATA )
	{
		m_IsNull = true;
	}
	else
	{
		m_IsNull = false;

		// For text data fill the current buffer.
		switch( m_NativeType )
		{
			case eNativeChar:
				if( m_SqlType == eSqlVarchar || m_SqlType == eSqlWideVarchar )
					memset( m_pCurrentData, _T( '\0' ), m_NativeLength );
				else
					memset( m_pCurrentData, _T( ' ' ), m_NativeLength );

				( ( TCHAR* ) m_pCurrentData )[ m_NativeLength ] = _T( '\0' );
				memcpy( m_pCurrentData, &( ( ( BYTE* ) m_pData )[ ( nRow ) * ( m_NativeLength + sizeof( TCHAR ) ) ] ), m_pIndicator[ nRow ] );
				break;

			case eNativeNumeric:
				memset( m_pCurrentData, 0, m_NativeLength );
				memcpy( m_pCurrentData, &( ( ( BYTE* ) m_pData )[ ( nRow ) *m_NativeLength ] ), m_pIndicator[ nRow ] );
				break;

			default:
				memcpy( m_pCurrentData, &( ( ( BYTE* ) m_pData )[ ( nRow ) *m_NativeLength ] ), m_pIndicator[ nRow ] );
				break;
		}
	}

	return bRetval;
}

// Data is converted from the native type into a CString. This function may be deprecated.
bool CODColumn::GetDataAsString( CString* pString ) const
{
	if( pString == nullptr )
		return false;

	if( m_IsNull == true )
	{
		*pString = _T( "" );
		return true;
	}

	switch( m_NativeType )
	{
		case eNativeChar:
			return GetString( pString );

		case eNativeTinyInt:
			return CODUtil::TinyIntToString( *( BYTE* ) m_pCurrentData, pString );

		case eNativeShort:
			return CODUtil::SmallIntToString( *( short* ) m_pCurrentData, pString );

		case eNativeInt:
			return CODUtil::IntToString( *( int* ) m_pCurrentData, pString );

		case eNativeBigInt:
			return CODUtil::BigIntToString( *( INT64* ) m_pCurrentData, pString );

		case eNativeFloat:
			return CODUtil::FloatToString( *( float* ) m_pCurrentData, pString );

		case eNativeDouble:
			return CODUtil::DoubleToString( *( double* ) m_pCurrentData, pString );

		case eNativeNumeric:
			return CODUtil::NumericToString( ( SQL_NUMERIC_STRUCT* ) m_pCurrentData, pString );

		case eNativeDate:
			return CODUtil::DateToString( ( SQL_DATE_STRUCT* ) m_pCurrentData, pString );

		case eNativeDateTime:
			return CODUtil::DateTimeToString( ( SQL_TIMESTAMP_STRUCT* ) m_pCurrentData, pString );

			// unsupported conversions
		case eNativeBinary:
		case eNativeBlob:
		case eNativeGuid:
		case eNativeUnknown:
			throw std::exception( "Incorrect NativeType for CODColumn::GetDataAsString" );
			break;
	}

	return false;
}

// Data is converted from the native type into a double. Conversion is not done on string, blob, clob, binary
bool CODColumn::GetDataAsDouble( double* pDouble ) const
{
	bool bRetval = true;

	if( !pDouble )
		return false;

	if( IsNull() )
	{
		*pDouble = NULL_VALUE;
		return true;
	}

	switch( m_NativeType )
	{
		case eNativeTinyInt:
			*pDouble = ( double ) *( BYTE* ) m_pCurrentData;
			break;

		case eNativeShort:
			*pDouble = ( double ) *( short* ) m_pCurrentData;
			break;

		case eNativeInt:
			*pDouble = ( double ) *( int* ) m_pCurrentData;
			break;

		case eNativeBigInt:
			*pDouble = ( double ) *( __int64* ) m_pCurrentData;
			break;

		case eNativeFloat:
			*pDouble = ( double ) *( float* ) m_pCurrentData;
			break;

		case eNativeDouble:
			*pDouble = *( double* ) m_pCurrentData;
			break;

		case eNativeNumeric:
			bRetval = GetNumeric( pDouble );
			break;

		case eNativeDate:
			bRetval = GetDate( pDouble );
			break;

		case eNativeDateTime:
			bRetval = GetDateTime( pDouble );
			break;

		default:
			throw std::exception( "Incorrect NativeType for CODColumn::GetDataAsDouble" );
	}

	return bRetval;
}
// Data is converted from the native type into a integer. Conversion is not done on string, blob, clob, binary
bool CODColumn::GetDataAsInteger( int* pInteger ) const
{
	bool bRetval = false;

	if( !pInteger )
		return false;

	if( IsNull() )
	{
		*pInteger = NULL_VALUE;
		return true;
	}

	switch( m_NativeType )
	{
		case eNativeTinyInt:
			*pInteger = *( BYTE* ) m_pCurrentData;
			bRetval = true;
			break;

		case eNativeShort:
			*pInteger = *( short* ) m_pCurrentData;
			bRetval = true;
			break;

		case eNativeInt:
			*pInteger = *( int* ) m_pCurrentData;
			bRetval = true;
			break;

		case eNativeBigInt:
			*pInteger = ( int ) *( __int64* ) m_pCurrentData;
			bRetval = true;
			break;

		case eNativeFloat:
			*pInteger = ( int ) ( *( ( float* ) m_pCurrentData ) + 0.5 );
			bRetval = true;
			break;

		case eNativeDouble:
			*pInteger = ( int ) ( *( ( double* ) m_pCurrentData ) + 0.5 );
			bRetval = true;
			break;

		case eNativeNumeric:
		{
			double Double;
			if( GetNumeric( &Double ) )
			{
				*pInteger = ( int ) &Double;
				bRetval = true;
			}
		}
		break;

		// unsupported conversions
		case eNativeBinary:
		case eNativeBlob:
		case eNativeClob:
		case eNativeGuid:
		case eNativeChar:
		case eNativeDate:
		case eNativeDateTime:
			throw std::exception( "Incorrect NativeType for CODColumn::GetDataAsInteger" );
			break;

		case eNativeUnknown:
			throw std::exception( "Incorrect NativeType for CODColumn::GetDataAsInteger" );
			break;
	}

	return bRetval;
}

// Get the data that was fetched from a TINYINT column
bool CODColumn::GetTinyInt( _TSCHAR* pByte ) const
{
	bool bRetval = false;

	if( pByte == nullptr )
		return bRetval;

	if( m_IsNull )
		return bRetval;

	if( m_NativeType != eNativeTinyInt )
	{
		throw std::exception( "Incorrect NativeType for CODColumn::GetTinyInt" );
	}

	ATLASSERT( m_pCurrentData );

	*pByte = *( BYTE* ) m_pCurrentData;

	bRetval = true;

	return bRetval;
}

// Get the data that was fetched from a SMALLINT column
bool CODColumn::GetShort( short* pShort ) const
{
	bool bRetval = false;

	if( pShort == nullptr )
		return bRetval;

	if( m_IsNull )
	{
		*pShort = NULL_VALUE;
		return true;
	}

	if( m_NativeType != eNativeShort )
	{
		throw std::exception( "Incorrect NativeType for CODColumn::GetShort" );
	}

	ATLASSERT( m_pCurrentData );

	*pShort = *( short* ) m_pCurrentData;

	bRetval = true;

	return bRetval;
}

// Get the data that was fetched from an INTEGER column
bool CODColumn::GetInt( int* pLong ) const
{
	bool bRetval = false;

	if( pLong == nullptr )
		return bRetval;

	if( m_IsNull )
	{
		*pLong = NULL_VALUE;
		return true;
	}

	if( m_NativeType == eNativeInt )
	{
		ATLASSERT( m_pCurrentData );

		*pLong = *( int* ) m_pCurrentData;

		bRetval = true;
	}
	else if( m_NativeType == eNativeNumeric )
	{
		SQL_NUMERIC_STRUCT numeric;

		if( GetNumeric( &numeric ) )
		{
			/*
			Now convert the numeric to an int
			*/
			bRetval = CODUtil::NumericToInt( &numeric, pLong );
		}
	}
	else if( m_NativeType == eNativeTinyInt )
	{
		ATLASSERT( m_pCurrentData );

		*pLong = *( BYTE* ) m_pCurrentData;

		bRetval = true;
	}
	else if( m_NativeType == eNativeShort )
	{
		ATLASSERT( m_pCurrentData );

		*pLong = *( short* ) m_pCurrentData;

		bRetval = true;
	}
	else if( m_NativeType == eNativeBigInt )
	{
		ATLASSERT( m_pCurrentData );

		*pLong = ( int ) *( __int64* ) m_pCurrentData;

		bRetval = true;
	}
	else
	{
		throw std::exception( "Incorrect NativeType for CODColumn::GetInt" );
	}

	return bRetval;
}

bool CODColumn::GetByType( int* pInt ) const
{
	return GetInt( pInt );
}

bool CODColumn::GetByType( bool* pInt ) const
{
	int nValue;
	bool bRet = GetInt( &nValue );
	if( bRet )
	{
		*pInt = nValue == 1 ? true : false;
	}
	return bRet;
}
// Get the data that was fetched from a BIGINT column
bool CODColumn::GetBigInt( INT64* pBigint ) const
{
	bool bRetval = false;

	if( pBigint == nullptr )
		return bRetval;

	if( m_IsNull )
	{
		*pBigint = NULL_VALUE;
		return true;
	}

	if( m_NativeType != eNativeBigInt )
	{
		throw std::exception( "Incorrect NativeType for CODColumn::GetBigInt" );
	}

	ATLASSERT( m_pCurrentData );

	*pBigint = *( INT64* ) m_pCurrentData;

	bRetval = true;

	return bRetval;
}

// Get the data that was fetched from a REAL (aka 'C' float) column
bool CODColumn::GetFloat( float* pFloat ) const
{
	bool bRetval = false;

	if( pFloat == nullptr )
		return bRetval;

	if( m_IsNull )
	{
		*pFloat = NULL_VALUE;
		return true;
	}

	if( m_NativeType != eNativeFloat )
	{
		throw std::exception( "Incorrect NativeType for CODColumn::GetFloat" );
	}

	ATLASSERT( m_pCurrentData );

	*pFloat = *( float* ) m_pCurrentData;

	bRetval = true;

	return bRetval;
}


bool CODColumn::GetByType( double* pDouble ) const
{
	return GetDouble( pDouble );
}

// Get the data that was fetched from a float/double column*/
bool CODColumn::GetDouble( double* pDouble ) const
{
	bool bRetval = false;

	if( pDouble == nullptr )
		return bRetval;

	if( m_IsNull )
	{
		*pDouble = NULL_VALUE;
		return true;
	}

	if( m_NativeType != eNativeDouble )
	{
		throw std::exception( "Incorrect NativeType for CODColumn::GetDouble" );
	}

	ATLASSERT( m_pCurrentData );

	*pDouble = *( double* ) m_pCurrentData;

	bRetval = true;

	return bRetval;
}

// Get the data that was fetched from a DATETIME column
bool CODColumn::GetDateTime( SQL_TIMESTAMP_STRUCT* pDateTime ) const
{
	bool bRetval = false;

	if( pDateTime == nullptr )
		return bRetval;

	if( m_IsNull )
		return bRetval;

	switch( m_NativeType )
	{
		case eNativeDateTime:
			memcpy( pDateTime, m_pCurrentData, sizeof( SQL_TIMESTAMP_STRUCT ) );
			bRetval = true;
			break;

		case eNativeDate:
		{
			SQL_DATE_STRUCT Date;
			memcpy( &Date, m_pCurrentData, sizeof( SQL_DATE_STRUCT ) );

			pDateTime->year = Date.year;
			pDateTime->month = Date.month;
			pDateTime->day = Date.day;

			pDateTime->hour = 0;
			pDateTime->minute = 0;
			pDateTime->second = 0;
			pDateTime->fraction = 0;

			bRetval = true;
		}
		break;

		// unsupported conversions
		case eNativeChar:
		case eNativeTinyInt:
		case eNativeShort:
		case eNativeInt:
		case eNativeBigInt:
		case eNativeFloat:
		case eNativeDouble:
		case eNativeNumeric:
		case eNativeBlob:
		case eNativeClob:
		case eNativeBinary:
		case eNativeGuid:
			throw std::exception( "Incorrect NativeType for CODColumn::GetDateTime" );
			break;

		case eNativeUnknown:
			throw std::exception( "Incorrect NativeType for CODColumn::GetDateTime" );
			break;
	}

	return bRetval;
}

// Get the date column as a SYSTEMTIME
bool CODColumn::GetDate( SYSTEMTIME* pSystemTime ) const
{
	bool bRetval = false;

	bRetval = GetDateTime( pSystemTime );
	if( bRetval )
	{
		pSystemTime->wHour = 0;
		pSystemTime->wMinute = 0;
		pSystemTime->wSecond = 0;
		pSystemTime->wMilliseconds = 0;
	}

	return bRetval;
}

// Get the date column as a SQL_DATE_STRUCT
bool CODColumn::GetDate( SQL_DATE_STRUCT* pDate ) const
{
	bool bRetval = false;

	if( pDate )
	{
		SQL_TIMESTAMP_STRUCT DateTime;

		bRetval = GetDateTime( &DateTime );

		pDate->year = DateTime.year;
		pDate->month = DateTime.month;
		pDate->day = DateTime.day;

		bRetval = true;
	}

	return bRetval;
}

// Get the date column as a variant time
bool CODColumn::GetDate( double* pVariantTime ) const
{
	bool bRetval = false;

	double VariantTime;
	bRetval = GetDateTime( &VariantTime );
	if( bRetval )
	{
		// remove the fractional
		*pVariantTime = ( int ) VariantTime;
	}

	return bRetval;
}

// Get a date time value into a SYSTEMTIME structure The Day of Week value is set to 0;
bool CODColumn::GetDateTime( SYSTEMTIME* pSystemTime ) const
{
	bool bRetval = false;

	if( pSystemTime == nullptr )
		return false;

	SQL_TIMESTAMP_STRUCT Timestamp;
	bRetval = GetDateTime( &Timestamp );

	if( bRetval )
	{
		pSystemTime->wYear = Timestamp.year;
		pSystemTime->wMonth = Timestamp.month;
		pSystemTime->wDay = Timestamp.day;
		pSystemTime->wHour = Timestamp.hour;
		pSystemTime->wMinute = Timestamp.minute;
		pSystemTime->wSecond = Timestamp.second;
		pSystemTime->wMilliseconds = ( WORD ) ( Timestamp.fraction / 1000000 );
		pSystemTime->wDayOfWeek = CODUtil::GetDayOfWeek( pSystemTime->wYear, pSystemTime->wMonth, pSystemTime->wDay );

		bRetval = true;
	}

	return bRetval;
}

bool CODColumn::GetDateTime( double* pVariantTime ) const
{
	bool bRetval = false;

	if( pVariantTime == nullptr )
		return bRetval;

	if( m_IsNull )
	{
		*pVariantTime = NULL_VALUE;
		return true;
	}

	if( !( m_NativeType == eNativeDate || m_NativeType == eNativeDateTime ) )
	{
		throw std::exception( "Incorrect NativeType for CODColumn::GetDateTime" );
	}

	// Get the datetime value from as a SYSTEMTIME; the convert it to a variant time
	SYSTEMTIME SystemTime;
	GetDateTime( &SystemTime );
	if( SystemTimeToVariantTimeEx( &SystemTime, pVariantTime ) )
		bRetval = true;

	return bRetval;
}

// Get a numeric column value as a double
bool CODColumn::GetNumeric( double* pDouble ) const
{
	bool bRetval = false;

	if( pDouble == nullptr )
		return false;

	SQL_NUMERIC_STRUCT numeric;

	if( GetNumeric( &numeric ) )
	{
		bRetval = CODUtil::NumericToDouble( &numeric, pDouble );
	}

	return bRetval;
}

// Get a numeric column value as a interger
bool CODColumn::GetNumeric( int* pInt ) const
{
	bool bRetval = false;

	if( pInt == nullptr )
		return false;

	SQL_NUMERIC_STRUCT numeric;

	if( GetNumeric( &numeric ) )
	{
		// Only convert the numeric to an int if the scale is 0
		bRetval = CODUtil::NumericToInt( &numeric, pInt );
	}

	return bRetval;
}

// Get the data that was fetched NUMERIC/DECIMAL column
bool CODColumn::GetNumeric( SQL_NUMERIC_STRUCT* pNumeric ) const
{
	bool bRetval = false;

	if( pNumeric == nullptr )
		return bRetval;

	if( m_IsNull )
		return bRetval;

	if( m_NativeType != eNativeNumeric )
	{
		throw std::exception( "Incorrect NativeType for CODColumn::GetNumeric" );
	}

	ATLASSERT( m_pCurrentData );

	memcpy( pNumeric, m_pCurrentData, sizeof( SQL_NUMERIC_STRUCT ) );

	bRetval = true;

	return bRetval;
}

LPVOID CODColumn::GetRawData() const
{
	return m_pCurrentData;
}


bool CODColumn::GetByType( CString* pString ) const
{
	return GetString( pString );
}

bool CODColumn::GetString( CString* pString ) const
{
	bool bRetval = false;

	if( pString == nullptr )
	{
		return bRetval;
	}

	if( m_IsNull )
	{
		pString->Empty();
		return true;
	}

	ATLASSERT( m_pCurrentData );

	if( m_NativeType == eNativeChar )
	{
		pString->SetString( ( LPCTSTR ) m_pCurrentData );
		bRetval = true;
	}
	else
	{
		throw std::exception( "Incorrect NativeType for CODColumn::GetString" );
	}

	return bRetval;
}


// Is the column a character column
bool CODColumn::IsChar() const
{
	bool bRetval = false;

	bRetval = CODUtil::IsChar( m_NativeType );

	return bRetval;
}
