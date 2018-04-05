#pragma once

// session_scheduler.h
//
//  (C) Copyright 2014 Marcin Kudzia
//

#include "session.h"

//--------------------------------------------------------------------------------

#include <list>

//--------------------------------------------------------------------------------

namespace asws              {
namespace client            {
namespace batch             {

//--------------------------------------------------------------------------------

/* session_scheduler
 Performs the submission of multiple sessions
 supports two modes, sequential execution or round robin
*/
class session_scheduler
	: public interface_isession
{
public:

	/* Session scheduler mode */
	typedef enum {
		round_robin		= 1,
		sequential		= 2
	} mode;

	/* Constructor */
	session_scheduler(mode m)
		: _mode(m)
	{ }

	/* Clear up the list */
	virtual ~session_scheduler() 
	{
		_sessions.clear();
	}

	/* Add the session to the scheduler */
	void add_session(std::unique_ptr<session>&& sp)
	{
		_sessions.push_back(std::move(sp));
	}

	/* Execute the sessions */
	virtual void run();

private:

	// Scheduler mode
	mode _mode;

	// List of the sessions
	std::list<std::unique_ptr<session>> _sessions;
}; // class session

//--------------------------------------------------------------------------------

}	// namespace batch
}	// namespace client
}	// namespace asws

//--------------------------------------------------------------------------------
