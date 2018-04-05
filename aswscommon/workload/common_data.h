#pragma once

// common_data.h
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

#include "serialization/serialization.h"

//--------------------------------------------------------------------------------

namespace asws             {
namespace workload         {

//--------------------------------------------------------------------------------

/*
	Common data
*/
class common_data
	: public interface_iworkload
{
public:

	/* Constructor */
	common_data(int size = 0)
		: interface_iworkload(interface_iworkload::common_data)
	{
		_size = size;
	}

	/* Destructor */
	virtual ~common_data()
	{ }

	/* Execute the workload call */
	virtual void execute() {}

	/* Generate report - null */
	virtual std::string report() {
		return std::string();
	}

	/* Serialization */
	virtual void save(serialization::binary_oarchive &oar)
	{
		interface_iworkload::save(oar);

		std::vector<char> payload(_size);
		for (int i = 0; i < _size; i++)
			payload[i] = 0xCC;
		oar << payload;
	}

	virtual void load(serialization::binary_iarchive &iar)
	{
		interface_iworkload::load(iar);
		std::vector<char> payload(_size);
		iar >> payload;
	}

private:

	int _size;						// Input data size
}; // class common_data

//--------------------------------------------------------------------------------

} // namespace workload
} // namespace asws

//--------------------------------------------------------------------------------