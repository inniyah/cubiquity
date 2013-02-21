#ifndef CUBIQUITY_MAINTHREADTASKPROCESSOR_H_
#define CUBIQUITY_MAINTHREADTASKPROCESSOR_H_

#include "TaskProcessor.h"

#include <list>

template <typename TaskType>
class MainThreadTaskProcessor : public TaskProcessor<TaskType>
{
public:
	MainThreadTaskProcessor();
	virtual ~MainThreadTaskProcessor();

	void addTask(TaskType task);

	bool hasAnyFinishedTasks(void);
	TaskType removeFirstFinishedTask(void);

	virtual void processOneTask(void)/* = 0*/;

	std::list<TaskType> mPendingTasks;
	std::list<TaskType> mFinishedTasks;
};

#include "MainThreadTaskProcessor.inl"

#endif //CUBIQUITY_MAINTHREADTASKPROCESSOR_H_
