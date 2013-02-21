template <typename TaskType>
MainThreadTaskProcessor<TaskType>::MainThreadTaskProcessor()
	:TaskProcessor()
{
}

template <typename TaskType>
MainThreadTaskProcessor<TaskType>::~MainThreadTaskProcessor()
{
}

template <typename TaskType>
void MainThreadTaskProcessor<TaskType>::addTask(TaskType task)
{
	mPendingTasks.push_back(task);
}

template <typename TaskType>
bool MainThreadTaskProcessor<TaskType>::hasAnyFinishedTasks(void)
{
	return mFinishedTasks.size() > 0;
}

template <typename TaskType>
TaskType MainThreadTaskProcessor<TaskType>::removeFirstFinishedTask(void)
{
	TaskType task = mFinishedTasks.front();
	mFinishedTasks.pop_front();
	return task;
}

template <typename TaskType>
void MainThreadTaskProcessor<TaskType>::processOneTask(void)
{
	if(mPendingTasks.size() > 0)
	{
		TaskType task = mPendingTasks.front();
		mPendingTasks.pop_front();

		task.process();

		mFinishedTasks.push_back(task);
	}
}