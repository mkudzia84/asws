/*
	Implementation of session action
*/

#include "worker.h"

#include "../../common/format/prefix_stream.h"

//--------------------------------------------------------------------------------

using namespace asws::common;
using namespace asws::client::batch;

//--------------------------------------------------------------------------------

void worker::start()
{
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	psout << "Starting thread " << _name << std::endl;

	try {
		_thread = new boost::thread(std::bind(&worker::execute, this));
	} catch (std::exception &e) {
		pserr << "Failed to start the thread : " << e.what() << std::endl;
	}
}

//--------------------------------------------------------------------------------

void worker::join()
{
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	try {
		_thread->join();
		delete _thread;
		_thread = nullptr;
	} catch (std::exception const &e) {
		pserr << "Error while waiting for thread to finish : " << e.what() << std::endl;
	}
}

//--------------------------------------------------------------------------------

void worker::execute()
{
	// Prepate
	std::stringstream prefix;
	prefix << "[thread:" << _name << "]";
	format::scoped_prefix sp(prefix.str());
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);


	// Execute the workload
	psout << "Executing the workload inside of the thread" << std::endl;

	try {
		for (auto session_itr = _batch.begin(); session_itr != _batch.end(); session_itr++) {
			interface_isession* sp = session_itr->get();
			sp->run();
		}
	} catch (std::exception const &e) {
		pserr << "Error in batch execution : " << e.what() << std::endl;
	} catch (...) {
		pserr << "Unhandled exception in batch execution, finishing..." << std::endl;
	}

	psout << "Worker thread finished" << std::endl;
}

//--------------------------------------------------------------------------------
