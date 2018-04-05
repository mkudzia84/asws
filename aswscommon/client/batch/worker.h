#pragma once

// worker.h
//
//  (C) Copyright 2014 Marcin Kudzia
//

#include "interface_isession.h"

//--------------------------------------------------------------------------------

#include <boost/thread.hpp>

//--------------------------------------------------------------------------------

namespace asws              {
namespace client            {
namespace batch             {

//--------------------------------------------------------------------------------

/* worker 
 Encapsulates the thread and thread associated workload
*/
class worker
{
public:

	/* Destructor */
	worker(const std::string& name)
		: _name(name), _thread(nullptr)
	{ }

	/* Start the thread */
	void start();

	/* Wait for it to finish */
	void join();

	/* Get the name */
	const std::string& get_name() const 
	{
		return _name;
	}

	/* Add a session to be executed */
	void add_action(std::unique_ptr<interface_isession>&& p)
	{
		_batch.push_back(std::move(p));
	}

private:

	// Worker functions
	void execute();

	// List of the sessions to execute
	std::list<std::unique_ptr<interface_isession>> _batch;

	// Name of the thread
	std::string _name;

	// Thread
	boost::thread* _thread;
}; // class worker

//--------------------------------------------------------------------------------

}	// namespace batch
}	// namespace client
}	// namespace asws

//--------------------------------------------------------------------------------
