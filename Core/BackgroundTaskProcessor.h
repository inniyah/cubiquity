#ifndef CUBIQUITY_BACKGROUNDTASKPROCESSOR_H_
#define CUBIQUITY_BACKGROUNDTASKPROCESSOR_H_

#include "ConcurrentQueue.h"
#include "TaskProcessor.h"

#include "boost/thread.hpp"

#include <list>

class BackgroundTaskProcessor : public TaskProcessor
{
public:
	BackgroundTaskProcessor(uint32_t noOfThreads);
	virtual ~BackgroundTaskProcessor();

	void addTask(Task* task);

	bool hasAnyFinishedTasks(void);
	Task* removeFirstFinishedTask(void);

	void processTasks(void);

	concurrent_queue<Task*, TaskSortCriterion> mPendingTasks;

	std::list<boost::thread*> mThreads;
};

extern BackgroundTaskProcessor gBackgroundTaskProcessor;

#endif //CUBIQUITY_BACKGROUNDTASKPROCESSOR_H_
