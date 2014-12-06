#include "BackgroundTaskProcessor.h"

/*#include "boost/bind.hpp"

namespace Cubiquity
{
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

	void BackgroundTaskProcessor::processTasks(void)
	{
		// This is commented out becaue we're not currently using this class, and having this running
		// causes problems on application shutdown. We probably need a way to stop this background processor.
		//while(true)
		//{
		//	Task* task = 0;
		//	mPendingTasks.wait_and_pop(task);
		//	task->process();
		//}
	}
}*/

namespace Cubiquity
{
	BackgroundTaskProcessor gBackgroundTaskProcessor; //Our global instance

	BackgroundTaskProcessor::BackgroundTaskProcessor()
		:TaskProcessor()
	{
	}

	BackgroundTaskProcessor::~BackgroundTaskProcessor()
	{
		mPendingTasks.clear();
	}

	void BackgroundTaskProcessor::addTask(Task* task)
	{
		mPendingTasks.push_back(task);
	}

	bool BackgroundTaskProcessor::hasTasks(void)
	{
		return mPendingTasks.size() > 0;
	}

	void BackgroundTaskProcessor::processOneTask(void)
	{
		if (mPendingTasks.size() > 0)
		{
			Task* task = mPendingTasks.front();
			mPendingTasks.pop_front();
			task->process();
		}
	}

	void BackgroundTaskProcessor::processAllTasks(void)
	{
		while (mPendingTasks.size() > 0)
		{
			Task* task = mPendingTasks.front();
			mPendingTasks.pop_front();
			task->process();
		}
	}
}
