#ifndef CUBIQUITY_TASK_H_
#define CUBIQUITY_TASK_H_

#include "PolyVoxCore/Impl/ErrorHandling.h"

namespace TaskStates
{
	enum TaskState
	{
		Pending,
		Processing,
		Finished
	};
}
typedef TaskStates::TaskState TaskState;

TaskState& operator++(TaskState& taskState);
TaskState operator++(TaskState& taskState, int);

class Task
{
public:
	Task();

	TaskState getState(void);
	void gotoNextState(void);

	virtual void process(void) = 0;

private:
	TaskState mState;
};

#endif //CUBIQUITY_TASK_H_