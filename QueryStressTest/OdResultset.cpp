#include "pch.h"
#include "OdResultSet.h"
#include "OdColumn.h"

#include <stdexcept>
#include <sstream>

CODResultset::CODResultset()
{
	m_nColumn = 0;
	m_NumRows = 0;
}

CODResultset::~CODResultset()
{
	Destroy();
}

// This function create a resultset object. A result set always belongs to a statement.
// This function constructs CODColumn object and each column will describe, allocate and bind itself
bool CODResultset::Create( CIStatement* pStatement, SQLSMALLINT nColumn, SQLINTEGER nBulkRows )
{
	bool bRetval = false;

	if( pStatement == nullptr )
		return false;

	if( nColumn <= 0 )
		return false;

	// Assume success here but set it to false if an error occures	when a colum is error created, allocated or bound.
	bRetval = true;

	for( SQLSMALLINT nIndex = 0; nIndex < nColumn; nIndex++ )
	{
		auto* pColumn = new CODColumn( pStatement, nIndex, nBulkRows );

		if( pColumn &&
			pColumn->Describe() &&
			pColumn->Allocate() &&
			pColumn->Bind() )
		{
			m_ColumnArray.push_back( pColumn );
			const CString& csName = pColumn->GetName();
			m_ColumnNameMap[ csName ] = pColumn;
		}
		else
		{
			if( pColumn )
				delete pColumn;

			bRetval = false;
		}
	}

	// If an error occurred destroy the result set.
	if( bRetval == false )
		Destroy();
	else
		m_nColumn = nColumn;

	return bRetval;
}

// This function destroys a result set object
bool CODResultset::Destroy()
{
	bool bRetval = false;

	auto nCount = m_ColumnArray.size();

	if( nCount )
	{
		for( unsigned int nIndex = 0; nIndex < nCount; nIndex++ )
		{
			delete m_ColumnArray[ nIndex ];
		}

		m_ColumnArray.clear();
		m_ColumnNameMap.clear();
		bRetval = true;
	}

	return bRetval;
}

// Return the number of columns in the result set
int CODResultset::GetColumnCount() const
{
	int nColumn = m_ColumnArray.size();

	return nColumn;
}

// This function returns by index a column object in the resultset
CIColumn* CODResultset::GetColumn( unsigned int nIndex ) const
{
	CODColumn* pColumn = nullptr;

	if( nIndex < m_ColumnArray.size() )
		pColumn = m_ColumnArray[ nIndex ];

	return ( CIColumn* ) pColumn;
}

CIColumn* CODResultset::GetColumn( LPCTSTR ColumnName ) const
{
	auto pColumnPtr = m_ColumnNameMap.find( ColumnName );
    if( pColumnPtr == m_ColumnNameMap.end() )
        return nullptr;

	return pColumnPtr->second;
}

void CODResultset::GetColumn( CIColumn*& pColumn, LPCTSTR columnName ) const
{
    auto pColumnPtr = m_ColumnNameMap.find( columnName );
	if( pColumnPtr == m_ColumnNameMap.end() )
	{
		std::string msg = "Column does not exist";
		throw new std::exception( msg.c_str() );
	}

	pColumn = pColumnPtr->second;
}

// This function call the columns fetch function.
// This function is called from CODStatement class and tells each column to copy its data from the bound memory into is non bound memory.
bool CODResultset::Fetch( SQLINTEGER nBulkRowIndex )
{
	bool bRetval = true;

	for( SQLSMALLINT nIndex = 0; nIndex < m_nColumn; nIndex++ )
	{
		bRetval = m_ColumnArray[ nIndex ]->Fetch( nBulkRowIndex );
	}

	return bRetval;
}

