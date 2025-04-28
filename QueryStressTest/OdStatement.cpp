#include "pch.h"
#include "OdStatement.h"
#include "OdResultSet.h"

#include <sqltypes.h>
#include <sqlext.h>
#include <stdexcept>
#include <string>

CODStatement::CODStatement( SQLHANDLE hDbc )
{
    m_BulkFetchCount = 0;
    m_BulkRowIndex = 0;
    m_RowsFetched = 0;
    m_RowsAffected = 0;
    m_SumRowsFetched = 0;

    m_pResultset = nullptr;
    m_pBulkRowStatus = nullptr;

    m_bEndOfData = false;

    m_hdbc = hDbc;
    m_hstmt = SQL_NULL_HANDLE;
}

CODStatement::~CODStatement()
{

}

bool CODStatement::Release()
{
    bool bRetval = false;
    if( m_hstmt )
    {
        SQLRETURN rc = ::SQLFreeStmt( m_hstmt, SQL_CLOSE );
        if( SQL_SUCCEEDED( rc ) )
        {
            m_hstmt = nullptr;
            bRetval = true;
        }
        else
        {
            HandleError();
        }
    }
    return bRetval;
}

bool CODStatement::DescribeTables( const char* database, const char* schema )
{
    bool bRetval = false;

    SQLTCHAR* DatabaseName = ( SQLTCHAR* ) ( LPCTSTR ) database;
    SQLTCHAR* SchemaName = ( SQLTCHAR* ) ( LPCTSTR ) schema;

    if( !AllocateStatement() )
        return false;

    SQLRETURN rc = ::SQLTables(
        m_hstmt,
        DatabaseName,
        SQL_NTS,
        SchemaName,
        SQL_NTS,
        nullptr,
        0,
        ( SQLTCHAR* ) _T( "'TABLE', 'VIEW', 'SYNONYM'" ),
        SQL_NTS
    );

    if( SQL_SUCCEEDED( rc ) )
    {
        SQLSMALLINT nColumns = 0;
        rc = ::SQLNumResultCols( m_hstmt, &nColumns );

        if( SQL_SUCCEEDED( rc ) )
        {
            bRetval = CreateResultset( nColumns );
        }
    }
    else
    {
        HandleError();
    }

    return bRetval;
}

bool CODStatement::Fetch()
{
    bool bRetval = false;

    if( m_BulkFetchCount == 0 )
    {
        m_bEndOfData = true;
        return bRetval;
    }

    if( m_hstmt && m_bEndOfData == false )
    {
        SQLRETURN rc = SQL_SUCCESS;

        if( m_BulkRowIndex == 0 || m_BulkRowIndex == m_BulkFetchCount )
        {
            m_BulkRowIndex = 0;

            rc = ::SQLFetchScroll( m_hstmt,
                SQL_FETCH_NEXT,
                m_BulkFetchCount );

            if( !( SQL_SUCCEEDED( rc ) || rc == SQL_NO_DATA ) )
                HandleError();
        }

        if( SQL_SUCCEEDED( rc ) )
        {
            if( m_pBulkRowStatus == nullptr ||
                m_pBulkRowStatus[ m_BulkRowIndex ] == SQL_ROW_SUCCESS ||
                m_pBulkRowStatus[ m_BulkRowIndex ] == SQL_ROW_SUCCESS_WITH_INFO )
            {
                bRetval = true;

                if( m_pResultset )
                    m_pResultset->Fetch( m_pBulkRowStatus ? m_BulkRowIndex : 0 );

                m_BulkRowIndex++;

                if( bRetval )
                {
                    m_RowsFetched++;
                    m_SumRowsFetched++;
                }
            }
            else if( m_pBulkRowStatus[ m_BulkRowIndex ] == SQL_ROW_NOROW )
            {
                m_bEndOfData = true;
            }
            else if( m_pBulkRowStatus[ m_BulkRowIndex ] == SQL_ROW_ERROR )
            {
                HandleError();
            }
        }
        else if( rc == SQL_NO_DATA )
        {
            m_bEndOfData = true;
        }
    }

    return bRetval;
}

bool CODStatement::AllocateStatement()
{
    bool bRetval = false;

    if( m_hstmt != SQL_NULL_HANDLE )
    {
        //  A statement may have been allocated before it was prepared since it attributes might have been set.
        return true;
    }

    // Allocate the statement handle. The statement handle may have be allocated already if an option was set on the handle.
    SQLRETURN rc = ::SQLAllocHandle(
        SQL_HANDLE_STMT,
        m_hdbc,
        &m_hstmt
    );

    if( SQL_SUCCEEDED( rc ) )
    {
        bRetval = true;
    }
    else
    {
        HandleError();
    }

    return bRetval;
}

bool CODStatement::CreateResultset( SQLSMALLINT nColumn )
{
    bool bRetval = false;

    if( nColumn )
    {
        m_BulkFetchCount = 1;

        CODResultset* pResultset = new CODResultset();

        if( pResultset )
        {
            if( pResultset->Create( this, nColumn, 1 ) )
            {
                bRetval = true;
                m_pResultset = pResultset;
            }
            else
            {
                delete pResultset;
            }
        }
    }

    return bRetval;
}

void CODStatement::HandleError()
{
    throw std::runtime_error( "Encountered an error " );
}
