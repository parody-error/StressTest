#include "pch.h"
#include "OdUtil.h"
#include <sql.h>
#include <sqlucode.h>
#include <cfloat>

CODUtil::CODUtil()
{
}

CODUtil::~CODUtil()
{
}

// Given an ODBC data type information get the native C type and size.
bool CODUtil::SqlToNative( EnumSqlType SqlType, SQLINTEGER SqlLength, SQLSMALLINT SqlScale, EnumNativeType* pNativeType, SQLINTEGER* pNativeLength )
{
	bool bRetval = true;

	if( SqlType == eSqlUnknown )
		return false;

	EnumNativeType NativeType = eNativeUnknown;
	SQLINTEGER  nNativeLength = 0;

	switch( SqlType )
	{
		case eSqlBinary:
			NativeType = eNativeBinary;
			nNativeLength = SqlLength;
			break;

		case eSqlVarbinary:
			// 	If the length is 0 then consider this to be a blob.
			// 	This is the case with SQL Server VARBINARY(MAX)
			if( SqlLength == 0 )
				NativeType = eNativeBlob;
			else
				NativeType = eNativeBinary;
			nNativeLength = SqlLength;
			break;

		case eSqlBlob:
			NativeType = eNativeBlob;
			nNativeLength = 0;
			break;

		case eSqlClob:
		case eSqlWideClob:
			NativeType = eNativeClob;
			nNativeLength = 0;
			break;

		case eSqlChar:
		case eSqlWideChar:
			NativeType = eNativeChar;
			nNativeLength = SqlLength;
			break;

		case eSqlVarchar:
		case eSqlWideVarchar:
			// 	If the size of the column is 0 then consider the column to be a clob.
			// 	This is the case with SQL Server VARCHAR(MAX) and NVARCHAR(MAX)
			if( SqlLength == 0 )
				NativeType = eNativeClob;
			else
				NativeType = eNativeChar;
			nNativeLength = SqlLength;
			break;

		case eSqlTinyInt:
			NativeType = eNativeTinyInt;
			nNativeLength = sizeof( BYTE );
			break;

		case eSqlBit:
			NativeType = eNativeTinyInt;
			nNativeLength = sizeof( BYTE );	 // This value was determined by trial and error. This seems to work,
			// but not completely certain. The native length must match the native type
			// or else a head corruption will occur
			break;

		case eSqlShort:
			NativeType = eNativeShort;
			nNativeLength = sizeof( INT16 );
			break;

		case eSqlInt:
			NativeType = eNativeInt;
			nNativeLength = sizeof( INT32 );
			break;

		case eSqlBigInt:
			NativeType = eNativeBigInt;
			nNativeLength = sizeof( INT64 );
			break;

		case eSqlNumeric:
			//  The following section of code is one method of translating SQL_NUMERIC/SQL_DECIMAL
			//  into native C types such as short, int, float and double.
			if( SqlScale == 0 )
			{
				NativeType = eNativeInt;
				nNativeLength = sizeof( int );
			}
			else
			{
				NativeType = eNativeDouble;
				nNativeLength = sizeof( double );
			}
			break;

		case eSqlDouble:
			NativeType = eNativeDouble;
			nNativeLength = sizeof( double );
			break;

		case eSqlReal:
			NativeType = eNativeFloat;
			nNativeLength = sizeof( float );
			break;

		case eSqlDate:
			if( SqlLength == 10 || SqlLength == 16 )
			{
				NativeType = eNativeDate;
				nNativeLength = sizeof( SQL_DATE_STRUCT );
			}
			else
			{
				NativeType = eNativeDateTime;
				nNativeLength = sizeof( SQL_TIMESTAMP_STRUCT );
			}
			break;

		case eSqlOldDate:
		case eSqlOldDateTime:
		case eSqlDateTime:
			NativeType = eNativeDateTime;
			nNativeLength = sizeof( SQL_TIMESTAMP_STRUCT );
			break;

		case eSqlGuid:
			NativeType = eNativeGuid;    // SQL Server 'uniqueidentifier', MS-Access 'Replication ID'
			nNativeLength = sizeof( GUID );
			break;

		case eSqlUnknown:
			break;
	}

	if( NativeType == eNativeUnknown )
	{
		bRetval = false;
	}
	else
	{
		*pNativeType = NativeType;
		*pNativeLength = nNativeLength;
	}

	return bRetval;
}

// 	Is this a blob column (includes clob types)
bool CODUtil::IsBlob( EnumNativeType NativeType )
{
	return ( NativeType == eNativeClob || NativeType == eNativeBlob );
}

//  Is this a clob column (excludes binary blobs)
bool CODUtil::IsClob( EnumNativeType NativeType )
{
	return ( NativeType == eNativeClob );
}

// Is this a char column ( does include clobs)
bool CODUtil::IsChar( EnumNativeType NativeType )
{
	return ( NativeType == eNativeChar || NativeType == eNativeClob );
}

// 	Is this column a guid column Or potentially a guid
bool CODUtil::IsGuid( EnumSqlType Type, int Size )
{
	return ( Type == eSqlGuid ||
		( Type == eSqlChar && Size == sizeof( GUID ) ) ||
		( Type == eSqlVarbinary && Size == sizeof( GUID ) ) ||
		( Type == eSqlBinary && Size == sizeof( GUID ) ) );
}

//  Deprecated
bool CODUtil::CharToString( LPCSTR lpchar, CString* pString )
{
	bool bRetval = true;

	if( pString )
	{
#ifndef _UNICODE
		LPCSTR psz = lpchar;
#else
		CA2W psz( lpchar );
#endif
		* pString = psz;
	}

	return bRetval;
}

// Deprecated
bool CODUtil::VarcharToString( LPCSTR lpchar, CString* pString )
{
	bool bRetval = true;

	if( pString )
	{
#ifndef _UNICODE
		LPCSTR psz = lpchar;
#else
		CA2W psz( lpchar );
#endif
		* pString = psz;
	}

	return bRetval;
}

// Deprecated
bool CODUtil::WideCharToString( LPCWSTR lpwidechar, CString* pString )
{
	bool bRetval = true;

	if( pString )
	{
#ifndef _UNICODE
		CW2A psz( lpwidechar );
#else
		LPCWSTR psz = lpwidechar;
#endif
		* pString = psz;
	}

	return bRetval;
}

// Deprecated
bool CODUtil::WideVarcharToString( LPCWSTR lpwidevarchar, CString* pString )
{
	bool bRetval = true;

	if( pString )
	{
#ifndef _UNICODE
		CW2A psz( lpwidevarchar );
#else
		LPCWSTR psz = lpwidevarchar;
#endif
		* pString = psz;
	}

	return bRetval;
}

bool CODUtil::TinyIntToString( INT8 byte_val, CString* pString )
{
	bool bRetval = false;

	if( pString )
	{
		TCHAR szBuffer[ s_BufferSize ];
		errno_t err = _itot_s( ( int ) byte_val, szBuffer, s_BufferSize, 10 );

		if( err != -1 )
		{
			*pString = szBuffer;
			bRetval = true;
		}
	}
	return bRetval;
}

bool CODUtil::SmallIntToString( INT16 shortVal, CString* pString )
{
	bool bRetval = false;

	if( pString )
	{
		TCHAR szBuffer[ s_BufferSize ];
		errno_t err = _itot_s( ( int ) shortVal, szBuffer, s_BufferSize, 10 );

		if( err != -1 )
		{
			*pString = szBuffer;
			bRetval = true;
		}
	}

	return bRetval;
}

bool CODUtil::IntToString( INT32 intVal, CString* pString )
{
	bool bRetval = false;

	if( pString )
	{
		TCHAR szBuffer[ s_BufferSize ];
		errno_t err = _itot_s( ( int ) intVal, szBuffer, s_BufferSize, 10 );

		if( err != -1 )
		{
			*pString = szBuffer;
			bRetval = true;
		}
	}

	return bRetval;
}

bool CODUtil::BigIntToString( INT64 bigintVal, CString* pString )
{
	bool bRetval = true;

	if( pString )
	{
		TCHAR szBuffer[ s_BufferSize ];
		errno_t err = _i64tot_s( ( INT64 ) bigintVal, szBuffer, s_BufferSize, 10 );

		if( err != -1 )
		{
			*pString = szBuffer;
			bRetval = true;
		}
	}

	return bRetval;
}

bool CODUtil::FloatToString( float fltVal, CString* pString )
{
	bool bRetval = false;

	if( pString )
	{
		TCHAR szBuffer[ s_BufferSize ];
		// FLT_DIG + FLT_ROUNDS == 7 so that is what is used for the precision in the string format.
		// If the format string is altered then the DoubleToString	function should also be examined.
		int err = _stprintf_s( szBuffer, s_BufferSize, _T( "%1.7e" ), fltVal );

		if( err != -1 )
		{
			*pString = szBuffer;
			bRetval = true;
		}
	}

	return bRetval;
}

bool CODUtil::DoubleToString( double dblVal, CString* pString )
{
	bool bRetval = false;

	if( pString )
	{
		TCHAR szBuffer[ s_BufferSize ];
		// DBL_DIG + _DBL_ROUNDS == 16 so that is what is used for	the precision in the string format.
		// If the format string is  altered then the DoubleToString function should also be examined.
		int err = _stprintf_s( szBuffer, s_BufferSize, _T( "%1.16e" ), dblVal );

		if( err != -1 )
		{
			*pString = szBuffer;
			bRetval = true;
		}
	}

	return bRetval;
}

bool CODUtil::NumericToString( SQL_NUMERIC_STRUCT* pNumeric, CString* pString )
{
	bool bRetval = false;

	if( pNumeric == nullptr ||
		pString == nullptr )
	{
		return bRetval;
	}

	bRetval = true;

	unsigned __int64 nValue = 0;
	bool bNegative = false;
	short nScale = 0;

	ExtractNumeric( pNumeric, &nValue, &bNegative, &nScale );

	// Calc the divisor to the scale value
	__int64 Div = 1;
	__int64 Mul = 1;

	if( nScale < 0 )
		for( int s = 0; s > nScale; s-- )
			Mul *= 10;
	else
		for( int s = 0; s < nScale; s++ )
			Div *= 10;

	// Create a format string that retains the scale. note that this uses %I64d for __int64
	TCHAR format_string[ s_BufferSize ];
	if( nScale > 0 )
		if( bNegative )
			_stprintf_s( format_string, s_BufferSize, _T( "-%%I64d.%%0%dI64d" ), nScale );
		else
			_stprintf_s( format_string, s_BufferSize, _T( "%%I64d.%%0%dI64d" ), nScale );
	else
		if( bNegative )
			_tcscpy_s( format_string, 32, _T( "-%I64d" ) );
		else
			_tcscpy_s( format_string, 32, _T( "%I64d" ) );

	if( Div > 1 || Mul > 1 )
		pString->Format( format_string, ( __int64 ) ( nValue * Mul ) / Div, nValue % Div );
	else
		pString->Format( format_string, nValue );

	return bRetval;
}

bool CODUtil::StringToChar( const CString& strVal, LPSTR lpchar, int odbclength )
{
	bool bRetval = false;

	int len = strVal.GetLength();

#ifdef _UNICODE
	CW2A psz( strVal );
#else
	LPCSTR psz = ( LPCSTR ) strVal;
#endif

	if( len <= odbclength && lpchar )
	{
		// Char are intialized to space with a null terminator
		memset( lpchar, _T( ' ' ), odbclength );
		lpchar[ odbclength ] = _T( '\0' );
		memcpy( lpchar, psz, len );

		bRetval = true;
	}

	return bRetval;
}

bool CODUtil::StringToVarchar( const CString& strVal, LPSTR lpvarchar, int odbclength )
{
	bool bRetval = false;

	int len = strVal.GetLength();

#ifdef _UNICODE
	CW2A psz( strVal );
#else
	LPCSTR psz = ( LPCSTR ) strVal;
#endif

	if( len <= odbclength && lpvarchar )
	{
		memset( lpvarchar, _T( '\0' ), odbclength );
		memcpy( lpvarchar, psz, len );

		bRetval = true;
	}

	return bRetval;
}

bool CODUtil::StringToWideChar( const CString& strVal, LPWSTR lpwchar, int odbclength )
{
	bool bRetval = false;

	int len = strVal.GetLength();

#ifdef _UNICODE
	LPCWSTR psz = strVal;
#else
	CA2W psz( strVal );
#endif

	if( len <= odbclength && lpwchar )
	{
		// Char are intialized to space with a null terminator
		wmemset( lpwchar, L' ', odbclength );
		lpwchar[ odbclength ] = L'\0';
		wmemcpy( lpwchar, psz, len );

		bRetval = true;
	}

	return bRetval;
}

bool CODUtil::StringToWideVarchar( const CString& strVal, LPWSTR lpwvarchar, int odbclength )
{
	bool bRetval = false;

	int len = strVal.GetLength();

#ifdef _UNICODE
	LPCWSTR psz = strVal;
#else
	CA2W psz( strVal );
#endif

	if( len <= odbclength && lpwvarchar )
	{
		// Varchar are intialized to nul's
		wmemset( lpwvarchar, L'\0', odbclength );
		wmemcpy( lpwvarchar, psz, len );

		bRetval = true;
	}

	return bRetval;
}

bool CODUtil::StringToTinyInt( const CString& strVal, INT8* byteVal )
{
	bool bRetval = false;

	if( byteVal )
	{
		int int_val = _tstoi( strVal );

		if( errno == 0 )
		{
			*byteVal = ( BYTE ) int_val;

			bRetval = true;
		}
	}

	return bRetval;
}

bool CODUtil::StringToSmallInt( const CString& strVal, INT16* shortVal )
{
	bool bRetval = false;

	if( shortVal )
	{
		int int_val = _tstoi( strVal );

		if( errno == 0 )
		{
			*shortVal = ( short ) int_val;
			bRetval = true;
		}
	}

	return bRetval;
}

bool CODUtil::StringToInt( const CString& strVal, INT32* intVal )
{
	bool bRetval = false;

	if( intVal )
	{
		int int_val = _tstoi( strVal );

		if( errno == 0 )
		{
			*intVal = int_val;
			bRetval = true;
		}
	}

	return bRetval;
}

bool CODUtil::StringToBigInt( const CString& strVal, INT64* bigintVal )
{
	bool bRetval = false;

	if( bigintVal )
	{
		INT64 int_val = _ttoi64( strVal );

		if( errno == 0 )
		{
			*bigintVal = int_val;
			bRetval = true;
		}
	}

	return bRetval;
}

bool CODUtil::StringToFloat( const CString& strVal, float* fltVal )
{
	bool bRetval = false;

	if( fltVal )
	{
		double float8_val = _tstof( strVal );

		if( errno == 0 &&
			float8_val > FLT_MIN &&
			float8_val < FLT_MAX )
		{
			*fltVal = ( float ) float8_val;
			bRetval = true;
		}
	}

	return bRetval;
}

bool CODUtil::StringToDouble( const CString& strVal, double* dblVal )
{
	bool bRetval = false;

	if( dblVal )
	{
		double float8_val = _tstof( strVal );

		if( errno == 0 )
		{
			bRetval = true;
			*dblVal = float8_val;
		}
	}

	return bRetval;
}

bool CODUtil::StringToNumeric( const CString& strVal, SQL_NUMERIC_STRUCT* pNumeric )
{
	bool bRetval = false;

	UNREFERENCED_PARAMETER( strVal );

	memset( pNumeric, 0, sizeof( SQL_NUMERIC_STRUCT ) );

	pNumeric->precision = 1;
	pNumeric->scale = 0;
	pNumeric->sign = 1;
	pNumeric->val[ 0 ] = _T( '\3' );

	return bRetval;
}

bool CODUtil::StringToDate( const CString& strVal, SQL_DATE_STRUCT* pDate )
{
	bool bRetval = false;

	if( pDate )
	{
		int year = 0;
		int month = 0;
		int day = 0;

		int count = _stscanf_s( ( LPCTSTR ) strVal,
			_T( "%d/%d/%d" ),
			&year,
			&month,
			&day );

		if( count > 0 &&
			year >= 0 && year <= 9999 &&
			month >= 1 && month <= 12 &&
			day >= 1 && day <= 31 )
		{
			pDate->year = ( SQLUSMALLINT ) year;
			pDate->month = ( SQLUSMALLINT ) month;
			pDate->day = ( SQLUSMALLINT ) day;

			bRetval = true;
		}
	}

	return bRetval;
}

bool CODUtil::StringToDateTime( const CString& strVal, SQL_TIMESTAMP_STRUCT* pDatetime )
{
	bool bRetval = false;

	if( pDatetime )
	{
		int year = 0;
		int month = 0;
		int day = 0;
		int hour = 0;
		int minute = 0;
		int second = 0;
		int fraction = 0;

		int count = _stscanf_s( ( LPCTSTR ) strVal,
			_T( "%d/%d/%d %d:%d:%d.%d" ),
			&year,
			&month,
			&day,
			&hour,
			&minute,
			&second,
			&fraction );

		if( count > 0 &&
			year >= 0 && year <= 9999 &&
			month >= 1 && month <= 12 &&
			day >= 1 && day <= 31 &&
			hour >= 0 && hour <= 23 &&
			minute >= 0 && minute <= 59 &&
			second >= 0 && second <= 59 )
		{
			pDatetime->year = ( SQLUSMALLINT ) year;
			pDatetime->month = ( SQLUSMALLINT ) month;
			pDatetime->day = ( SQLUSMALLINT ) day;
			pDatetime->hour = ( SQLUSMALLINT ) hour;
			pDatetime->minute = ( SQLUSMALLINT ) minute;
			pDatetime->second = ( SQLUSMALLINT ) second;
			pDatetime->fraction = ( SQLUSMALLINT ) fraction;

			bRetval = true;
		}
	}

	return bRetval;
}

bool CODUtil::DateToString( SQL_DATE_STRUCT* pDate, CString* pString )
{
	bool bRetval = false;

	if( pDate && pString )
	{
		pString->Format(
			_T( "%04d/%02d/%02d" ),
			pDate->year,
			pDate->month,
			pDate->day );

		bRetval = true;
	}

	return bRetval;
}

bool CODUtil::DateTimeToString( SQL_TIMESTAMP_STRUCT* pDatetime, CString* pString )
{
	bool bRetval = false;

	if( pDatetime && pString )
	{
		pString->Format(
			_T( "%04d/%02d/%02d %02d:%02d:%02d.%03d" ),
			pDatetime->year,
			pDatetime->month,
			pDatetime->day,
			pDatetime->hour,
			pDatetime->minute,
			pDatetime->second,
			pDatetime->fraction / 1000000 );

		bRetval = true;

	}

	return bRetval;
}

// Convert an SQL_NUMERIC_STRUCT 'numeric', 'decimal' to C double
bool CODUtil::NumericToDouble( SQL_NUMERIC_STRUCT* pNumeric, double* pDouble )
{
	if( pNumeric == nullptr || pDouble == nullptr )
	{
		return false;
	}

	unsigned __int64 nValue = 0;
	bool bNegative = false;
	short nScale = 0;

	ExtractNumeric( pNumeric, &nValue, &bNegative, &nScale );

	// Calc the divisor to the scale value
	__int64 Div = 1;
	__int64 Mul = 1;

	if( nScale < 0 )
		for( int s = 0; s > nScale; s-- )
			Mul *= 10;
	else
		for( int s = 0; s < nScale; s++ )
			Div *= 10;

	// Calculate the final value
	*pDouble = ( ( double ) nValue * Mul ) / Div;

	if( bNegative )
		*pDouble = 0 - *pDouble;

	return true;
}

// Convert an SQL_NUMERIC_STRUCT 'numeric', 'decimal' to C int
bool CODUtil::NumericToInt( SQL_NUMERIC_STRUCT* pNumeric, int* pInt )
{
	if( pNumeric == nullptr ||
		pInt == nullptr )
	{
		return false;
	}

	unsigned __int64 nValue = 0;
	bool bNegative = false;
	short nScale = 0;

	ExtractNumeric( pNumeric, &nValue, &bNegative, &nScale );

	// Calc the divisor to the scale value
	__int64 Div = 1;
	__int64 Mul = 1;

	if( nScale < 0 )
		for( int s = 0; s > nScale; s-- )
			Mul *= 10;
	else
		for( int s = 0; s < nScale; s++ )
			Div *= 10;

	// Calculate the final value
	*pInt = ( int ) ( nValue * Mul / Div );

	if( bNegative )
		*pInt = 0 - *pInt;

	return true;
}

// Convert C double to an SQL_NUMERIC_STRUCT 'numeric', 'decimal'
bool CODUtil::DoubleToNumeric( double Double, SQL_NUMERIC_STRUCT* pNumeric, short nPrecision, short nScale )
{
	bool bRetval = false;

	if( pNumeric == nullptr )
		return bRetval;

	memset( pNumeric, 0, sizeof( SQL_NUMERIC_STRUCT ) );

	// Save the precision, scale and sign
	pNumeric->precision = ( SQLCHAR ) nPrecision;
	pNumeric->scale = ( SQLCHAR ) nScale;
	pNumeric->sign = Double > 0 ? 1 : 0;

	// Calculate mulitplier for the scale
	int Mul = 1;
	while( nScale-- )
	{
		Mul *= 10;
	}

	if( Double < 0 )
		Double = 0 - Double;

	// Adjust for the multiplier/scale
	unsigned __int64 nValue = ( INT64 ) ( Double * Mul );

	for( int nIndex = 0; nIndex < SQL_MAX_NUMERIC_LEN / 2; nIndex += 2 )
	{
		BYTE Byte1 = ( BYTE ) ( nValue % 256 );
		nValue >>= 8;
		BYTE Byte2 = ( BYTE ) ( nValue % 256 );
		nValue >>= 8;
		pNumeric->val[ nIndex ] = Byte1;
		pNumeric->val[ nIndex + 1 ] = Byte2;
	}

	bRetval = true;

	return bRetval;
}

bool CODUtil::IntToNumeric( int intval, SQL_NUMERIC_STRUCT* pNumeric, short nPrecision, short nScale )
{
	bool bRetval = false;

	if( pNumeric == nullptr )
		return bRetval;

	memset( pNumeric, 0, sizeof( SQL_NUMERIC_STRUCT ) );

	// Save the precision, scale and sign
	pNumeric->precision = ( SQLCHAR ) nPrecision;
	pNumeric->scale = ( SQLCHAR ) nScale;
	pNumeric->sign = intval > 0 ? 1 : 0;

	// Calculate mulitplier for the scale
	__int64 Div = 1;
	__int64 Mul = 1;

	if( nScale < 0 )
		for( int s = 0; s > nScale; s-- )
			Div *= 10;
	else
		for( int s = 0; s < nScale; s++ )
			Mul *= 10;

	if( intval < 0 )
		intval = 0 - intval;

	unsigned __int64 nValue = ( INT64 ) ( intval ) *Mul / Div;

	for( int nIndex = 0; nIndex < SQL_MAX_NUMERIC_LEN / 2; nIndex += 2 )
	{
		BYTE Byte1 = ( BYTE ) ( nValue % 256 );
		nValue >>= 8;
		BYTE Byte2 = ( BYTE ) ( nValue % 256 );
		nValue >>= 8;
		pNumeric->val[ nIndex ] = Byte1;
		pNumeric->val[ nIndex + 1 ] = Byte2;
	}

	bRetval = true;

	return bRetval;
}

// Return the day of week given the Year, Month and Day
// Sunday = 0. Used when getting the system time.
unsigned short CODUtil::GetDayOfWeek( int Year, int Month, int Day )
{
	if( Month < 3 )
	{
		Month += 12;
		Year--;
	}

	int dow = Day +
		( 2 * Month ) + int( 6 * ( Month + 1 ) / 10 )
		+ Year + int( Year / 4 ) - int( Year / 100 ) + int( Year / 400 ) + 1;

	return unsigned short( dow % 7 );
}

bool CODUtil::ExtractNumeric( SQL_NUMERIC_STRUCT* pNumeric, unsigned __int64* pValue, bool* pbNegative, short* pScale )
{
	bool bRetval = false;

	__int64 Value = 0;
	__int64 Last = 1;
	BYTE LoByte = 0;
	BYTE HiByte = 0;

	for( int nIndex = 0; nIndex < SQL_MAX_NUMERIC_LEN; nIndex++ )
	{
		LoByte = ( pNumeric->val[ nIndex ] & 0x0F );
		HiByte = ( pNumeric->val[ nIndex ] & 0xF0 ) >> 4;

		Value += Last * LoByte;
		Last <<= 4;
		Value += Last * HiByte;
		Last <<= 4;
	}

	*pValue = Value;
	*pbNegative = pNumeric->sign == 1 ? false : true;
	*pScale = pNumeric->scale;

	return bRetval;
}

// Return a handle to file that is created in the windows temporary directory
// If the pcsTempFilename arguement is supplied then this will also contain the full path to the temp file that was created
HANDLE CODUtil::CreateTempFile( CString* pcsTempFilename )
{
	HANDLE hTempFile = INVALID_HANDLE_VALUE;

	// get the temp file directory
	TCHAR szTempDirectory[ _MAX_PATH + 1 ];
	::GetTempPath( _MAX_PATH, szTempDirectory );

	DWORD dwLastError = ERROR_ALREADY_EXISTS;
	TCHAR szTempFileName[ _MAX_PATH + 1 ] = { 0 };

	//  loop while the file exists if we fail to create it.
	//  this is done to prevent us from being affected by other application temp  file creation process
	while( hTempFile == INVALID_HANDLE_VALUE && dwLastError == ERROR_ALREADY_EXISTS )
	{
		UINT nResult = ::GetTempFileName( szTempDirectory, _T( "TMP" ), 0, szTempFileName );
		hTempFile = ::CreateFile( szTempFileName,
			GENERIC_WRITE,           // open for reading
			FILE_SHARE_READ,        // share for reading
			nullptr,                    // default security
			CREATE_ALWAYS,           // existing file only
			FILE_ATTRIBUTE_TEMPORARY,// temp file
			nullptr );                  // no attr. template

		if( hTempFile == INVALID_HANDLE_VALUE )
			dwLastError = ::GetLastError();
		else
			dwLastError = ERROR_SUCCESS;
	}

	// failed to create temp file for a reason other that 'file exists'
	if( dwLastError != ERROR_SUCCESS )
		return nullptr;

	if( pcsTempFilename )
		*pcsTempFilename = szTempFileName;

	return hTempFile;
}

// Return the EnumSqlType from the ODBC Sql Type
EnumSqlType CODUtil::GetSqlEnum( int OdbcType )
{
	EnumSqlType SqlType = eSqlUnknown;

	switch( OdbcType )
	{
		case SQL_CHAR:
			SqlType = eSqlChar;
			break;
		case  SQL_VARCHAR:
			SqlType = eSqlVarchar;
			break;
		case SQL_WCHAR:
			SqlType = eSqlWideChar;
			break;
		case SQL_WVARCHAR:
			SqlType = eSqlWideVarchar;
			break;
		case SQL_VARBINARY:
			SqlType = eSqlVarbinary;
			break;
		case SQL_DATE:
			SqlType = eSqlOldDate;
			break;
		case SQL_TIMESTAMP:
			SqlType = eSqlOldDateTime;
			break;
		case SQL_TYPE_DATE:
			SqlType = eSqlDate;
			break;
		case SQL_TYPE_TIMESTAMP:
			SqlType = eSqlDateTime;
			break;
		case SQL_TINYINT:
			SqlType = eSqlTinyInt;
			break;
		case SQL_SMALLINT:
			SqlType = eSqlShort;
			break;
		case SQL_INTEGER:
			SqlType = eSqlInt;
			break;
		case SQL_BIGINT:
			SqlType = eSqlBigInt;
			break;
		case SQL_DECIMAL:
		case SQL_NUMERIC:
			SqlType = eSqlNumeric;
			break;
		case SQL_REAL:
			SqlType = eSqlReal;
			break;
		case SQL_FLOAT:
		case SQL_DOUBLE:
			SqlType = eSqlDouble;
			break;
		case SQL_GUID:
			SqlType = eSqlGuid;
			break;
		case SQL_LONGVARBINARY:
			SqlType = eSqlBlob;
			break;
		case SQL_LONGVARCHAR:
			SqlType = eSqlClob;
			break;
		case SQL_WLONGVARCHAR:
			SqlType = eSqlWideClob;
			break;
		case SQL_BINARY:
			SqlType = eSqlBinary;
			break;
		case SQL_BIT:
			SqlType = eSqlBit;
			break;
		default:
			break;
	}

	return SqlType;
}

// Return ODBC SQL type from the EnumSqlType
SQLSMALLINT CODUtil::GetSqlBindType( EnumSqlType SqlType )
{
	SQLSMALLINT OdbcType = SQL_UNKNOWN_TYPE;

	switch( SqlType )
	{
		case eSqlChar:
			OdbcType = SQL_CHAR;
			break;
		case eSqlVarchar:
			OdbcType = SQL_VARCHAR;
			break;
		case eSqlWideChar:
			OdbcType = SQL_WCHAR;
			break;
		case eSqlWideVarchar:
			OdbcType = SQL_WVARCHAR;
			break;
		case eSqlVarbinary:
			OdbcType = SQL_VARBINARY;
			break;
		case eSqlOldDate:
			OdbcType = SQL_DATE;
			break;
		case eSqlOldDateTime:
			OdbcType = SQL_TIMESTAMP;
			break;
		case eSqlDate:
			OdbcType = SQL_TYPE_DATE;
			break;
		case eSqlDateTime:
			OdbcType = SQL_TYPE_TIMESTAMP;
			break;
		case eSqlBit:
			OdbcType = SQL_BIT;
			break;
		case eSqlTinyInt:
			OdbcType = SQL_TINYINT;
			break;
		case eSqlShort:
			OdbcType = SQL_SMALLINT;
			break;
		case eSqlInt:
			OdbcType = SQL_INTEGER;
			break;
		case eSqlBigInt:
			OdbcType = SQL_BIGINT;
			break;
		case eSqlNumeric:
			OdbcType = SQL_NUMERIC;
			break;
		case eSqlReal:
			OdbcType = SQL_REAL;
			break;
		case eSqlDouble:
			OdbcType = SQL_DOUBLE;
			break;
		case eSqlGuid:
			OdbcType = SQL_GUID;
			break;
		case eSqlBlob:
			OdbcType = SQL_LONGVARBINARY;
			break;
		case eSqlClob:
			OdbcType = SQL_LONGVARCHAR;
			break;
		case eSqlWideClob:
			OdbcType = SQL_WLONGVARCHAR;
			break;
		case eSqlBinary:
			OdbcType = SQL_BINARY;
			break;
		case eSqlUnknown:
			OdbcType = SQL_UNKNOWN_TYPE;
			break;
	}

	return OdbcType;
}

// Return the native bind type given an EnumNativeType
SQLSMALLINT CODUtil::GetNativeBindType( EnumNativeType NativeType )
{
	SQLSMALLINT NativeBindType = 0;

	switch( NativeType )
	{
		case eNativeChar:
			NativeBindType = SQL_C_TCHAR;
			break;
		case eNativeDate:
			NativeBindType = SQL_C_TYPE_DATE;
			break;
		case eNativeDateTime:
			NativeBindType = SQL_C_TYPE_TIMESTAMP;
			break;
		case eNativeTinyInt:
			NativeBindType = SQL_C_TINYINT;
			break;
		case eNativeShort:
			NativeBindType = SQL_C_SHORT;
			break;
		case eNativeInt:
			NativeBindType = SQL_C_LONG;
			break;
		case eNativeBigInt:
			NativeBindType = SQL_C_SBIGINT;
			break;
		case eNativeFloat:
			NativeBindType = SQL_C_FLOAT;
			break;
		case eNativeDouble:
			NativeBindType = SQL_C_DOUBLE;
			break;
		case eNativeNumeric:
			NativeBindType = SQL_C_NUMERIC;
			break;
		case eNativeGuid:
			NativeBindType = SQL_C_GUID;
			break;
		case eNativeBinary:
			NativeBindType = SQL_C_BINARY;
			break;
		case eNativeBlob:
			NativeBindType = SQL_C_BINARY;
			break;
		case eNativeClob:
			NativeBindType = SQL_C_TCHAR;
			break;
		case eNativeUnknown:
			NativeBindType = SQL_UNKNOWN_TYPE;
			break;
	}

	return NativeBindType;
}

