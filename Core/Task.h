#ifndef CUBIQUITY_TASK_H_
#define CUBIQUITY_TASK_H_

#include "PolyVoxCore/Impl/ErrorHandling.h"

#include <cstdint>

class Task
{
public:
	Task();

	virtual void process(void) = 0;

	uint32_t mPriority;
};

class TaskSortCriterion
{
public:
	bool operator()(const Task* task1, const Task* task2) const
	{
		return task1->mPriority < task2->mPriority;
	}
};

#endif //CUBIQUITY_TASK_H_