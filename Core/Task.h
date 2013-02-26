#ifndef CUBIQUITY_TASK_H_
#define CUBIQUITY_TASK_H_

class Task
{
public:
	Task() {}
	virtual ~Task() {}

	virtual void process(void) = 0;
};

#endif //CUBIQUITY_TASK_H_