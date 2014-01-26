#include "Task.h"

// VLD is commented out by default because it interferes with Unity. More specifically, if VLD is enabled then
// Unity crashes when using a debug version of Cuiquity (VLD is only enabled in debug mode). The crash does not
// appear to be located in a particular Cubiquity function, and I think it is instead a result of Unity calling
// the garbage collector and this somehow interferring with whatever VLD does.
//#include "vld.h"

namespace Cubiquity
{
	Task::Task()
		:mPriority(0)
	{
	}
}
