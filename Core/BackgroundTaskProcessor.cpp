#include "BackgroundTaskProcessor.h"

#include "boost/bind.hpp"

BackgroundTaskProcessor gBackgroundTaskProcessor(1); //Our global instance

BackgroundTaskProcessor::BackgroundTaskProcessor(uint32_t noOfThreads)
	:TaskProcessor()
{
	for(uint32_t ct = 0; ct < noOfThreads; ct++)
	{
		boost::thread* taskProcessingThread = new boost::thread(boost::bind(&BackgroundTaskProcessor::processTasks, this));
		mThreads.push_back(taskProcessingThread);
	}
}

BackgroundTaskProcessor::~BackgroundTaskProcessor()
{
	for(std::list<boost::thread*>::iterator threadIter = mThreads.begin(); threadIter != mThreads.end(); threadIter++)
	{
		(*threadIter)->interrupt();
		(*threadIter)->join();
		delete *threadIter;
	}
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
        while( mPendingTasks.size() == 0 ) // while - to guard agains spurious wakeups (see http://stackoverflow.com/a/2379903)
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