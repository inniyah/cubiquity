#include "Clock.h"

#include "PolyVoxCore\Impl\ErrorHandling.h"

namespace Cubiquity
{
	Timestamp Clock::mTimestamp = 0;

	Timestamp Clock::getTimestamp(void)
	{
		// I don't think we need to protect this operation with a mutex. Potentially two threads could enter this function and then
		// leave in a different order to which they entered, but I don't think that matters as long as the timestamps are unique?
		POLYVOX_ASSERT(mTimestamp < std::numeric_limits<Timestamp>::max(), "Time stamp is wrapping around.");
		return ++mTimestamp;
	}
}
