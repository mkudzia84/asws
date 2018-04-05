#pragma once

// grid_client.h
//
//  (C) Copyright 2014 Marcin Kudzia
//

#include "worker_pool.h"

#include "interface_isession.h"
#include "interface_itask_queue.h"

#include "../symphony/soam_client.h"
#include "../symphony/interface_isession.h"

//--------------------------------------------------------------------------------

#include "../../../3rdparty/pugixml-1.4/src/pugixml.hpp"

//--------------------------------------------------------------------------------

namespace asws              {
namespace client            {
namespace batch             {

//--------------------------------------------------------------------------------

/* Performs submission of the workload of the session
 Provides a high level interface that loads the batch
 config from the xml tree and executes the workload
*/
class grid_client
{
public:

	/* Constructor 
	 Initialize the components */
	grid_client();

	/* Destructor
	 Clean up */
	~grid_client();

	/* Parse the config file; establish the connection
	 and initialize the components */
	void load_config(pugi::xml_document& config);
	 
	/* Execute the batch */
	void run();

private:

	/* Parse the root */
	void parse_xml(pugi::xml_node st_node);

private:

	// Connection settings
	std::string _app_name;
	std::string _username;
	std::string _password;

	// Soam client
	symphony::soam_client _sym_client;

	// Worker pool
	worker_pool _wpool;
}; // class grid_client

//--------------------------------------------------------------------------------

}	// namespace batch
}	// namespace client
}	// namespace asws

//--------------------------------------------------------------------------------
