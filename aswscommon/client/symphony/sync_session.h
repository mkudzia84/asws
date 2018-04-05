#pragma once

// sync_session.h
//
//  (C) Copyright 2014 Marcin Kudzia
//

//--------------------------------------------------------------------------------

#include "interface_isession.h"
#include "soam_client.h"
#include "bookkeeper.h"

//--------------------------------------------------------------------------------

namespace asws              {
namespace client            {
namespace symphony          {

//--------------------------------------------------------------------------------

/* 
	Synchronous session
*/
class sync_session
	: public interface_isession
{
public:

	/* Constructor */
	sync_session(soam_client& cl)
		: interface_isession(interface_isession::sync), _client(cl)
	{ }

	/* Destructor */
	~sync_session()
	{ }

	/* Open the session */
	virtual void open(session_attributes& attr);

	/* Close the session */
	virtual void close(bool wait_for_results = false);

	/* Start the submission of workload segment
	 if message overlap enabled - will start the monitoring thread
	*/
	virtual void submit_segment_start();

	/* Stop the submission 
	 if message overlap enabled - will block untill the ACKs have arrived
	*/
	virtual void submit_segment_end(); 

	/* Submit workload */
	virtual void submit(workload::iworkload_ptr workload);

	/* Wait for results of submitted tasks
	 Blocks until execution finished
	*/
	virtual void wait();

	/* Get the session id */
	virtual std::string get_id() const
	{ 
		return _name;
	}

	/* Get the state */
	virtual state get_state() const
	{
		if (_session == nullptr)
			return interface_isession::invalid;
		else 
			return _state;
	}

private:

	// Bookkeeping
	bookkeeper _bookkeeper;

	// Underlying grid client
	soam_client& _client;

	// Underlying symphony session
	soam::SessionPtr _session;

	// Session id
	std::string _name;
	// Session state
	interface_isession::state _state;
}; // class client

//--------------------------------------------------------------------------------

}	// namespace symphony
}	// namespace client
}	// namespace asws

//--------------------------------------------------------------------------------

