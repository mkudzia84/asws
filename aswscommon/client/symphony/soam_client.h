#pragma once

// soam_client.h
//
//  (C) Copyright 2014 Marcin Kudzia
//

#include <memory>

//--------------------------------------------------------------------------------

#include <soam.h>

//--------------------------------------------------------------------------------

#include "interface_isession.h"
#include "../../common/pattern/factory.h"

//--------------------------------------------------------------------------------

namespace asws              {
namespace client            {
namespace symphony          {

//--------------------------------------------------------------------------------

/* 
	Client class
*/
class soam_client
	: private asws::common::pattern::factory<interface_isession, interface_isession::type>
{
public:

	/* Constructor 
	 Initializes the API
	*/
	soam_client();

	/* Destructor 
	 Cleans up SOAM state
	*/
	~soam_client();

	/* Connect to the grid */
	void connect(const std::string& app_name, const std::string& user_name, const std::string& user_passwd);

	/* Disconnect from the grid */
	void close();

	/* Create a session */
	isession_ptr create_session(interface_isession::type type)
	{
		return isession_ptr(instantiate(type));
	}

	/* Get soam connection */
	soam::ConnectionPtr get_soam_connection() { return _conn; }

private:

	// SOAM connection
	soam::ConnectionPtr _conn;
}; // class soam_client

//--------------------------------------------------------------------------------

}	// namespace symphony
}	// namespace client
}	// namespace asws

