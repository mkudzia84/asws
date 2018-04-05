#pragma once

// task_repeater.h
//
//  (C) Copyright 2014 Marcin Kudzia
//

#include "interface_itask_queue.h"

//--------------------------------------------------------------------------------

namespace asws              {
namespace client            {
namespace batch             {

//--------------------------------------------------------------------------------

/* task_repeater
 Executed the underlying sequence n-times
*/
class task_repeater
	: public interface_itask_queue
{
public:

	/* Constructor */
	task_repeater(unsigned int num_repeat)
		: _nrun(num_repeat), _nmax(num_repeat)
	{ }

	/* Set the workload
	 Set the underlying queue 
	*/
	void set_workload(std::unique_ptr<interface_itask_queue>&& tqp)
	{
		_workload = std::move(tqp);
	}

	/* Get next task */
	virtual workload::iworkload_ptr get_next()
	{
		while (_nrun > 0) {
			if (_workload->is_done()) {
				_nrun--;
				_workload->reset();
				continue;
			} else {
				workload::iworkload_ptr next = _workload->get_next();
				if (_workload->is_done()) {
					_nrun--;
					_workload->reset();
				}
				return next;
			}

			return _workload->get_next();
		}

		return nullptr;
	}

	/* Check if is done */
	virtual bool is_done() const 
	{
		return _nrun == 0;
	}

	/* Reset */
	virtual void reset() 
	{
		_workload->reset();
		_nrun = _nmax;
	}

private:

	std::unique_ptr<interface_itask_queue> _workload;

	// Number of reperitions done
	unsigned int _nrun;
	// Number of repetitions to do
	unsigned int _nmax;

}; // class task_repeat

//--------------------------------------------------------------------------------

}	// namespace batch
}	// namespace client
}	// namespace asws

//--------------------------------------------------------------------------------
