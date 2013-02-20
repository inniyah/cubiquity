#ifndef CUBIQUITY_TASKPROCESSOR_H_
#define CUBIQUITY_TASKPROCESSOR_H_

#include <list>

template <typename TaskType>
class TaskProcessor
{
public:
	TaskProcessor();
	virtual ~TaskProcessor();

	void addTask(TaskType task);

	bool hasAnyFinishedTasks(void);
	TaskType removeFirstFinishedTask(void);

	virtual void processOneTask(void)/* = 0*/;

	std::list<TaskType> mPendingTasks;
	std::list<TaskType> mFinishedTasks;
};

#include "TaskProcessor.inl"

#endif //CUBIQUITY_TASKPROCESSOR_H_
