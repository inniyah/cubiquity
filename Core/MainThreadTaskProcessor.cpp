#include "MainThreadTaskProcessor.h"

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

bool MainThreadTaskProcessor::hasAnyFinishedTasks(void)
{
	return mFinishedTasks.size() > 0;
}

Task* MainThreadTaskProcessor::removeFirstFinishedTask(void)
{
	Task* task = mFinishedTasks.front();
	mFinishedTasks.pop_front();
	return task;
}

void MainThreadTaskProcessor::processOneTask(void)
{
	if(mPendingTasks.size() > 0)
	{
		Task* task = mPendingTasks.front();
		mPendingTasks.pop_front();

		POLYVOX_ASSERT(task->getState() == TaskStates::Pending, "Task must have pending state to be processed");
		task->gotoNextState();
		task->process();
		task->gotoNextState();

		mFinishedTasks.push_back(task);
	}
}

void MainThreadTaskProcessor::processAllTasks(void)
{
	while(mPendingTasks.size() > 0)
	{
		Task* task = mPendingTasks.front();
		mPendingTasks.pop_front();

		POLYVOX_ASSERT(task->getState() == TaskStates::Pending, "Task must have pending state to be processed");
		task->gotoNextState();
		task->process();
		task->gotoNextState();

		mFinishedTasks.push_back(task);
	}
}