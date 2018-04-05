#pragma once

// iworkload.h
//
//  (C) Copyright 2014 Marcin Kudzia
//

//--------------------------------------------------------------------------------

#include <memory>
#include <string>

//--------------------------------------------------------------------------------

#include "serialization/binary_archive.h"
//--------------------------------------------------------------------------------

namespace asws          {
namespace workload      {

//--------------------------------------------------------------------------------

/*
	Workload interface
	Uses boost serialization
*/
class interface_iworkload
{
public:

	typedef enum {
		none = 0,
		idle = 1,
		busy = 2,
		common_data = 3,
		linpack_benchmark = 4,
	} type; // enum type

	/* Destructor */
	virtual ~interface_iworkload() = 0;

	/* Get the type */
	type get_type() const { return _workload_type; }

	/* Execute the workload call */
	virtual void execute() = 0;

	/* Return the report */
	virtual std::string report() = 0;

	/* Serialization */
	virtual void save(serialization::binary_oarchive& oar)
	{
		oar << (int)_workload_type;
	}

	virtual void load(serialization::binary_iarchive& iar)
	{
		iar >> (int&)_workload_type;
	}

protected:

	/* Initializer */
	interface_iworkload(type t)
		: _workload_type(t) 
	{ }

private:

	// Workload type
	type _workload_type;
}; // class iworkload

//--------------------------------------------------------------------------------

typedef std::shared_ptr<interface_iworkload> iworkload_ptr;

//--------------------------------------------------------------------------------

inline interface_iworkload::~interface_iworkload() {}

//--------------------------------------------------------------------------------

} // namespace workload
} // namespace asws

//--------------------------------------------------------------------------------
