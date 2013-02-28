#include "Task.h"

TaskState& operator++(TaskState& taskState)
{
	int tmp = static_cast<int>(taskState);
	++tmp;
	taskState = static_cast<TaskState>(tmp);
	return taskState;
}

TaskState operator++(TaskState& taskState, int)
{
  TaskState tmp(taskState);
  ++taskState;
  return tmp;
}

Task::Task()
	:mState(TaskStates::Pending)
{
}

TaskState Task::getState(void)
{
	return mState;
}

void Task::gotoNextState(void)
{
	// NEED MUTEX HERE?
	POLYVOX_ASSERT(mState != TaskStates::Finished, "Already on the finished state!");
	mState++;
}