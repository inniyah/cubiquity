#include "SQLitePager.h"

#include "PolyVoxCore/Region.h"

namespace Cubiquity
{
	// This function encodes a Region as a 64-bit integer so that it can be used as a key to access block data in the SQLite database.
	// A region actually contains more than 64-bits of data so some has to be lost here. Specifically we assume that we already know
	// the size of the region (so we only have to encode it's lower corner and not its upper corner or extents), and we also restrict
	// the range of valid coordinates. A Region's coordinates are represented by 3-buts of data, but we only support converting to a key 
	// if every coordinate can be represented by 21 bits of data. This way we can fit three coordinates only 63 bits of data. This limits
	// the range of values to +/- 2^20, which is enough for our purposes.
	uint64_t regionToKey(const PolyVox::Region& region)
	{
		// Cast to unsigned values so that bit shifting works predictably.
		uint32_t x = static_cast<uint32_t>(region.getLowerX());
		uint32_t y = static_cast<uint32_t>(region.getLowerY());
		uint32_t z = static_cast<uint32_t>(region.getLowerZ());

		// The magnitude of our input values is fairly restricted, but the values could stil be negative. This means the sign bit could
		// be set and this needs to be encoded as well. We therefore perform a left rotate on the bits to bring the sign bit into the LSB.
		x = rotateLeft(x);
		y = rotateLeft(y);
		z = rotateLeft(z);

		// Now convert to 64-bits
		uint64_t x64 = x;
		uint64_t y64 = y;
		uint64_t z64 = z;

		// Perform shifts so that x, y, and z are in differnt parts of the integer.
		x64 = x64 << 42;
		y64 = y64 << 21;

		// Return the combined value
		return x64 ^ y64 ^ z64;
	}
}
