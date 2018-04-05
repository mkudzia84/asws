/*
	Implementation of the Linpack workload
*/

#include <boost/thread.hpp>				/* @TODO: Replace <boost/thread.hpp> with <thread> from C++0x */
#include <boost/chrono.hpp>				/* @TODO: Replace <boost/chrono.hpp> with <chrono> from C++0x */

#include "linpack_workload.h"
#include <iostream>

/* 
	Linpack code
*/
extern "C" {
	void linpack_init(int arsize);
	void linpack_free();
	double linpack(long nreps, int arsize, double *totalt, double *dgefa, double *dgesl, double *overhead, double *kflops);
};

//--------------------------------------------------------------------------------

using namespace asws::workload;

//--------------------------------------------------------------------------------

void linpack_workload::execute()
{
	linpack_init(_arsize);
	linpack(_nreps, _arsize, &_totalt, &_dgefa, &_dgesl, &_overhead, &_kflops);
	linpack_free();
}

//--------------------------------------------------------------------------------

std::string linpack_workload::report()
{
	std::stringstream sout;
	sout << "nreps: " << _nreps << ", ar-size: " << _arsize << ", mflops: " << (_kflops / 1000.) << ", total-time[s]:" << _totalt
		<< ", DGEFA: " << (100.*_dgefa / _totalt) << ", DGESL:" 
		<< std::fixed << (100.*_dgesl / _totalt) << ", OVERHEAD: " << (100.*_overhead / _totalt);
	return sout.str();
}

//--------------------------------------------------------------------------------
