#ifndef __CUBIQUITY_EXCEPTIONS_H__
#define __CUBIQUITY_EXCEPTIONS_H__

namespace Cubiquity
{
	// Exceptions listed in the order we added them, as this halps match the order
	// to the defines in the C interface (where they are also ordered numerically).

	class DatabaseError : public std::runtime_error
	{
	public:
		DatabaseError(const std::string& what_arg)
			:runtime_error(what_arg)
		{
		}
	};

	class CompressionError : public std::runtime_error
	{
	public:
		CompressionError(const std::string& what_arg)
			:runtime_error(what_arg)
		{
		}
	};
}

#endif //__CUBIQUITY_EXCEPTIONS_H__
