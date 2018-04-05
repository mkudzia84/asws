/*
	Implementation of the Idle Workload
*/

#include <boost/thread.hpp>				/* @TODO: Replace <boost/thread.hpp> with <thread> from C++0x */
#include <boost/chrono.hpp>				/* @TODO: Replace <boost/chrono.hpp> with <chrono> from C++0x */

#include "idle_workload.h"

//--------------------------------------------------------------------------------

using namespace asws::workload;

//--------------------------------------------------------------------------------

void idle_workload::execute()
{
	/* @TODO: Replace with C++0x */
	boost::this_thread::sleep_for(boost::chrono::milliseconds(runtime));
}

//--------------------------------------------------------------------------------

std::string idle_workload::report()
{
	std::stringstream sout;
	sout << "executed in " << runtime;
	return sout.str();
}

//--------------------------------------------------------------------------------