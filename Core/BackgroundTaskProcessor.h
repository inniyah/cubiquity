#ifndef CUBIQUITY_BACKGROUNDTASKPROCESSOR_H_
#define CUBIQUITY_BACKGROUNDTASKPROCESSOR_H_

#include "TaskProcessor.h"

#include "boost/thread.hpp"

#include <list>

class BackgroundTaskProcessor : public TaskProcessor
{
public:
	BackgroundTaskProcessor();
	virtual ~BackgroundTaskProcessor();

	void addTask(Task* task);

	bool hasAnyFinishedTasks(void);
	Task* removeFirstFinishedTask(void);

	void processTasks(void);

	std::list<Task*> mPendingTasks;
	std::list<Task*> mFinishedTasks;

	boost::thread* mThread;
	boost::mutex mPendingTasksMutex;
	boost::mutex mFinishedTasksMutex;
	boost::condition_variable mHasPendingTasks;
};

extern BackgroundTaskProcessor gBackgroundTaskProcessor;

#endif //CUBIQUITY_BACKGROUNDTASKPROCESSOR_H_
