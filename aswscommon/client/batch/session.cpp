/*
	Implementation of session action
*/

#include "session.h"

#include "../../common/format/prefix_stream.h"

//--------------------------------------------------------------------------------

using namespace asws::common;
using namespace asws::client::batch;

//--------------------------------------------------------------------------------

void session::open()
{
	assert(_session != nullptr);

	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	try {
		// Open the session
		_session->open(_attrs);

		// Start submission to the session
		_session->submit_segment_start();
	} catch (std::exception const &e) {
		pserr << "Error opening the session : " << e.what() << std::endl;
		std::stringstream err;
		pserr << "[batch::session] error opening the session : " << e.what();
		throw std::runtime_error(err.str().c_str());
	}
}

//--------------------------------------------------------------------------------

void session::submit_task()
{
	assert(_session != nullptr);

	if (_queue.empty()) {
		throw std::runtime_error("[batch::session] no more tasks in the queue");
	} else {
		while (!_queue.empty()) {
			interface_itask_queue* tqp = _queue.front().get();
		
			if (tqp->is_done()) {
				_queue.pop();
				continue;
			}
				
			_session->submit(tqp->get_next());
			if (tqp->is_done())
				_queue.pop();
			break;
		}
	}

	// If no more tasks in the queue - end submission
	if (_queue.empty())
		_session->submit_segment_end();
}

//--------------------------------------------------------------------------------

bool session::is_submission_done() const
{
	return _queue.empty();
}

//--------------------------------------------------------------------------------

void session::close()
{
	assert(_session != nullptr);

	// Prepare
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	if (!_queue.empty())
		throw std::runtime_error("[batch::session] not all of the tasks processed");

	try {
		
		// Wait for the results
		_session->wait();
		_session->close();
	} catch (std::exception const &e) {
		std::stringstream err;
		pserr << "[batch::session] error closing the session : " << e.what();
		throw std::runtime_error(err.str().c_str());
	}

	_session = nullptr;
}

//--------------------------------------------------------------------------------

void session::run()
{
	assert(_session != nullptr);

	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	// Perorm back to end execution
	try {

		// Open the session
		_session->open(_attrs);

		// Submit workload
		_session->submit_segment_start();

		while (!_queue.empty()) {
			workload::iworkload_ptr next = nullptr;
			while ((next = _queue.front()->get_next()) != nullptr)
				_session->submit(next);
			
			// Empty
			_queue.pop();
		}

		_session->submit_segment_end();

		// Wait for results
		_session->wait();

		// Close
		_session->close(false);

	} catch (std::exception const &e) {
		std::stringstream err;
		pserr << "[batch::session] error executing the session : " << e.what();
		throw std::runtime_error(err.str().c_str());
	}
}

//--------------------------------------------------------------------------------

