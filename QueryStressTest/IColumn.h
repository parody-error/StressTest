#pragma once

#include <atlstr.h>
#include <sqltypes.h>
#include <vector>

class CIStatement;

class CIColumn
{
public:

	virtual LPVOID GetRawData() const = 0;

	std::vector<CString>* m_associatedStringColumnData = nullptr;
	std::vector<int>* m_associatedIntColumnData = nullptr;
	std::vector<double>* m_associatedDoubleColumnData = nullptr;

	CIColumn() {};
	virtual ~CIColumn() {};

	virtual const CString& GetName() const = 0;
	virtual CString GetTypeName() const = 0;                               //  Get the name of the column type
	virtual short GetDataType() const = 0;                                 //  Get the column data type
	virtual short GetNativeType() const = 0;                               //  Get the column native 'c' data type
	virtual short GetScale() const = 0;                                    //  Get the column scale
	virtual int GetLength() const = 0;                                     //  Get the column data type
	virtual int GetNativeLength() const = 0;                               //  Get the column native 'c' data type
	virtual bool GetDataType( short* pDatatype ) const = 0;                 //  Get the columns datatype
	virtual bool GetNativeType( short* pNativeType ) const = 0;             //  Get the columns native data type
	virtual bool IsNull() const = 0;                                       //  Is the column's value sql null
	virtual bool NotNull() const = 0;                                      //  Is the column's value not null (ie there is a value)
	virtual bool GetDataAsString( CString* pString ) const = 0;            //  Get the data as a string
	virtual bool GetDataAsDouble( double* pDouble ) const = 0;              //  Get the data as a double
	virtual bool GetDataAsInteger( int* pInt ) const = 0;                   //  Get the data as a integer
	virtual bool GetString( CString* pString ) const = 0;                  //  sql char, nchar, varchar, nvarchar, etc
	virtual bool GetTinyInt( _TSCHAR* pByte ) const = 0;               //  sql tinyint
	virtual bool GetShort( short* pShort ) const = 0;                      //  sql smallint
	virtual bool GetInt( int* pLong ) const = 0;                           //  sql integer
	virtual bool GetBigInt( INT64* pBigint ) const = 0;                    //  sql bigint
	virtual bool GetFloat( float* pFloat ) const = 0;                      //  sql real
	virtual bool GetDouble( double* pDouble ) const = 0;                   //  sql float, sql double
	virtual bool GetDate( SYSTEMTIME* pSystemtime ) const = 0;             //  sql date
	virtual bool GetDate( SQL_DATE_STRUCT* pDate ) const = 0;               //  sql date
	virtual bool GetDate( double* dbldatetime ) const = 0;                  //  sql date
	virtual bool GetDateTime( SYSTEMTIME* pSystemtime ) const = 0;         //  sql datetime
	virtual bool GetDateTime( SQL_TIMESTAMP_STRUCT* pDateTime ) const = 0;     //  sql datetime
	virtual bool GetDateTime( double* dbldatetime ) const = 0;              //  sql datetime
	virtual bool GetNumeric( SQL_NUMERIC_STRUCT* pNumeric ) const = 0;     //  sql numeric
	virtual bool GetNumeric( double* pDouble ) const = 0;                   //  sql numeric as a double
	virtual bool GetNumeric( int* pInt ) const = 0;                  		//  sql numeric as an integer

	virtual bool GetByType( CString* pString ) const = 0;
	virtual bool GetByType( double* pDouble ) const = 0;
	virtual bool GetByType( bool* pBool ) const = 0;
	virtual bool GetByType( int* pInt ) const = 0;

	virtual bool Describe() = 0;
	virtual bool Allocate() = 0;
	virtual bool Bind() = 0;
	virtual bool Fetch( int nRow ) = 0;
	virtual bool IsChar() const = 0;
};
