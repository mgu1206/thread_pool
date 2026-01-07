#include "job.h"

job::job(unsigned long long job_id)
{
	this->_job_id = job_id;
	this->_job_data = nullptr;
	this->_job_priority = job_priority::NORMAL_PRIORITY;
	this->_work_function = nullptr;
}

// Lambda-based constructors
job::job(unsigned long long job_id, std::function<void()> work_func) :
	job(job_id)
{
	this->_work_function = work_func;
}

job::job(unsigned long long job_id, job_priority job_priority, std::function<void()> work_func) :
	job(job_id, work_func)
{
	this->_job_priority = job_priority;
}

job::job(unsigned long long job_id, std::unique_ptr<job_data>&& job_data, std::function<void()> work_func) :
	job(job_id)
{
	this->_job_data = std::move(job_data);
	this->_work_function = work_func;
}

job::job(unsigned long long job_id, job_priority job_priority, std::unique_ptr<job_data>&& job_data, std::function<void()> work_func) :
	job(job_id, std::move(job_data), work_func)
{
	this->_job_priority = job_priority;
}

job::job(std::function<void()> work_func) :
	job(0)
{
	this->_work_function = work_func;
}

job::job(job_priority job_priority, std::function<void()> work_func) :
	job(work_func)
{
	this->_job_priority = job_priority;
}

job::~job()
{
	this->_work_function = nullptr;
	// _job_data automatically destroyed by unique_ptr
}

void job::setJobId(const unsigned long long job_id)
{
	this->_job_id = job_id;
}

void job::setJobData(std::unique_ptr<job_data>&& job_data)
{
	this->_job_data = std::move(job_data);
}

void job::setJobPriority(job_priority job_priority)
{
	this->_job_priority = job_priority;
}

unsigned long long job::getJobId()
{
	return this->_job_id;
}

job_data* job::getJobData()
{
	return this->_job_data.get();
}

job_priority job::getJobPriority()
{
	return this->_job_priority;
}

void job::setJobManager(std::weak_ptr<job_manager> job_manager)
{
	this->_job_manager = job_manager;
}

std::shared_ptr<job> job::getPtr()
{
	return this->shared_from_this();
}

void job::work()
{
	// Default implementation: invoke lambda if provided
	if (this->_work_function)
	{
		this->_work_function();
	}
	// If _work_function is nullptr, this is inheritance pattern
	// Subclass MUST override work() or behavior is no-op
}