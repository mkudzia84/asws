/*
	Implementation of worker pool
*/

#include "worker_pool.h"

#include "../../common/format/prefix_stream.h"

//--------------------------------------------------------------------------------

using namespace asws::common;
using namespace asws::client::batch;

//--------------------------------------------------------------------------------

void worker_pool::start()
{
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	psout << "Starting the worker threads" << std::endl;

	for (auto worker_itr = _workers.begin(); worker_itr != _workers.end(); worker_itr++) {
		try {
			worker* wp = worker_itr->get();
			wp->start();
		} catch (std::exception const &e) {
			pserr << "Error starting worker : " << e.what() << std::endl;
		} catch (...) {
			pserr << "Unhandled exception..." << std::endl;
		}
	}

	psout << "Finished starting the workers" << std::endl;
}

//--------------------------------------------------------------------------------

void worker_pool::finish()
{
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	psout << "Waiting for worker threads to finish" << std::endl;

	for (auto worker_itr = _workers.begin(); worker_itr != _workers.end(); worker_itr++) {
		try {
			worker* wp = worker_itr->get();
			wp->join();
		} catch (std::exception const &e) {
			pserr << "Error while joining the worker thread : " << e.what() << std::endl;
		} catch (...) {
			pserr << "Unahndled exception..." << std::endl;
		}
	}

	psout << "All worker threads finished" << std::endl;
}

//--------------------------------------------------------------------------------
