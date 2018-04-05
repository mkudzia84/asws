#pragma once

// interface_isession.h
//
//  (C) Copyright 2014 Marcin Kudzia
//

//--------------------------------------------------------------------------------

#include <memory>

//--------------------------------------------------------------------------------

#include "../../workload/interface_iworkload.h"

//--------------------------------------------------------------------------------

#include "session_flags.h"
#include "session_attributes.h"

//--------------------------------------------------------------------------------

namespace asws              {
namespace client            {
namespace symphony          {

//--------------------------------------------------------------------------------

/* 
	Session interface
*/
class interface_isession
{
public:

	// Session type
	typedef enum {
		sync  = 0,
		async = 1
	} type;

	// Session state
	typedef enum {
		closed      = 0,
		established = 1,
		invalid     = 2
	} state;

	/* Destructor */
	virtual ~interface_isession() { }

	/* Get the type */
	type get_type() const { return _type; }

	/* Get the flags */
	int get_flags() const { return _flags; }

	/* Open the session */
	virtual void open(session_attributes& attr) = 0;

	/* Close the session */
	virtual void close(bool wait_for_results = false) = 0;

	/* Notify the monitoring submission of workload starts now */
	virtual void submit_segment_start() = 0;
	/* Notify the monitoring submission of workload finishes now */
	virtual void submit_segment_end() = 0;

	/* Submit workload
	 transfers the ownership of workload
	*/
	virtual void submit(workload::iworkload_ptr workload) = 0;

	/* Wait for results of submitted tasks
	 Blocks the thread
	*/
	virtual void wait() = 0;

	/* Get the state */
	virtual state get_state() const = 0;

	/* Get the id of the session */
	virtual std::string get_id() const = 0;

protected:

	/* Constructor */
	interface_isession(type tp)
		: _type(tp), _flags(0)
	{ }

	
	/* Set the flags */
	void set_flags(int f) { _flags = f; }

private:

	// type
	type _type;

	// flags
	int _flags;
}; // class client

//--------------------------------------------------------------------------------

typedef std::shared_ptr<interface_isession> isession_ptr;

//--------------------------------------------------------------------------------

}	// namespace symphony
}	// namespace client
}	// namespace asws

//--------------------------------------------------------------------------------

