#pragma once

// session_attributes.h
//
//  (C) Copyright 2014 Marcin Kudzia
//

//--------------------------------------------------------------------------------

#include <memory>

//--------------------------------------------------------------------------------

#include "../../workload/interface_iworkload.h"

//--------------------------------------------------------------------------------

namespace asws              {
namespace client            {
namespace symphony          {

//--------------------------------------------------------------------------------

/* 
	Session attibutes
*/
class session_attributes
{
public:

	/* Constructor */
	session_attributes(const std::string& session_name, const std::string& session_type, int session_flags, int cdata_size = 0)
		: name(session_name), type(session_type), flags(session_flags), common_data_size(cdata_size)
	{ }

	// Settings
	const std::string name;
	const std::string type;
	const int flags;

	// Common data
	const int common_data_size;
}; // session_attributes

//--------------------------------------------------------------------------------

}	// namespace symphony
}	// namespace client
}	// namespace asws

//--------------------------------------------------------------------------------

