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
	mMutex.lock();
	mPendingTasks.push_back(task);
	mMutex.unlock();
}

bool BackgroundTaskProcessor::hasAnyFinishedTasks(void)
{
	mMutex.lock();
	bool result = false;
	result = mFinishedTasks.size() > 0;
	mMutex.unlock();

	return result;
}

Task* BackgroundTaskProcessor::removeFirstFinishedTask(void)
{
	mMutex.lock();
	Task* task = mFinishedTasks.front();
	mFinishedTasks.pop_front();
	mMutex.unlock();

	return task;
}

void BackgroundTaskProcessor::processTasks(void)
{
	while(true)
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(1));
		if(mPendingTasks.size() > 0)
		{
			mMutex.lock();
			Task* task = mPendingTasks.front();
			mPendingTasks.pop_front();
			mMutex.unlock();

			POLYVOX_ASSERT(task->getState() == TaskStates::Pending, "Task must have pending state to be processed");
			task->gotoNextState();
			task->process();
			task->gotoNextState();

			mMutex.lock();
			mFinishedTasks.push_back(task);
			mMutex.unlock();
		}
	}
}