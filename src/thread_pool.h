#pragma once

#include <atomic>
#include <chrono>
#include <future>
#include <mutex>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#include <functional>

#include "job_manager.h"
#include "thread_worker.h"

class job_manager;
class thread_pool: public std::enable_shared_from_this<thread_pool>
{
public:
	thread_pool();
	virtual ~thread_pool();

public:
	std::shared_ptr<thread_pool> getPtr(void);

public:
	void addWorker(std::shared_ptr<thread_worker> new_worker);
	void removeWorker(std::shared_ptr<thread_worker> worker);
	void removeWorkers();

public:
	std::map<job_priority, int> _priority_worker_numbers;
	void setWorkersPriorityNumbers();
	int getWorkerNumbers();

public:
	void addJob(std::shared_ptr<job> new_job);

public:
	template <typename F, typename... Args>
	auto submit(F&& func, Args&&... args)
		-> std::future<std::invoke_result_t<F, Args...>>
	{
		return submit(job_priority::NORMAL_PRIORITY, std::forward<F>(func), std::forward<Args>(args)...);
	}

	template <typename F, typename... Args>
	auto submit(job_priority priority, F&& func, Args&&... args)
		-> std::future<std::invoke_result_t<F, Args...>>
	{
		using return_type = std::invoke_result_t<F, Args...>;

		if (this->_terminated)
		{
			std::promise<return_type> promise;
			promise.set_exception(std::make_exception_ptr(std::runtime_error("thread_pool is terminated")));
			return promise.get_future();
		}

		auto task = std::make_shared<std::packaged_task<return_type()>>(
			[func = std::forward<F>(func), args_tuple = std::make_tuple(std::forward<Args>(args)...)]() mutable -> return_type
			{
				return std::apply(std::move(func), std::move(args_tuple));
			});

		auto future = task->get_future();
		auto task_job = std::make_shared<job>(priority, [task]() mutable { (*task)(); });
		addJob(task_job);

		return future;
	}

public:
	void stopPool(bool wait_for_finish_jobs = false, std::chrono::seconds max_wait_time = std::chrono::seconds(0));

public:
	std::weak_ptr<job_manager> getJobManager();

private:
	std::mutex _woker_mutex;

	std::atomic_bool _terminated;

	std::shared_ptr<job_manager> _job_manager;
	std::vector<std::shared_ptr<thread_worker>> _workers;

public:
	void notifyWakeUpWorkers();

};
