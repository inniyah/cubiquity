#ifndef CUBIQUITY_BACKGROUNDTASKPROCESSOR_H_
#define CUBIQUITY_BACKGROUNDTASKPROCESSOR_H_

#include "ConcurrentQueue.h"
#include "TaskProcessor.h"

//#include "boost/thread.hpp"

#include <list>

namespace Cubiquity
{
	/*class BackgroundTaskProcessor : public TaskProcessor
	{
	public:
		BackgroundTaskProcessor(uint32_t noOfThreads);
		virtual ~BackgroundTaskProcessor();

		void addTask(Task* task);

		bool hasAnyFinishedTasks(void);
		Task* removeFirstFinishedTask(void);

		void processTasks(void);

		concurrent_queue<Task*, TaskSortCriterion> mPendingTasks;

		std::list<boost::thread*> mThreads;
	};

	extern BackgroundTaskProcessor gBackgroundTaskProcessor;*/

	class BackgroundTaskProcessor : public TaskProcessor
	{
	public:
		BackgroundTaskProcessor();
		virtual ~BackgroundTaskProcessor();

		void addTask(Task* task);
		bool hasTasks(void);

		virtual void processOneTask(void)/* = 0*/;
		virtual void processAllTasks(void)/* = 0*/;

		std::list<Task*> mPendingTasks;
	};

	extern BackgroundTaskProcessor gBackgroundTaskProcessor;
}

#endif //CUBIQUITY_BACKGROUNDTASKPROCESSOR_H_
