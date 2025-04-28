#pragma once

#include <atlstr.h>

class CIError
{
public:
	static const int ERROR_UNSUPPORTED_VERSION = 948;
	static const int SQL_ERROR_LOG_FILE_NOT_MATCH = 5108;
	static const int SQL_ERROR_LOG_FILE_NOT_MATCH_OR_CORRUPTION = 9003;

	CIError( int /*nError*/, const CString& /*szError*/, const CString& /*szState*/, int /*nErrorCode*/ ) {};
	CIError( int /*nErrorCode*/ ) {};
	virtual ~CIError() {};
	virtual int GetError() const = 0;
	virtual CString GetErrorMessage() const = 0;
	virtual CString GetErrorState() const = 0;
	virtual int GetErrorCode() const = 0;
};
