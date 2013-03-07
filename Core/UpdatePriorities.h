#ifndef CUBIQUITY_UPDATEPRIORITIES_H_
#define CUBIQUITY_UPDATEPRIORITIES_H_

namespace Cubiquity
{
	namespace UpdatePriorities
	{
		enum UpdatePriority
		{
			DontUpdate = 0,
			Background = 1,
			Immediate = 2
		};
	}
	typedef UpdatePriorities::UpdatePriority UpdatePriority;
}

#endif //CUBIQUITY_UPDATEPRIORITIES_H_
