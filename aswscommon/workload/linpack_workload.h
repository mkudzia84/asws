#pragma once

// linpack_workload.h
//
//  (C) Copyright 2017 Marcin Kudzia
//

//--------------------------------------------------------------------------------

#pragma warning(disable : 4309)

//--------------------------------------------------------------------------------

#include <vector>
#include <memory>

//--------------------------------------------------------------------------------

#include "interface_iworkload.h"

//--------------------------------------------------------------------------------

namespace asws             {
namespace workload         {

//--------------------------------------------------------------------------------

/*
	Linpack workload
*/
class linpack_workload
	: public interface_iworkload
{
public:

	/* Constructor */
	linpack_workload(int nreps = 0, int arsize = 0)
		: interface_iworkload(interface_iworkload::linpack_benchmark),
		_nreps(nreps), _arsize(arsize)
	{}

	/* Destructor */
	virtual ~linpack_workload() 
	{ }

	/* Execute the workload call */
	virtual void execute();

	/* Generate the report */
	std::string report();

	/* Serialization */
	virtual void save(serialization::binary_oarchive &oar)
	{
		interface_iworkload::save(oar);

		// The params
		oar << (int)_nreps << (int)_arsize;
		
		auto level = oar.get_level();
		// If writing response - also build an byte array
		if (level == serialization::serialization_level::all || 
			level == serialization::serialization_level::response) {
			oar << _totalt << _dgefa << _dgesl << _overhead << _kflops;
		}
	}

	virtual void load(serialization::binary_iarchive &iar)
	{
		interface_iworkload::load(iar);

		// The params
		iar >> (int&)_nreps >> (int&)_arsize;

		auto level = iar.get_level();
		// If reading a respone - also read the byte array
		if (level == serialization::serialization_level::all ||
			level == serialization::serialization_level::response) {
			iar >> _totalt >> _dgefa >> _dgesl >> _overhead >> _kflops;
		}
	}

private:

	int _nreps;						// Number of repetitions
	int _arsize;					// Matrix size
	
	// Collected metrics
	double _totalt;
	double _dgefa;
	double _dgesl;
	double _overhead;
	double _kflops;

}; // class linpack_workload

//--------------------------------------------------------------------------------

} // namespace workload
} // namespace asws

//--------------------------------------------------------------------------------
