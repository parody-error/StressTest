#pragma once

#include <map>
#include <vector>

#include <atlstr.h>
#include <sqltypes.h>

class CIColumn;
class CODColumn;
class CIStatement;

class CODResultset
{
public:
	CODResultset();
	~CODResultset();
	virtual bool Create( CIStatement* pStatement, SQLSMALLINT nColumn, SQLINTEGER nBulkRows = 1 );
	virtual bool Destroy();
	virtual int GetColumnCount() const;
	virtual CIColumn* GetColumn( unsigned int nIndex ) const;

	// A nullptr is returned if the column name cannot be found in the result set;
	virtual CIColumn* GetColumn( LPCTSTR ColumnName ) const;

	// An exception will be thrown if the column name cannot be found in the result set.
	virtual void GetColumn( CIColumn*& pColumn, LPCTSTR columnName ) const;

	virtual bool Fetch( SQLINTEGER nBulkRowIndex = 1 );

private:
	const int m_maxInstancesPerColumn = 26;
	SQLSMALLINT m_nColumn;
	SQLINTEGER  m_NumRows;
	std::vector<CODColumn*> m_ColumnArray;
	std::map<CString, CODColumn*> m_ColumnNameMap;
};
