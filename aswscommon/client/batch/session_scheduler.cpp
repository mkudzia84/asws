/*
	Implementation of session action
*/

#include "session_scheduler.h"

#include "../../common/format/prefix_stream.h"

//--------------------------------------------------------------------------------

#include <soam.h>

//--------------------------------------------------------------------------------

using namespace asws::common;
using namespace asws::client::batch;

//--------------------------------------------------------------------------------

void session_scheduler::run()
{
	// Prepare
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	if (_mode == session_scheduler::sequential) {
		psout << "Sequential mode enabled : executing sessions" << std::endl;

		// Execute the sessions in sequence
		for (auto session_itr = _sessions.begin(); session_itr != _sessions.end(); session_itr++) {
			try {
				(*session_itr)->run();
			} catch (std::exception const &e) {
				pserr << "Failed executing the session : " << e.what() 
					<< ", continuing to next one..." << std::endl;
			}
		}
	}

	if (_mode == session_scheduler::round_robin) {
		psout << "Round robin mode enabled : executing the sessions" << std::endl;
		
		// Execute sessions - round robin style
		
		// Open the sessions
		psout << "Opening the sessions" << std::endl;

		for (auto session_itr = _sessions.begin(); session_itr != _sessions.end(); session_itr++) {

			try {
				(*session_itr)->open();
			} catch (std::exception const &e) {
				pserr << "Failed opening the session : " << e.what() << ", continuing to next one" << std::endl;
			}
		}

		// Submit tasks one at a time
		psout << "Submitting the workload" << std::endl;

		while (true) {
			bool all_done = true; 

			// Go thru all of the sessions
			for (auto session_itr = _sessions.begin(); session_itr != _sessions.end(); session_itr++) {
				session* sp = session_itr->get();
				
				if (!sp->is_submission_done()) {
					sp->submit_task();
					if (!sp->is_submission_done())
						all_done = false;
				}
			}

			// Submission done
			if (all_done)
				break;
		}

		psout << "Submission completed" << std::endl;

		// Wait for the results
		psout << "Waiting for the results" << std::endl;

		for (auto session_itr = _sessions.begin(); session_itr != _sessions.end(); session_itr++) {
			session* sp = session_itr->get();
			try {
				sp->close();
			} catch (std::exception const &e) {
				pserr << "Failed to close the session : " << e.what() << std::endl;
			}
		}
	}
}

//--------------------------------------------------------------------------------
