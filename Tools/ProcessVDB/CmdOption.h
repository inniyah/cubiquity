#ifndef CUBIQUITYTOOLS_CMDOPTION_H_
#define CUBIQUITYTOOLS_CMDOPTION_H_

#include <string>

// Simple command-line parsing from here: http://stackoverflow.com/a/868894
char* getCmdOption(const char ** begin, const char ** end, const std::string & option);
bool cmdOptionExists(const char** begin, const char** end, const std::string& option);

#endif // CUBIQUITYTOOLS_CMDOPTION_H_
