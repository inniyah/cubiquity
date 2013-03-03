#ifndef CUBIQUITY_TASK_H_
#define CUBIQUITY_TASK_H_

#include "PolyVoxCore/Impl/ErrorHandling.h"

class Task
{
public:
	Task();

	virtual void process(void) = 0;
};

#endif //CUBIQUITY_TASK_H_