#pragma once

// session.h
//
//  (C) Copyright 2014 Marcin Kudzia
//

#include "interface_isession.h"
#include "interface_itask_queue.h"

#include "../symphony/interface_isession.h"

#include <queue>

//--------------------------------------------------------------------------------

namespace asws              {
namespace client            {
namespace batch             {

//--------------------------------------------------------------------------------

/* session
 Performs submission of the workload of the session
 Provides a high level call that executes the :
 - open, submit, wait, close sequence
*/

class session
	: public interface_isession
{
	friend class session_scheduler;
public:

	/* Constructor */
	session(symphony::isession_ptr sp, symphony::session_attributes attr)
		: _session(sp), _attrs(attr)
	{ }

	/* Execute the session workload */
	virtual void run();

private:

	/* Open the session 
	 Used by session_scheduler
	*/
	void open(); 

	/* Submit the next task 
	 Used by session_scheduler
	*/
	void submit_task();

	/* Check if submission is completed 
	 Used by session_scheduler 
	*/
	bool is_submission_done() const;

	/* Close the session 
	 Used by session_scheduler
	*/
	void close();

public:

	/* Set the workload */
	void add_workload(std::unique_ptr<interface_itask_queue>&& tqp)
	{
		_queue.push(std::move(tqp));
	}

private:

	// Symphony session
	symphony::isession_ptr _session;
	// Symphony session attributes
	symphony::session_attributes _attrs;

	// Task queue
	std::queue<std::unique_ptr<interface_itask_queue>> _queue;

}; // class session

//--------------------------------------------------------------------------------

}	// namespace batch
}	// namespace client
}	// namespace asws

//--------------------------------------------------------------------------------
