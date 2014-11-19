#ifndef CUBIQUITYTOOLS_EXCEPTIONS_H_
#define CUBIQUITYTOOLS_EXCEPTIONS_H_

#include "HeaderOnlyLibs.h" // For logging

#include <stdexcept>

// Used if 
class OptionsError : public std::runtime_error
{
public:
	OptionsError(const std::string& what_arg)
		:runtime_error(what_arg)
	{
	}
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

#endif //CUBIQUITYTOOLS_EXCEPTIONS_H_