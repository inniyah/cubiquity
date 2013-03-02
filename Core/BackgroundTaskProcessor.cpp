#include "BackgroundTaskProcessor.h"

#include "boost/bind.hpp"

BackgroundTaskProcessor gBackgroundTaskProcessor; //Our global instance

BackgroundTaskProcessor::BackgroundTaskProcessor()
	:TaskProcessor()
{
	mThread = new boost::thread(boost::bind(&BackgroundTaskProcessor::processTasks, this));
}

BackgroundTaskProcessor::~BackgroundTaskProcessor()
{
	//mThread->join(); //This will never happen!
	delete mThread;
}

void BackgroundTaskProcessor::addTask(Task* task)
{
	boost::unique_lock<boost::mutex> lock(mPendingTasksMutex);

	mPendingTasks.push_back(task);

	mHasPendingTasks.notify_one();
}

bool BackgroundTaskProcessor::hasAnyFinishedTasks(void)
{
	boost::unique_lock<boost::mutex> lock(mFinishedTasksMutex);

	return mFinishedTasks.size() > 0;
}

Task* BackgroundTaskProcessor::removeFirstFinishedTask(void)
{
	boost::unique_lock<boost::mutex> lock(mFinishedTasksMutex);

	Task* task = mFinishedTasks.front();
	mFinishedTasks.pop_front();
	return task;
}

void BackgroundTaskProcessor::processTasks(void)
{
	while(true)
	{
		boost::unique_lock<boost::mutex> pendingTasksLock(mPendingTasksMutex);

		// Process data
        while( mPendingTasks.size() == 0 ) // while - to guard agains spurious wakeups
        {
            mHasPendingTasks.wait( pendingTasksLock );
        }

		Task* task = mPendingTasks.front();
		mPendingTasks.pop_front();

		POLYVOX_ASSERT(task->getState() == TaskStates::Pending, "Task must have pending state to be processed");
		task->gotoNextState();
		task->process();
		task->gotoNextState();

		// Save the completed task
		{
			boost::unique_lock<boost::mutex> lock(mFinishedTasksMutex);
			mFinishedTasks.push_back(task);
		}
	}
}