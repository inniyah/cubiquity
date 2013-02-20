template <typename TaskType>
TaskProcessor<TaskType>::TaskProcessor()
{
}

template <typename TaskType>
TaskProcessor<TaskType>::~TaskProcessor()
{
}

template <typename TaskType>
void TaskProcessor<TaskType>::addTask(TaskType task)
{
	mPendingTasks.push_back(task);
}

template <typename TaskType>
bool TaskProcessor<TaskType>::hasAnyFinishedTasks(void)
{
	return mFinishedTasks.size() > 0;
}

template <typename TaskType>
TaskType TaskProcessor<TaskType>::removeFirstFinishedTask(void)
{
	TaskType task = mFinishedTasks.front();
	mFinishedTasks.pop_front();
	return task;
}

template <typename TaskType>
void TaskProcessor<TaskType>::processOneTask(void)
{
	if(mPendingTasks.size() > 0)
	{
		TaskType task = mPendingTasks.front();
		mPendingTasks.pop_front();

		task.process();

		mFinishedTasks.push_back(task);
	}
}