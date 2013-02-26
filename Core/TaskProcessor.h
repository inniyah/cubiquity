#ifndef CUBIQUITY_TASKPROCESSOR_H_
#define CUBIQUITY_TASKPROCESSOR_H_

#include "Task.h"

class TaskProcessor
{
public:
	TaskProcessor() {}
	virtual ~TaskProcessor() {}

	virtual void addTask(Task* task) = 0;

	virtual bool hasAnyFinishedTasks(void) = 0;
	virtual Task* removeFirstFinishedTask(void) = 0;
};

//#include "TaskProcessor.inl"

#endif //CUBIQUITY_TASKPROCESSOR_H_
