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
	mPendingTasks.push(task);
}

bool BackgroundTaskProcessor::hasAnyFinishedTasks(void)
{
	return !mFinishedTasks.empty();
}

Task* BackgroundTaskProcessor::removeFirstFinishedTask(void)
{

	Task* task = 0;
	mFinishedTasks.wait_and_pop(task);
	return task;
}

void BackgroundTaskProcessor::processTasks(void)
{
	while(true)
	{
		Task* task = 0;
		mPendingTasks.wait_and_pop(task);
		task->process();
		mFinishedTasks.push(task);
	}
}