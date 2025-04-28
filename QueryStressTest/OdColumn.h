/*********************************************************************************
**
**  Compact Open Access Library
**
*********************************************************************************/

#pragma once

#include "IColumn.h"
#include "OdEnums.h"
#include <atlstr.h>
#include <sqltypes.h>

class CGuid;
class CIStatement;

class CODColumn : public CIColumn
{
public:
	CODColumn( CIStatement* pStatement, SQLSMALLINT nNumber, SQLINTEGER nBulkRows = 1 );
	~CODColumn();

	const CString& GetName() const;
	CString GetTypeName() const;
	short GetDataType() const;
	short GetNativeType() const;
	EnumSqlType GetSqlDataType() const;
	EnumNativeType GetNativeDataType() const;
	short GetScale() const;
	int GetLength() const;
	int GetNativeLength() const;
	bool GetDataType( short* pDatatype ) const;
	bool GetNativeType( short* pNativeType ) const;
	bool IsNull() const;
	bool NotNull() const;

	/*
	**  Generic data retrieval to get the column data as a CString
	*/
	bool GetDataAsString( CString* pString ) const;
	bool GetDataAsDouble( double* pDouble ) const;
	bool GetDataAsInteger( int* pInt ) const;

	/*
	**  Specific data retrieval to get the column data in the correct type
	**
	**  The column class contain each columns data that is part of the result set.
	*/
	LPVOID GetRawData() const;
	bool GetString( CString* pString ) const;
	bool GetTinyInt( _TSCHAR* pByte ) const;
	bool GetShort( short* pShort ) const;
	bool GetInt( int* pLong ) const;
	bool GetBigInt( INT64* pBigint ) const;
	bool GetFloat( float* pFloat ) const;
	bool GetDouble( double* pDouble ) const;
	bool GetDate( SYSTEMTIME* pSystemTime ) const;
	bool GetDate( SQL_DATE_STRUCT* pDate ) const;
	bool GetDate( double* pVariantTime ) const;
	bool GetDateTime( SYSTEMTIME* pSystemTime ) const;
	bool GetDateTime( SQL_TIMESTAMP_STRUCT* pDateTime ) const;
	bool GetDateTime( double* pVariantTime ) const;
	bool GetNumeric( SQL_NUMERIC_STRUCT* pNumeric ) const;
	bool GetNumeric( double* pDouble ) const;
	bool GetNumeric( int* pInt ) const;
	virtual bool GetByType( CString* pString ) const;
	virtual bool GetByType( double* pDouble ) const;
	virtual bool GetByType( int* pInt ) const;
	virtual bool GetByType( bool* pBool ) const;

	bool IsChar() const;

	/*
	**  Public but not exported - used by CODStatement
	*/
	bool Describe();
	bool Allocate();
	bool Bind();
	bool Fetch( int nRow );

protected:
	//  Statement to which the column belongs
	CIStatement* m_pStatement;		//  pointer to CIStatement

	// Column Order...
	SQLSMALLINT		m_Ordinal;          //  Column Number (0 based, ODBC '?' place holder is 1 based)

	// Column Type...
	EnumSqlType		m_SqlType;			//  Data type - as returned by ODBC driver
	EnumNativeType	m_NativeType;		//  Data type - as understood by COALAPI (may transform some data types to be one internal type as they need to be handled the same way)
	CString			m_TypeName;			//  Column type name for example 'smallmoney' in SQL Server

	// Data Length...
	SQLINTEGER		m_SqlLength;         //  Data length - as returned by ODBC driver
	int				m_NativeLength;		 //  Data Length - as understood by COALAPI e.g. short->sizeof(INT16) & int->sizeof(INT32)

	// Scale...
	SQLSMALLINT		m_SqlScale;			//  Scale - as returned by ODBC driver
	short			m_NativeScale;		//  Scale - as understood by COALAPI

	// Precision....
	SQLSMALLINT		m_OdbcPrecision;	//  Precision - as returned by ODBC driver
	short			m_NativePrecision;  //  Precision - as understood by COALAPI

	// Column required Property...
	int				m_Nullable;			//  Is this Column defined to be "NULLABLE" in the database?

	// There was nothing in the database
	bool			m_IsNull;           //  This column returned no value when read from the database

	// Bulk Fetch Count....
	int				m_BulkFetchCount;   //  number of columns to bulk fetch

	//  Column data
	LPVOID			m_pData;            //  pointer to the data that is allocated

	SQLLEN* m_pIndicator;       //  pointer for column indicators

	// Microsoft Access related
	bool			m_bMSJet;			//  flag to indicate MS Access Jet ODBC driver.

private:
	CString			m_Name;

	LPVOID			m_pCurrentData;     //  pointer into m_pData for bulk fetch

};
