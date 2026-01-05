#include "job.h"

job::job(unsigned long long job_id)
{
	this->_job_id = job_id;
	this->_job_data = nullptr;
	this->_job_priority = job_priority::NORMAL_PRIORITY;
	this->_job_callback_1 = nullptr;
	this->_job_callback_2 = nullptr;
}

job::job(unsigned long long job_id, const std::function<void(std::vector<unsigned char >)>& job_callback) :
	job(job_id)
{
	this->_job_callback_1 = job_callback;
}

job::job(unsigned long long job_id, job_priority job_priority, const std::function<void(std::vector<unsigned char >)>& job_callback) :
	job(job_id, job_callback)
{
	this->_job_priority = job_priority;
}

job::job(unsigned long long job_id, job_data* job_data, const std::function<void(std::vector<unsigned char >)>& job_callback) :
	job(job_id)
{
	this->_job_data = job_data;
	this->_job_callback_1 = job_callback;
}

job::job(unsigned long long job_id, job_priority job_priority, job_data* job_data, const std::function<void(std::vector<unsigned char >)>& job_callback) :
	job(job_id, job_data, job_callback)
{
	this->_job_priority = job_priority;
}

job::job(const std::function<void(std::vector<unsigned char >)>& job_callback) :
	job(0)
{
	this->_job_callback_1 = job_callback;
	this->_job_callback_2 = nullptr;
}

job::job(job_priority job_priority, const std::function<void(std::vector<unsigned char >)>& job_callback) :
	job(job_callback)
{
	this->_job_priority = job_priority;
}

job::job(unsigned long long job_id, const std::function<void(callback_data*)>& job_callback) :
	job(job_id)
{
	this->_job_id = job_id;
	this->_job_callback_2 = job_callback;
}

job::job(unsigned long long job_id, job_priority job_priority, const std::function<void(callback_data*)>& job_callback) :
	job(job_id, job_callback)
{
	this->_job_priority = job_priority;
}

job::job(unsigned long long job_id, job_data* job_data, const std::function<void(callback_data*)>& job_callback) :
	job(job_id)
{
	this->_job_id = job_id;
	this->_job_data = job_data;
	this->_job_callback_2 = job_callback;
}

job::job(unsigned long long job_id, job_priority job_priority, job_data* job_data, const std::function<void(callback_data*)>& job_callback) :
	job(job_id, job_data, job_callback)
{
	this->_job_priority = job_priority;
}

job::job(const std::function<void(callback_data*)>& job_callback) :
	job(0)
{
	this->_job_callback_1 = nullptr;
	this->_job_callback_2 = job_callback;
}

job::job(job_priority job_priority, const std::function<void(callback_data*)>& job_callback) :
	job(job_callback)
{
	this->_job_priority = job_priority;
}

job::~job()
{
	this->_job_callback_1 = nullptr;
	this->_job_callback_2 = nullptr;

	if (this->_job_data != nullptr)
	{
		delete this->_job_data;
		this->_job_data = nullptr;
	}
}

void job::setJobId(const unsigned long long job_id)
{
	this->_job_id = job_id;
}

void job::setJobData(job_data* job_data)
{
	this->_job_data = job_data;
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
	return this->_job_data;
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