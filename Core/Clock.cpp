#include "Clock.h"

#include "PolyVoxCore\Impl\ErrorHandling.h"

Timestamp Clock::mTimestamp = 0;

Timestamp Clock::getTimestamp(void)
{
	POLYVOX_ASSERT(mTimestamp < std::numeric_limits<Timestamp>::max(), "Time stamp is wrapping around.");
	return ++mTimestamp;
}
