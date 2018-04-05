#pragma once

// worker_pool.h
//
//  (C) Copyright 2014 Marcin Kudzia
//


#include "worker.h"

//--------------------------------------------------------------------------------

#include <list>

//--------------------------------------------------------------------------------

namespace asws              {
namespace client            {
namespace batch             {

//--------------------------------------------------------------------------------

/* worker_pool
 Collections of the worker threads
*/
class worker_pool
{
public:

	/* Constructor */
	worker_pool()
	{ }

	/* Add new worker */
	void add(std::unique_ptr<worker>&& wp)
	{
		_workers.push_back(std::move(wp));
	}

	/* Start all workers */
	void start();

	/* Wait for all workers to finish */
	void finish();

private:
	
	// List of workers
	std::list<std::unique_ptr<worker>> _workers;
}; // class worker_pool

//--------------------------------------------------------------------------------

}	// namespace batch
}	// namespace client
}	// namespace asws

//--------------------------------------------------------------------------------
