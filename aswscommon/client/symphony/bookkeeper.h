#pragma once

// bookkeeper.h
//
//  (C) Copyright 2014 Marcin Kudzia
//

//--------------------------------------------------------------------------------

#include <list>
#include <chrono>

//--------------------------------------------------------------------------------

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

//--------------------------------------------------------------------------------

#include <soam.h>

//--------------------------------------------------------------------------------

namespace asws               {
namespace client            {
namespace symphony          {

//--------------------------------------------------------------------------------

/*
	Bookkeeper
	Tracks the state of the tasks, is thread safe
*/

class bookkeeper 
{
public:

	/* Initialize stats */
	bookkeeper()
		: _submitted(0), _confirmed(0), _failed(0), _retrieved(0), _succeded(0), _errors(0)
	{ 
		_is_async_submission_done = true;
	}

	/* Execution starts
	 Mark the begining of the run
	*/
	void start();

	/* Submission starts 
		Starts the submission monitoring thread
	*/
	void async_submission_start(const std::string& name);

	/* Submission ends 
		Starts the retrieval monitoring thread
	*/
	void async_submission_end();

	/* Block untill all of the task sends are ACK/Failed */
	void async_submission_wait();

	/* Throttle down the submission */
	void async_throttle_submission(int threshold = 0);

	/* Block untill all of the task results are processed */
	void async_retrieval_wait();

	/* Register submission confirmed */
	void reg_submission(soam::TaskInputHandlePtr ptr);

	/* Register retrieval confirmed */
	void reg_retrieval(soam::TaskOutputHandlePtr ptr);

	/* Following getters dont need a mutex, as long 
	 as variables are marked as voltile
	 compiler should treat those operations as atomic */

	/* Get the number of task submitted */
	int get_tasks_submitted() const 
	{
		return _submitted;
	}

	/* Get the number of tasks succesfully submitted */
	int get_tasks_confirmed() const 
	{
		return _confirmed;
	}

	/* Get the number of failed submissions */
	int get_tasks_failed() const 
	{
		return _failed;
	}

	/* Get the number of tasks retrieved */
	int get_tasks_retrieved() const
	{
		return _retrieved;
	}

	/* Get the number outstanding */
	int get_tasks_outstanding() const;

	/* Check if submission is completed */
	bool is_submission_completed() const;

	/* Check if retrieval is completed */
	bool is_retrieval_completed() const;

	/* Get the submission time [s]*/
	double get_submission_time() const
	{
		std::chrono::duration<double> diff = _tp_submission_end - _tp_submission_start;
		return diff.count();
	}

	/* Get the end to end runtime [s] */
	double get_execution_time() const
	{
		std::chrono::duration<double > diff = _tp_execution_end - _tp_execution_start;
		return diff.count();
	}

private:

	// name
	std::string _name;

	// Queue used for submission requests
	std::list<soam::TaskInputHandlePtr>  _inp_handles;
	std::list<soam::TaskOutputHandlePtr> _out_handles;

	// Submission monitoring function
	void async_submission_func();
	// Retrieval monitoring function
	void async_retrieval_func();

	// Underlying thread
	boost::thread _submission_thread;
	boost::thread _retrieval_thread;
	mutable boost::mutex  _sm;
	mutable boost::mutex  _rm;

	// Flag specifing if submission is finished
	volatile bool _is_async_submission_done;

	// Bookkeeping
	volatile int _submitted;
	volatile int _confirmed;
	volatile int _failed;

	volatile int _retrieved;
	volatile int _succeded;
	volatile int _errors;

	// Timestamps
	std::chrono::high_resolution_clock::time_point _tp_submission_start;
	std::chrono::high_resolution_clock::time_point _tp_submission_end;
	std::chrono::high_resolution_clock::time_point _tp_execution_start;
	std::chrono::high_resolution_clock::time_point _tp_execution_end;

}; // class bookkeeper

//--------------------------------------------------------------------------------

} // namespace symphony
} // namespace client
} // namespace asws

//--------------------------------------------------------------------------------
