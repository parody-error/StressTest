#pragma once

#include "IStatement.h"
#include <sqltypes.h>

class CODResultset;

class CODStatement : public CIStatement
{
public:

    CODStatement( SQLHANDLE hDbc );
    virtual ~CODStatement();

    bool DescribeTables( const char* database, const char* schema ) override;
    bool Fetch() override;
    bool Release() override;
    void HandleError() override;
    
    SQLHANDLE GetHandle() const override { return m_hstmt; }

    CODResultset* GetResultset() const { return m_pResultset; }

private:

    bool AllocateStatement();
    bool CreateResultset( SQLSMALLINT nColumn );

    SQLINTEGER m_BulkFetchCount;    //  number of row to fetch in bulk
    SQLINTEGER m_BulkRowIndex;      //  current bulk row offset
    SQLINTEGER m_RowsFetched;       //  number of rows fetched
    SQLINTEGER m_RowsAffected;      //  number of rows affected
    SQLINTEGER m_SumRowsFetched;    //  accumulated number row affected

    SQLHANDLE m_hdbc;  //  database connection handle
    SQLHANDLE m_hstmt;  // sql handle on which to execute

    SQLUSMALLINT* m_pBulkRowStatus; //  pointer to array of bulk row indicators

    CODResultset* m_pResultset;    //  pointer to the result set

    bool m_bEndOfData;
};
