#pragma once

#ifdef _WIN32
#include <WinSock2.h>
#endif

#include <functional>
#include <vector>
#include <queue>

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

namespace Hnet
{
	class ThreadPool
	{
	private:
		bool should_terminate = false;
		std::mutex queue_mutex;
		std::condition_variable mutex_condition;
		std::vector<std::thread> threads;
		std::queue<std::function<void()>> jobs;
	private:
		ThreadPool();

	public:
		void Start();
		void QueueJob(const std::function<void()>& job);
		void Stop();
		void Busy();
	};
}