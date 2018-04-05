/*
	Implementation of the synchronous session
*/

#include <iostream>

//--------------------------------------------------------------------------------

#include "soam_client.h"
#include "sync_session.h"
#include "async_session.h"

//--------------------------------------------------------------------------------

#include "../../common/format/prefix_stream.h"

//--------------------------------------------------------------------------------

using namespace asws::common;
using namespace asws::client::symphony;

//-------------------------------------------------------------------------------

soam_client::soam_client()
{
	format::scoped_prefix sp("[soam-client]");
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	// Initialize types
	register_type(interface_isession::sync, [this]()->interface_isession* { return new sync_session(*this); });
	register_type(interface_isession::async, [this]()->interface_isession* { return new async_session(*this); });

	// Initialize soam and connection
	psout << "Initializing SOAM SDK" << std::endl;
	soam::SoamFactory::initialize();
}

//-------------------------------------------------------------------------------

soam_client::~soam_client()
{
	format::scoped_prefix sp("[soam-client]");
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	// Clean up the API
	soam::SoamFactory::uninitialize();

	psout << "SOAM SDK uninitialized" << std::endl;
}

//-------------------------------------------------------------------------------

void soam_client::connect(const std::string& app_name, const std::string& user_name, const std::string& user_passwd)
{
	format::scoped_prefix sp("[soam-client]");
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	// Connect to the grid
	psout << "Connecting to the grid : { app : " << app_name << ", user : " << user_name << " }" << std::endl;

	soam::DefaultSecurityCallback security_callback(user_name.c_str(), user_passwd.c_str());

	try {
		_conn = soam::SoamFactory::connect(app_name.c_str(), &security_callback);
	} catch (soam::SoamException& e) {
		pserr << "Failed to connect to the grid : " << e.what() << std::endl;

		throw std::runtime_error(e.what());
	}

	psout << "Succesfully connected to the grid, conn-id : " << _conn->getId() << std::endl;

}

//-------------------------------------------------------------------------------

void soam_client::close()
{
	format::scoped_prefix sp("[soam-client]");
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	// Close the connection
	psout << "Closing the connection" << std::endl;

	try {
		_conn->close();
	} catch (soam::SoamException& e) {
		pserr << "Failed to cleanly close the connection to the grid : " << e.what() << std::endl;

		throw std::runtime_error(e.what());
	}

	psout << "Connection to the grid terminated" << std::endl;
}

//-------------------------------------------------------------------------------
