#pragma once

#include <atlstr.h>
#include <sqltypes.h>

class CIColumn;
class CIParam;
class CIParamset;
class CIResultset;
class CIError;

class CIStatement
{
public:
	enum scrollableCursor
	{
		defaultScrollable = ( int ) 0,
		scrollableOn = ( int ) 1,
		scrollableOff = ( int ) 2,
	};

	enum concurrencyTypes
	{
		defaultConcurrency = ( int ) 0,
		concurrencyReadOnly = ( int ) 1,    //  Cursor is read-only. No updates are allowed.
		concurrencyLock = ( int ) 2,    //  Cursor uses the lowest level of locking sufficient to ensure that the row can be updated.
		concurrencyRowVersion = ( int ) 3,    //  Cursor uses optimistic concurrency control, comparing row versions such as SQLBase ROWID or Sybase TIMESTAMP (or a Primary Key?)
		concurrencyValues = ( int ) 4,    //  Cursor uses optimistic concurrency control, comparing values.
	};

	enum cursorTypes
	{
		defaultType = ( int ) 0,
		typeForwardOnly = ( int ) 1,    //  The cursor only scrolls forward.
		typeStatic = ( int ) 2,    //  The data in the result set is static
		typeKeysetDriven = ( int ) 3,    //  The driver saves and uses the keys for the number of rows specified by the keyset size attribute.
		typeDynamic = ( int ) 4,    //  The driver saves and uses only the keys for the rows in the rowset.
	};

	enum cursorSensitivity
	{
		defaultSensitivity = ( int ) 0,
		sensitivityOn = ( int ) 1,    //  Keyset-driven cursor and version-based optimistic concurrency
		sensitivityOff = ( int ) 2,    //
	};

	/*
	**  Define the metadata/catalog enums and functions
	**
	**  The enum prior to each Describe function denotes
	**  columns ordinal (position).  The enum only define a
	**  portion of the available data.
	**
	**  For example:
	**      A call to DescribeTable will have the table name
	**      in ordinal 2.
	*/

	// 	For more information about the values of these enums, see the ODBC
	//	reference for SQLProcedures, SQLProcedureColumns, SQLTables, SQLColumns,
	//	SQLStatictics and SQLPrimaryKeys
	//  Note that this library use 0-based column ordinal, while ODBC uses 1-based,
	//  so the enum values here a one less than that specified in the ODBC documentation.
	enum describeProcedures
	{
		procedureSchema = ( int ) 1,	//  schema name
		procedureName = ( int ) 2,	//  procedure name
		// Parameters 3-5 are reserved by the ODBC for future use and shouldn't be used.
		procedureRemarks = ( int ) 6,	//  description of the procedure
		procedureType = ( int ) 7,	// One Of: SQL_PT_UNKNOWN,
		// SQL_PT_PROCEDURE (no return value) or
		// SQL_PT_FUNCTION (has return value)
	};

	//  Procedure column enums prefixed with "pc" to save confusion interpreting them later.
	//	For example, ProcedureColumnProcedureName is a headache to figure out.
	enum describeProcedureColumns
	{
		pcSchema = ( int ) 1,	//  Schema name
		pcProcedureName = ( int ) 2,	//  Procedure name
		pcName = ( int ) 3,	//	Procedure Column Name
		pcColumnType = ( int ) 4,	// One of: SQL_PARAM_TYPE_UNKNOWN, SQL_PARAM_INPUT,
		// SQL_PARAM_INPUT_OUTPUT, SQL_PARAM_OUTPUT
		// SQL_RETURN_VALUE: the return value of the procedure
		// SQL_RESULT_COL: a result set column.
		pcDataType = ( int ) 5,	// SQL data type; small int
		pcDataTypeName = ( int ) 6,	// Data type; varchar
		pcSize = ( int ) 7,	// Procedure column size; int, null if not applicable.
		pcScale = ( int ) 9,    // Procedure column decimal digits i.e. scale
		pcNullable = ( int ) 11,	// One of: SQL_NO_NULLS (doesn't accept nullptr values)
		//	SQL_NULLABLE (accepts nullptr values),
		//	or SQL_NULLABLE_UNKNOWN.
		pcRemarks = ( int ) 12,	// description of the procedure column
	};

	enum describeTables
	{
		schemaName = ( int ) 1,         //  schema name
		tableName = ( int ) 2,         //  table name
		tableType = ( int ) 3,		 //  table type
		tableRemarks = ( int ) 4,		 //  remarks
	};

	// These enums are based on SQLColumns.  Search for SQLColumns in msdn
	// Note that there enums are off by 1
	enum describeColumns		// resultset enum for SQLColumns
	{
		columnSchemaName = ( int ) 1,        //  table schema
		columnTableName = ( int ) 2,        //  table to which the column belongs
		columnName = ( int ) 3,        //  column name
		columnType = ( int ) 4,        //  column type
		columnTypeName = ( int ) 5,        //  column type name (ie 'text', 'int')
		columnSize = ( int ) 6,        //  column size
		columnNullable = ( int ) 10,       //  column null capability
		columnScale = ( int ) 8,        //  column decimal digits i.e. scale
		columnDescription = ( int ) 11		//  column description
	};

	enum describeIndexes 		// resultset enum for SQLStatistics
	{
		indexTableName = ( int ) 2,        //  table to which the index belongs
		indexUniqueness = ( int ) 3,        //  index uniqueness
		indexName = ( int ) 5,        //  index name
		indexType = ( int ) 6,        //  the type of index
		indexColumnOrdinal = ( int ) 7,        //  the columns ordinal in the index
		indexColumnName = ( int ) 8         //  column name in the index
	};

	enum describePrimaryKeys
	{
		primaryTable = ( int ) 2,        //  table name
		primaryColumn = ( int ) 3,        //  column name
		primaryName = ( int ) 5         //  primary key name
	};

	enum describeForeignKeys
	{
		referenceSchema = ( int ) 1,        //  schema of table foreign key refers to
		referenceTable = ( int ) 2,        //  table foreign key refers to
		referenceColumn = ( int ) 3,        //  column foreign key refers to
		foreignSchema = ( int ) 5,        //  schema of the foreign key
		foreignTable = ( int ) 6,        //  table containing foreign key
		foreignColumn = ( int ) 7,        //  column that is part of the foreign key
		foreignUpdateRule = ( int ) 9,        //  foreign key update rule
		foreignDeleteRule = ( int ) 10,       //  foreign key delete rule
		foreignKeyName = ( int ) 11,       //  foreign key name
		referencePrimaryKey = ( int ) 12,       //  primary key the foreign key references
	};

	enum privilegeTable
	{
		privilegeSchema = ( int ) 2,        //  the schema name
		privilegeTable = ( int ) 3,        //  the table name
		privilegeGranted = ( int ) 5,        //  the privilege,  one of SELECT, INSERT, UPDATE, DELETE
	};

	CIStatement() {};
	virtual ~CIStatement() {};

	virtual bool Fetch() = 0;
	virtual bool Release() = 0;
	virtual void HandleError() = 0;

	virtual bool DescribeTables( const char* database, const char* schema ) = 0;

	virtual SQLHANDLE GetHandle() const = 0;
};
