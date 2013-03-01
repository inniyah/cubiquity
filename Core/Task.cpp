#include "Task.h"

TaskState& operator++(TaskState& taskState)
{
	POLYVOX_ASSERT(taskState != TaskStates::Finished, "Cannot increment the finished task state!");
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
	
	mState++;
}