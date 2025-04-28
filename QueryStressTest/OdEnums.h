#pragma once

/*
**	SQL Server data type used in the Harmony database
**
**	SQL Server Data Types	ODBC SQL Data Type		C++ Type
**	=====================	==================		=================
**	UNIQUEIDENTIFIER		SQL_GUID				CGuid
**	NCHAR					SQL_WCHAR				CString
**	INT						SQL_INTEGER				int
**	NVARCHAR				SQL_WVARCHAR			CString
**	DATETIME				SQL_TYPE_TIMESTAMP		double
**	FLOAT					SQL_FLOAT				double
**	VARBINARY(MAX)			SQL_VARBINARY(MAX)		CBlob
**	NVARCHAR(MAX)			SQL_VARCHAR(MAX)		CString (clob)
**
*/

/*
**	Oracle data types used in the Harmony database
**	=====================	==================		=================
**	RAW(16)					SQL_VARBINARY			CGuid
**	NCHAR					SQL_VARCHAR				CString
**	NUMBER(10)				SQL_NUMERIC				int
**	NVARCHAR2				SQL_WVARCHAR			CString
**	TIMESTAMP(6)			SQL_TYPE_TIMESTAMP		double
**	FLOAT(126)				SQL_FLOAT				double
**	BLOB					SQL_LONGVARBINARY		CBlob
**	CLOB					SQL_LONGVARCHAR			CString
**
*/

/*
**	ODBC SQL Data Type		EnumSqlType				EnumNativeType								ODBC Bind Type							C++ Type
**	===========				===========				==============								==============							==============
**	SQL_CHAR				eSqlChar				eNativeChar									SQL_C_TCHAR								CString
**	SQL_VARCHAR				eSqlVarchar				eNativeChar or eNativeClob (if max)			SQL_C_TCHAR								CString
**	SQL_WCHAR				eSqlWideChar			eNativeChar									SQL_C_TCHAR								CString
**	SQL_WVARCHAR			eSqlWideVarchar			eNativeChar or eNativeClob (if max)			SQL_C_TCHAR								CString
**	SQL_TINYINT				eSqlTinyINt				eNativeTinyInt								SQL_C_TINYINT							char
**	SQL_SMALLINT			eSqlShort				eNativeShort								SQL_C_SHORT								short
**	SQL_INTEGER				eSqlInt					eNativeInt									SQL_C_LONG								int
**	SQL_BIGINT				eSqlBigINt				eNativeBigInt								SQL_C_SBIGINT							__bigint
**	SQL_REAL				eSqlReal				eNativeFloat								SQL_C_FLOAT								float
**	SQL_FLOAT is mapped directly to SQL_DOUBLE
**	SQL_DOUBLE				eSqlDouble				eNativeDouble								SQL_C_DOUBLE							double
**	SQL_DECIMAL	is mapped to SQL_NUMERIC
**	SQL_NUMERIC				eSqlNumeric				eNativeInt or eNativeDouble (scale > 0)		SQL_C_LONG or SQL_C_DOUBLE				double
**	SQL_DATE                eSqlOldDate				eNativeDate or eNativeDateTime (w/time)		SQL_C_TYPE_DATE or SQL_C_TYPE_TIMESTAMP	double
**	SQL_TIMESTAMP			eSqlOldDateTime			eNativeDateTime								SQL_C_TYPE_TIMESTAMP					double
**	SQL_TYPE_DATE			eSqlDate				eNativeDate or eNativeDateTime (w/time)		SQL_C_TYPE_DATE or SQL_C_TYPE_TIMESTAMP	double
**	SQL_TYPE_TIMESTAMP		eSqlDateTime			eNativeDateTime								SQL_C_TYPE_TIMESTAMP					double
**	SQL_BINARY				eSqlBinary				eNativeBinary								SQL_C_BINARY							BYTE
**	SQL_VARBINARY			eSqlVarbinary			eNativeBinary or eNativeBlob (if max)		SQL_C_BINARY							BYTE or CBlob
**	SQL_LONGVARBINARY		eSqlBlob				eNativeBlob									SQL_C_BINARY							CBlob
**	SQL_LONGVARCHAR			eSqlClob				eNativeClob									SQL_C_TCHAR								CString
**	SQL_WLONGVARCHAR		eSqlWideClob			eNativeClob									SQL_C_TCHAR								CString
**	SQL_GUID				eSqlGuid				eNativeGuid									SQL_C_TCHAR								CGuid
**
**
**	SQL_CHAR(16) and SQL_BINARY(16) can be treated as a CGuid.  This was done since Oracle has no guid datatype.
*/

/*
**	This file declares enum that can be used instead of the SQL ODBC defines.
**
**	The EnumSqlType maps each (supported) ODBC data type to an enum.
**	For example SQL_CHAR becomes eSqlChar, SQL_WCHAR becomes eSqlWideChar.
**	These types correspond to the data type in the databases have the prefix 'Sql'
**
**	The EnumNativeType maps ODBC C data type defines to an enum.
**	For example SQL_C_CHAR and SQL_C_WCHAR becomes eNativeChar
**	These type correspond to the C data types in the application hence the prefix 'Native'
**
**	The CoalAPI will map the EnumSqlType to an EnumNativeType.  Since the mapping of
**	Sql to Native types is many to one you cannot map in the reverse direction.
**
**	eSqlChar
**	eSqlVarchar(n)
**	eSqlWideChar =
**	eSqlWideVarchar(n)* map to eNativeChar.
**
**	eSqlGuid maps to eNativeGuid
**
**	eSqlTinyInt maps to eNativeTinyInt
**	eSqlShort maps to eNativeShort
**	eSqlInt maps to eNativeInt
**	eSqlBigInt maps to eNativeBigInt
**
**	eSqlReal maps to eNativeFloat
**	eSqlDouble maps to eNativeDouble
**
**	eSqlNumeric maps to either eNativeInt or eNativeDouble depending the scale.
**
**	eSqlOldDate
**	eSqlOldDateTime
**	eSqlDate
**	eSqlDateTime will map to eNativeDate or eNativeDateTime (depending on the size)
**
**	eSqlOldDate
**	eSqlOldDateTime
**	eSqlDate
**	eSqlDateTime will map to either eNativeDate or eNativeDateTime
**
**	eSqlBinary
**	eSqlVarbinary map to eNativeBinary
**
**	eSqlBlob
**	eSqlVarbinary(MAX) map to eNativeBlob
**
**	eSqlClob
**	eSqlWideClob
**	eSqlVarchar(max)
**	eSqlWideVarchar(max) map to eNativeClob
*/

/*
**	These are the datatypes that are mapped to individual ODBC SQL datatypes.
*/
enum EnumSqlType
{
	eSqlUnknown = 0,
	eSqlChar,			// non unicode CHAR 
	eSqlVarchar,		// non unicode VARCHAR
	eSqlWideChar,		// unicode CHAR
	eSqlWideVarchar,	// unicode VARCHAR
	eSqlGuid,			// GUID, UUID or uniqueidentifier
	eSqlBit,			// SQL Server bit (bool)
	eSqlTinyInt,		// 8 bit integer
	eSqlShort,			// 16 bit integer
	eSqlInt,			// 32 bit integer
	eSqlBigInt,			// 64 bit integer
	eSqlReal,			// REAL (4 bytes)
	eSqlDouble,			// DOUBLE or FLOAT (8 bytes)
	eSqlNumeric,		// NUMERIC or DECIMAL
	eSqlOldDate,		// OBDC date type prior to ODBC 3
	eSqlOldDateTime,	// ODBC date type prior to ODBC 3
	eSqlDate,			// ODBC 3 type SQL_TYPE_DATE
	eSqlDateTime,		// ODBC 3 type SQL_TYPE_TIMESTAMP
	eSqlBinary,			// BINARY (fixed length)
	eSqlVarbinary,		// VARBINARY (variable length)
	eSqlBlob,			// BLOB, IMAGE, VARBINARY(MAX)
	eSqlClob,			// CLOB, TEST, VARCHAR(MAX)
	eSqlWideClob,		// NCLOB, NTEXT or NVARCHAR(max)
};

/*
**	These are the datatypes that the columns are translated into
*/
enum EnumNativeType
{
	eNativeUnknown = 0,
	eNativeChar,
	eNativeDate,
	eNativeDateTime,
	eNativeTinyInt,
	eNativeShort,
	eNativeInt,
	eNativeBigInt,
	eNativeFloat,
	eNativeDouble,
	eNativeNumeric,
	eNativeGuid,
	eNativeBinary,
	eNativeBlob,
	eNativeClob,
};

// These are the possible results when attempting to connect to a database
enum DbConnectionResult
{
	eConnectionSucceeded = 0,
	eConnectionFailed = 1,
	eConnectionCancelled = 2,
	eConnectionInvalidServerVersion = 3,
	eConnectionExceedsSELimits = 4
};

// See definition of "state" column here: https://docs.microsoft.com/en-us/sql/relational-databases/system-catalog-views/sys-databases-transact-sql?view=sql-server-2017
enum SqlServerSysDatabasesState
{
	eOnline = 0,
	eRestoring = 1,
	eRecovering = 2,
	eRecoveryPending = 3,
	eSuspect = 4,
	eEmergency = 5,
	eOffline = 6,
	eCopying = 7,
	eOfflineSecondary = 10
};