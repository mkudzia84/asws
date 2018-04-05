/*
	Implementation of the Asynchronous session
*/

#include <boost/thread.hpp>				/* @TODO: Replace <boost/thread.hpp> with <thread> from C++0x */
#include <boost/chrono.hpp>				/* @TODO: Replace <boost/chrono.hpp> with <chrono> from C++0x */

//--------------------------------------------------------------------------------

#include "async_session.h"

#include "../../workload/symphony/message.h"
#include "../../workload/common_data.h"

//--------------------------------------------------------------------------------

#include "../../common/format/prefix_stream.h"

//--------------------------------------------------------------------------------

using namespace asws::common;
using namespace asws::client::symphony;

//-------------------------------------------------------------------------------

void async_session::soam_callback::onException(soam::SoamException& exp) throw()
{
	format::scoped_prefix sp("[async_session::callback]");
	format::prefix_stream pserr(std::cerr);

	pserr << "Session Exception:" << exp.what() << std::endl;
	// TODO: Add extra handling to shut down book-keeping
}

//-------------------------------------------------------------------------------

void async_session::soam_callback::onResponse(soam::TaskOutputHandlePtr& task_handle) throw()
{
	std::stringstream prefix;
	prefix << "[async_session::callback][task:" << task_handle->getId() << "][output]";
	format::scoped_prefix sp(prefix.str());
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	// Register with bookkeeper
	_bookkeeper.reg_retrieval(task_handle);

	// Deserialize
	if (task_handle->isSuccessful()) {
		workload::iworkload_ptr payload = nullptr;

		try {
			using namespace asws::workload::symphony;
			message<message_type::output> msg;

			task_handle->populateTaskOutput(&msg);

			payload = msg.payload();
			psout << payload->report() << std::endl;
		}
		catch (soam::SoamException& e) {
			pserr << "Error deserializing the payload : " << e.what() << std::endl;
		}
	}
}

//-------------------------------------------------------------------------------

void async_session::open(session_attributes& attr)
{
	format::scoped_prefix sp("[async_session::open]");
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	// Start the book-keeping
	_bookkeeper.start();

	// @TODO: Add options for connection handling - needs Windows build
	psout << "Creating new session ASYNC : { name : " << attr.name << ", type : " << attr.type 
		<< "common-data : " << attr.common_data_size << " bytes }" << std::endl;

	// Extract flags
	bool async_send = false;

	if (session_flags::is_set(attr.flags, session_flags::send_overlapped))
		async_send = true;
	else
		async_send = false;

	psout << "async send mode : " << async_send << std::endl;

	// Setup the attributes
	soam::SessionCreationAttributes session_attr;
	session_attr.setSessionName(attr.name);
	session_attr.setSessionType(attr.type);
	session_attr.setSessionCallback(&_callback);
	
	int sym_flags = soam::Session::ReceiveAsync;

	if (async_send)
		sym_flags |= soam::Session::SendOverlapped;
	else
		sym_flags |= soam::Session::SendSync;

	session_attr.setSessionFlags(sym_flags);

	// Add common data
	if (attr.common_data_size != 0) {
		using namespace asws::workload::symphony;

		psout << "Attaching common data" << std::endl;

		workload::iworkload_ptr cdata(new workload::common_data(attr.common_data_size));
		message<message_type::input> msg(cdata);
		session_attr.setCommonData(&msg);
	}

	// Set flags
	set_flags(attr.flags);

	// ---------------------------------------------------
	// Sym connection patch code
	// ---------------------------------------------------

	while (true) {
		soam::ConnectionPtr conn = _client.get_soam_connection();

		// Wait until connection is established
		while (conn->getState() == soam::Connection::CONNECTING || conn->getState() == soam::Connection::RECONNECTING) {
			psout << "SOAM connection being (re-)established" << std::endl;
			boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
		}

		// Terminal states
		if (conn->getState() == soam::Connection::CLOSED) {
			_state = interface_isession::closed;
			throw std::runtime_error("SOAM connection terminated");
		}

		if (conn->getState() == soam::Connection::INVALID) {
			_state = interface_isession::invalid;
			throw std::runtime_error("SOAM connection invalid");
		}

		// End state
		if (conn->getState() == soam::Connection::ESTABLISHED) {
			try {
				_session = conn->createSession(session_attr);
				// Finish
				break;
			} catch (soam::SoamException& e) {
				// Connection problem - retry the whole loop
				if (e.getErrorCode() == soam::SoamException::CONNECTION_ERROR)
					pserr << "Connection problem while establishing a session : " << e.what() << std::endl;
				else {
					_state = interface_isession::invalid;
					throw std::runtime_error(e.what());
				}
			}
		}
	}

	// Identifier
	std::stringstream session_id;
	session_id << attr.name << "-" << _session->getId();
	_name = session_id.str();

	psout << "Succesfully created the session, session-id : " << _name << std::endl;

	// Mark as established
	_state = interface_isession::established;
}

//-------------------------------------------------------------------------------

void async_session::close(bool wait_for_results)
{
	std::stringstream prefix;
	prefix << "[session-" << _name << "::close]";
	format::scoped_prefix(prefix.str());
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	if (wait_for_results) {
		psout << "Waiting for results of tasks" << std::endl;
		wait();
	}

	// Close the session
	psout << "Closing the session" << std::endl;

	// Clear
	_name = "";

	// ---------------------------------------------------
	// Sym connection patch code
	// ---------------------------------------------------

	while (true) {
		soam::ConnectionPtr conn = _client.get_soam_connection();

		// Wait until connection is established
		while (conn->getState() == soam::Connection::CONNECTING || conn->getState() == soam::Connection::RECONNECTING) {
			psout << "SOAM connection being (re-)established" << std::endl;
			boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
		}

		// Terminal states
		if (conn->getState() == soam::Connection::CLOSED) {
			_state = interface_isession::closed;
			throw std::runtime_error("SOAM connection closed, operating on invalid connection");
		}

		if (conn->getState() == soam::Connection::INVALID) {
			_state = interface_isession::invalid;
			psout << "SOAM connection invalid, will do a client side close() anyway" << std::endl;
			_session = nullptr;
			return;
		}

		// End state
		if (conn->getState() == soam::Connection::ESTABLISHED) {
			try {
				_session->close();
				_session = nullptr;
				_state = interface_isession::closed;
				break;
			} catch (soam::SoamException& e) {
				// Try again
				if (e.getErrorCode() == soam::SoamException::CONNECTION_ERROR)
					pserr << "Connection problem while closing the session : " << e.what() << std::endl;
				else {
					pserr << "Unknown error code : " << e.what() << std::endl;
					_state = interface_isession::invalid;
					throw std::runtime_error(e.what());
				}
			}
		}
	}

	psout << "Succesfully closed the session" << std::endl;
}

//-------------------------------------------------------------------------------

void async_session::submit_segment_start()
{
	std::stringstream prefix;
	prefix << "[session-" << _name << "::submit]";
	format::scoped_prefix sp(prefix.str());
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	// Submission segment starts now
	psout << "Starting workload submission" << std::endl;

	// Start the monitoring thread anyway
	psout << "Starting the workload monitoring/bookkeeping thread" << std::endl;
	_bookkeeper.async_submission_start(_name);
}

//-------------------------------------------------------------------------------

void async_session::submit_segment_end()
{
	std::stringstream prefix;
	prefix << "[session-" << _name << "::submit]";
	format::scoped_prefix sp(prefix.str());
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	// Submission segment ends now
	psout << "Finished workload submission" << std::endl;

	// Wait for monitoring thread to finish all workload
	psout << "Waiting for monitoring thread to confirm submissions" << std::endl;
	_bookkeeper.async_submission_end();
}

//-------------------------------------------------------------------------------

void async_session::submit(asws::workload::iworkload_ptr workload)
{
	std::stringstream prefix;
	prefix << "[session-" << _name << "::submit]";
	format::scoped_prefix ns(prefix.str());
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	// Create the message
	using namespace asws::workload::symphony;

	// Pass the ownership down
	message<message_type::input> msg(workload);

	try {
		// Throttle down
		_bookkeeper.async_throttle_submission(500);

		soam::TaskSubmissionAttributes task_attr;
		task_attr.setTaskInput(&msg);

		// Send the task
		soam::TaskInputHandlePtr task_handle = _session->sendTaskInput(task_attr);

		// Record
		_bookkeeper.reg_submission(task_handle);
	} catch(soam::SoamException& e) {
		pserr << "Task submission failed : " << e.what() << std::endl;
		throw std::runtime_error(e.what());
	}
}

//-------------------------------------------------------------------------------

void async_session::wait()
{
	std::stringstream prefix;
	prefix << "[session-" << _name << "::wait]";
	format::scoped_prefix ns(prefix.str());
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	psout << "Waiting for submission to finish" << std::endl;
	_bookkeeper.async_submission_wait();

	// Get the stats
	std::stringstream stats;
	stats << "[stats] Collected statistics\n";
	stats << "[stats]-----------------------------------------------------\n";
	stats << "[stats] tasks-submitted          : " << _bookkeeper.get_tasks_submitted() << "\n";
	stats << "[stats] submission-succeeded     : " << _bookkeeper.get_tasks_confirmed() << "\n";
	stats << "[stats] submission-failure       : " << _bookkeeper.get_tasks_failed() << "\n";
	stats << "[stats] submission-time [s]      : " << _bookkeeper.get_submission_time() << "\n";
	stats << "[stats] submission-rate [task/s] : " << _bookkeeper.get_tasks_confirmed() / _bookkeeper.get_submission_time() << "\n";
	stats << "[stats]-----------------------------------------------------\n";
	stats << std::flush;

	psout << stats.str() << std::flush;

	// Wait for all the messages
	psout << "Waiting for retrieval to finish" << std::endl;
	_bookkeeper.async_retrieval_wait();

	// Get the stats
	stats << "[stats]-----------------------------------------------------\n";
	stats << "[stats] tasks-retrieved          : " << _bookkeeper.get_tasks_retrieved() << "\n";
	stats << "[stats] execution-time [s]       : " << _bookkeeper.get_execution_time() << "\n";
	stats << "[stats]-----------------------------------------------------\n";
	stats << std::flush;
	psout << stats.str() << std::flush;
}

//-------------------------------------------------------------------------------
