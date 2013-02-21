#ifndef CUBIQUITY_TASKPROCESSOR_H_
#define CUBIQUITY_TASKPROCESSOR_H_

template <typename TaskType>
class TaskProcessor
{
public:
	TaskProcessor();
	virtual ~TaskProcessor();

	virtual void addTask(TaskType task) = 0;

	virtual bool hasAnyFinishedTasks(void) = 0;
	virtual TaskType removeFirstFinishedTask(void) = 0;
};

#include "TaskProcessor.inl"

#endif //CUBIQUITY_TASKPROCESSOR_H_
