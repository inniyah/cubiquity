#ifndef CUBIQUITY_CLOCK_H
#define CUBIQUITY_CLOCK_H

#include <cstdint>

typedef uint32_t Timestamp;

class Clock
{
public:
	static Timestamp getTimestamp(void);

private:
	//This should use boost::atomic?
	static Timestamp mTimestamp;
};

#endif //CUBIQUITY_CLOCK_H
