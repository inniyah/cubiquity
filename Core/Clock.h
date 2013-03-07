#ifndef CUBIQUITY_CLOCK_H
#define CUBIQUITY_CLOCK_H

#include <cstdint>

namespace Cubiquity
{
	typedef uint32_t Timestamp;

	class Clock
	{
	public:
		static Timestamp getTimestamp(void);

	private:
		// This should be atomic but I don't have a recent enough version of boost.
		// This will be addressed in the future with a new boost version or C++11.
		static Timestamp mTimestamp;
	};
}

#endif //CUBIQUITY_CLOCK_H
