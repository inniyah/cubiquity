#ifndef CUBIQUITYTOOLS_EXCEPTIONS_H_
#define CUBIQUITYTOOLS_EXCEPTIONS_H_

#include "HeaderOnlyLibs.h" // For logging

#include "CubiquityC.h"

#include <stdexcept>

// Thrown if there is a problem parsing command line options
class OptionsError : public std::runtime_error
{
public:
	OptionsError(const std::string& what_arg)
		:runtime_error(what_arg)
	{
	}
};

// Thrown if an error code is returned by Cubiquity.
class CubiquityError : public std::runtime_error
{
public:
	CubiquityError(int32_t errorCode, const std::string& errorMessage)
		:runtime_error(errorMessage)
		,mErrorCode(errorCode)
	{
	}

	int32_t getErrorCode(void)
	{
		return mErrorCode;
	}

	std::string getErrorCodeAsString(void)
	{
		return cuGetErrorCodeAsString(mErrorCode);
	}

	std::string getErrorMessage(void)
	{
		return what();
	}

private:
	int32_t mErrorCode;
};

// We use this utility function for throwing exceptions
// because it lets us log the message at the same time.
template<typename ExceptionType>
void throwException(ExceptionType e)
{
	LOG(ERROR) << e.what();
	throw e;
}

// This version is useful for testing error conditions with minimal
// lines of code, and throwing an exception if the condition fails.
template<typename ExceptionType>
void throwExceptionIf(bool condition, ExceptionType e)
{
	if (condition)
	{
		throwException(e);
	}
}

// I don't think it matters whether or not calls to this macro have
// a terminating ';' or not. Extra semicolons should be harmless.
#define VALIDATE_CALL(functionCall) \
{ \
	int32_t result = functionCall; \
	if (result != CU_OK) \
	{ \
		throwException(CubiquityError(result, cuGetLastErrorMessage())); \
	} \
}

#endif //CUBIQUITYTOOLS_EXCEPTIONS_H_