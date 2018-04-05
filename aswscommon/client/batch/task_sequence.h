#pragma once

// task_sequence.h
//
//  (C) Copyright 2014 Marcin Kudzia
//

#include "interface_itask_queue.h"

//--------------------------------------------------------------------------------

#include <queue>
#include <cassert>

//--------------------------------------------------------------------------------

namespace asws              {
namespace client            {
namespace batch             {

//--------------------------------------------------------------------------------

/* task_sequence
 Executes the items in sequence
 May contain sub-task-queues
*/
class task_sequence
	: public interface_itask_queue
{
public:

	/* Constructor */
	task_sequence()
	{
		// Sets the _current to .end()
		_current = _workload.begin();
	}

	/* Destructor */
	virtual ~task_sequence() {}

	/* Add new item to the sequence */
	void add_workload(std::unique_ptr<interface_itask_queue>&& tqp)
	{
		if (_workload.empty()) {
			_workload.push_back(std::move(tqp));
			reset();
		} else {
			_workload.push_back(std::move(tqp));
		}
	}

	/* Get next task 
	 returns nullptr if no more tasks in collection
	*/
	virtual workload::iworkload_ptr get_next()
	{
		while (true) {
			if (_current == _workload.end())
				return nullptr;

			// Get the next one from queue
			interface_itask_queue* tqp = _current->get();

			if (tqp->is_done()) {
				_current++;
				continue;
			} else {
				workload::iworkload_ptr next = tqp->get_next();
				if (tqp->is_done())
					_current++;
				return next;
			}
		}
	}

	/* Check if is done */
	virtual bool is_done() const
	{
		return _current == _workload.end();
	}

	/* Reset all underlying collections */
	virtual void reset() 
	{
		for (auto itr = _workload.begin(); itr != _workload.end(); itr++)
			(*itr)->reset();
		_current = _workload.begin();
	}

private:

	// Current location 
	std::list<std::unique_ptr<interface_itask_queue>>::iterator _current;
	// Workload
	std::list<std::unique_ptr<interface_itask_queue>> _workload;
}; // class task_repeat

//--------------------------------------------------------------------------------

}	// namespace batch
}	// namespace client
}	// namespace asws

//--------------------------------------------------------------------------------