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

	//std::list<Task*> mPendingTasks;
	//std::list<Task*> mFinishedTasks;
	concurrent_queue<Task*> mPendingTasks;
	concurrent_queue<Task*> mFinishedTasks;

	std::list<boost::thread*> mThreads;
	//boost::mutex mPendingTasksMutex;
	//boost::mutex mFinishedTasksMutex;
	//boost::condition_variable mHasPendingTasks;
};

extern BackgroundTaskProcessor gBackgroundTaskProcessor;

#endif //CUBIQUITY_BACKGROUNDTASKPROCESSOR_H_
