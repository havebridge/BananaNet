#pragma once

#include <thread>
#include <atomic>
#include <future>
#include <condition_variable>

#include <queue>
#include <vector>
#include <functional>

namespace Core
{
	class ThreadPool
	{
	private:
		std::vector<std::thread> threads;
		std::queue<std::function<void()>> jobs;
		std::mutex job_mutex;
		std::condition_variable cv;
		std::atomic<bool> is_terminated = false;

	private:
		void ThreadLoop()
		{
			std::function<void()> job;

			while (true)
			{
				{
					std::unique_lock<std::mutex> job_lock(job_mutex);

					cv.wait(job_lock, [this] {
						return !jobs.empty() || is_terminated;
						});

					if (is_terminated)
						return;

					job = std::move(jobs.front());
					jobs.pop();
				}

				job();
			}
		}

		void InitThreads()
		{
			int thread_count = std::thread::hardware_concurrency();

			for (int i = 0; i != thread_count; ++i)
			{
				threads.emplace_back(std::thread(&ThreadPool::ThreadLoop, this));
			}
		}
	public:
		ThreadPool()
		{
			InitThreads();
		}

		ThreadPool(const ThreadPool&) = delete;
		ThreadPool& operator=(const ThreadPool&) = delete;

		~ThreadPool()
		{
			for (auto& thread : threads)
			{
				if (thread.joinable())
				{
					thread.join();
				}
			}
		}

	public:
		template<typename F, typename... Args>
		auto AddJob(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
		{
			auto func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
			auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
			auto wrapper = [task_ptr]() {(*task_ptr)(); };

			{
				std::lock_guard<std::mutex> job_lock(job_mutex);
				jobs.push(wrapper);
				cv.notify_one();
			}

			return task_ptr->get_future();
		}

		template<typename F, typename... Args>
		void AddJob(const F& job, const Args&... args) { AddJob([job, args...]{ job(args...); }); }

		void Join()
		{
			for (auto& thread : threads)
			{
				if (thread.joinable())
				{
					thread.join();
				}
			}
		}

	};
}