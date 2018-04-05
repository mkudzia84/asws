#pragma once

// idle_workload.h
//
//  (C) Copyright 2014 Marcin Kudzia
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
	Idle workload
*/
class idle_workload
	: public interface_iworkload
{
public:

	/* Constructor */
	idle_workload(int rt = 0, int inp_size = 0, int out_size = 0)
		: interface_iworkload(interface_iworkload::idle),
		input_size(inp_size), output_size(out_size)
	{
		runtime = rt;
	}

	/* Destructor */
	virtual ~idle_workload() 
	{ }

	/* Execute the workload call */
	virtual void execute();

	/* Provide report */
	virtual std::string report();

	/* Serialization */
	virtual void save(serialization::binary_oarchive &oar)
	{
		interface_iworkload::save(oar);

		// Serialize the ranges
		oar << (int)runtime << (int)input_size << (int)output_size;
		auto level = oar.get_level();

		// If writing request - also build an byte array
		if (level == serialization::serialization_level::all || 
			level == serialization::serialization_level::request) {
			std::vector<char> payload(input_size);
			for (int i = 0; i < input_size; i++)
				payload[i] = 0xff;
			oar << payload;
		}

		// If writing response - also build an byte array
		if (level == serialization::serialization_level::all || 
			level == serialization::serialization_level::response) {
			std::vector<char> payload(output_size);
			for (int i = 0; i < output_size; i++)
				payload[i] = 0xab;
			oar << payload;
		}
	}

	virtual void load(serialization::binary_iarchive &iar)
	{
		interface_iworkload::load(iar);

		// Deserialize the ranges
		iar >> (int&)runtime >> (int&)input_size >> (int&)output_size;
		auto level = iar.get_level();

		// If reading a request read the byte array
		if (level == serialization::serialization_level::all ||
			level == serialization::serialization_level::request) {
			std::vector<char> payload;
			iar >> payload;
		}

		// If reading a respone - also read the byte array
		if (level == serialization::serialization_level::all ||
			level == serialization::serialization_level::response) {
			std::vector<char> payload;
			iar >> payload;
		}
	}

private:

	int runtime;                    // Task runtime

	int input_size;					// Input data size
	int output_size;				// Output data size
}; // class IdleWorkload


//--------------------------------------------------------------------------------

} // namespace workload
} // namespace asws

//--------------------------------------------------------------------------------
