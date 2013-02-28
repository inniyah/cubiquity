#ifndef CUBIQUITY_MAINTHREADTASKPROCESSOR_H_
#define CUBIQUITY_MAINTHREADTASKPROCESSOR_H_

#include "TaskProcessor.h"

#include <list>

class MainThreadTaskProcessor : public TaskProcessor
{
public:
	MainThreadTaskProcessor();
	virtual ~MainThreadTaskProcessor();

	void addTask(Task* task);

	bool hasAnyFinishedTasks(void);
	Task* removeFirstFinishedTask(void);

	virtual void processOneTask(void)/* = 0*/;
	virtual void processAllTasks(void)/* = 0*/;

	std::list<Task*> mPendingTasks;
	std::list<Task*> mFinishedTasks;
};

extern MainThreadTaskProcessor gMainThreadTaskProcessor;

//#include "MainThreadTaskProcessor.inl"

#endif //CUBIQUITY_MAINTHREADTASKPROCESSOR_H_
