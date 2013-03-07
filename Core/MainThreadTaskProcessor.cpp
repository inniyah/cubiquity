#include "MainThreadTaskProcessor.h"

namespace Cubiquity
{
	MainThreadTaskProcessor gMainThreadTaskProcessor; //Our global instance

	MainThreadTaskProcessor::MainThreadTaskProcessor()
		:TaskProcessor()
	{
	}

	MainThreadTaskProcessor::~MainThreadTaskProcessor()
	{
	}

	void MainThreadTaskProcessor::addTask(Task* task)
	{
		mPendingTasks.push_back(task);
	}

	void MainThreadTaskProcessor::processOneTask(void)
	{
		if(mPendingTasks.size() > 0)
		{
			Task* task = mPendingTasks.front();
			mPendingTasks.pop_front();
			task->process();
		}
	}

	void MainThreadTaskProcessor::processAllTasks(void)
	{
		while(mPendingTasks.size() > 0)
		{
			Task* task = mPendingTasks.front();
			mPendingTasks.pop_front();
			task->process();
		}
	}
}