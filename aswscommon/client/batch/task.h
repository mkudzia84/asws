#pragma once

// task.h
//
//  (C) Copyright 2014 Marcin Kudzia
//

#include "interface_itask_queue.h"

#include "../../workload/factory.h"

//--------------------------------------------------------------------------------

namespace asws              {
namespace client            {
namespace batch             {

//--------------------------------------------------------------------------------

/* task
 acts as a single task, is treated as a single item queue
*/
class task
	: public interface_itask_queue
{
public:

	// Typedefs
	typedef workload::factory_ex::identifier_type type_id;
	typedef workload::factory_ex::params_type params;

	/* Constructor */
	task(type_id id, const params& p)
		: _type_id(id), _params(p), _is_executed(false)
	{ }

	/* Instantiate the task */
	virtual workload::iworkload_ptr instantiate()
	{
		return workload::iworkload_ptr(
			workload::factory_ex::instance()->instantiate(_type_id, _params)
		);
	}

	/* Get the next task
	 Invalidates after execution untill reseted
	*/
	virtual workload::iworkload_ptr get_next()
	{
		if (_is_executed)
			return nullptr;
		_is_executed = true;
		return instantiate();
	}

	/* Check if is done */
	virtual bool is_done() const
	{
		return _is_executed;
	}

	/* Reset */
	virtual void reset()
	{
		_is_executed = false;
	}

private:

	// Workload params
	type_id _type_id;
	params _params;

	// Has been "extracted"
	bool _is_executed;

}; // class task

//--------------------------------------------------------------------------------

}	// namespace batch
}	// namespace client
}	// namespace asws