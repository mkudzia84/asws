#pragma once

// interface_itask_queue.h
//
//  (C) Copyright 2014 Marcin Kudzia
//

#include "../../workload/interface_iworkload.h"

//--------------------------------------------------------------------------------

#include <memory>

//--------------------------------------------------------------------------------

namespace asws              {
namespace client            {
namespace batch             {

//--------------------------------------------------------------------------------

/* interface_itask_queue
 Queue of tasks, provides interface to process tasks
 one at a time
*/
class interface_itask_queue
{
public:

	/* Destructor */
	virtual ~interface_itask_queue()
	{ }

	/* Get the next task in queue 
	 If reached end of the queue - return nullptr */
	virtual workload::iworkload_ptr get_next() = 0;

	/* Reset the submission queue 
	 Should restore the state */
	virtual void reset() = 0;

	/* Check if is done */
	virtual bool is_done() const = 0;

}; // class interface_itask_queue

//--------------------------------------------------------------------------------

}	// namespace batch
}	// namespace client
}	// namespace asws
