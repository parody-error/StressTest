/*********************************************************************************
**
**  Compact Open Access Library
**
********************************************************************************/

#pragma once

#include <sqltypes.h>
#include "OdEnums.h"
#include <atlstr.h>


class CODUtil
{
protected:
	static const int s_BufferSize = 64;

public:
	CODUtil();
	~CODUtil();

	/*
	**  Static helper functions used by ODColumm, ODParam, and MetaCatalog
	*/
	static bool IsChar( EnumNativeType Type );
	static bool IsBlob( EnumNativeType Type );
	static bool IsClob( EnumNativeType Type );
	static bool IsGuid( EnumSqlType Type, int Size );

	/*
	**  Conversion functions that will convert the stated type to a CString
	*/
	static bool    CharToString( LPCSTR lpchar, CString* pString );
	static bool    VarcharToString( LPCSTR lpchar, CString* pString );
	static bool    WideVarcharToString( LPCWSTR lpwidevarchar, CString* pString );
	static bool    WideCharToString( LPCWSTR lpwidechar, CString* pString );
	static bool    TinyIntToString( INT8 byteVal, CString* pString );
	static bool    SmallIntToString( INT16 shortVal, CString* pString );
	static bool    IntToString( INT32 intVal, CString* pString );
	static bool    BigIntToString( INT64 bigintVal, CString* pString );
	static bool    FloatToString( float floatVal, CString* pString );
	static bool    DoubleToString( double doubleVal, CString* pString );
	static bool    NumericToString( SQL_NUMERIC_STRUCT* pNumeric, CString* pString );
	static bool    DateToString( SQL_DATE_STRUCT* pDateStruct, CString* pString );
	static bool    DateTimeToString( SQL_TIMESTAMP_STRUCT* pDatetime, CString* pString );

	/*
	**  Conversion function that will convert a CString into the stated type
	*/
	static bool    StringToChar( const CString& strVal, LPSTR lpchar, int length );
	static bool    StringToVarchar( const CString& strVal, LPSTR lpvarchar, int length );
	static bool    StringToWideChar( const CString& strVal, LPWSTR lpwidechar, int length );
	static bool    StringToWideVarchar( const CString& strVal, LPWSTR lpwidevarchar, int length );
	static bool    StringToTinyInt( const CString& strVal, INT8* byte_value );
	static bool    StringToSmallInt( const CString& strVal, INT16* short_value );
	static bool    StringToInt( const CString& strVal, INT32* int_value );
	static bool    StringToBigInt( const CString& strVal, INT64* bigint_value );
	static bool    StringToFloat( const CString& strVal, float* float_value );
	static bool    StringToDouble( const CString& strVal, double* double_value );
	static bool    StringToNumeric( const CString& strVal, SQL_NUMERIC_STRUCT* pNumeric );
	static bool    StringToDate( const CString& strVal, SQL_DATE_STRUCT* pDate );
	static bool    StringToDateTime( const CString& strVal, SQL_TIMESTAMP_STRUCT* pDatetime );

	/*
	**  Numeric to Double conversion routines
	*/
	static bool    NumericToDouble( SQL_NUMERIC_STRUCT* pNumeric, double* pDouble );
	static bool    NumericToInt( SQL_NUMERIC_STRUCT* pNumeric, int* pInt );
	static bool    DoubleToNumeric( double Double, SQL_NUMERIC_STRUCT* pNumeric, short nPrecision, short nScale );
	static bool    IntToNumeric( int intval, SQL_NUMERIC_STRUCT* pNumeric, short nPrecision, short nScale );

	/*
	**  Conversion function that will map the ODBC SQL_<type> to a 'C' SQL_C_<type>.
	*/
	static bool     SqlToNative( EnumSqlType SqlType, SQLINTEGER SqlLength, SQLSMALLINT SqlScale, EnumNativeType* pNativeType, SQLINTEGER* nNativeLength );

	/*
	**	Static helper datatype conversion
	*/
	// Convert an SQL_ constant to an EnumSqlType
	static EnumSqlType GetSqlEnum( int OdbcType );
	// Convert an EnumSqlType to an SQL_ type
	static SQLSMALLINT GetSqlBindType( EnumSqlType SqlType );
	// Convert an EnumNativeType to an SQL_C_ type
	static SQLSMALLINT GetNativeBindType( EnumNativeType NativeType );


	/*
	**  Calculate day of week given the Year, Month and Day.
	**  Used in ODColumn::GetDateTime ( SYSTEMTIME * pSystemTime )
	*/
	static unsigned short  GetDayOfWeek( int Year, int Month, int Day );

	/*
	**  Create a temp file.  Returns the handle to the
	**  the temp file and fills in the path if supplied.
	*/
	static HANDLE   CreateTempFile( CString* pcsTempFilename );

private:

	/*
	**  Internal function to take apart an numeric struct into more usable parts.
	*/
	static bool ExtractNumeric( SQL_NUMERIC_STRUCT* pNumeric, unsigned __int64* pValue, bool* pbNegative, short* pScale );
};
