
// main.cpp
//
// Client of Advanced Symphony Workload Simulator (ASWS) formerly known as:
// Platform Independed Symphony Simulation Oriented Failure Framework (PISSOFF)
//  (C) Copyright 2014 Marcin Kudzia
//

#include <string>
#include <iostream>

//--------------------------------------------------------------------------------

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

//--------------------------------------------------------------------------------

#include "../aswscommon/common/format/prefix_stream.h"
#include "../aswscommon/common/random/generators.h"

#include "../aswscommon/client/batch/grid_client.h"

//--------------------------------------------------------------------------------

/*
	@PLATFORM Thank you Platform for this little snippet here
	But seriously, I can't believe you are such a bunch of muppets
	Did someone read C++ Anti-patterns and decided to apply them????
*/
#include <SoamSrc.cpp> 

//--------------------------------------------------------------------------------

using namespace asws::common;
using namespace asws;

//--------------------------------------------------------------------------------
enum enum_type {
	fuu = 1,
	bar = 2
};

int main(int argc, char** argv)
{
	using namespace boost;


		std::cout << "sizeof(short):" << sizeof(short) << std::endl;
		std::cout << "sizeof(unsigned int):" << sizeof(unsigned int) << std::endl;
		std::cout << "sizeof(unsigned long):" << sizeof(unsigned long) << std::endl;
		std::cout << "sizeof(enum_type):" << sizeof(enum_type) << std::endl;



	// Setup the argc/argv parameters
	program_options::options_description opts("Allowed options");

	opts.add_options()
		("help", "show help message")
		("test-script", program_options::value<std::string>(), "input test script")
	;

	program_options::variables_map vars;
	program_options::store(program_options::parse_command_line(argc, argv, opts), vars);

	// Show help
	if (vars.count("help")) {
		std::cout << opts << std::endl;
		return 1;
	}

	// Prep the logger
	format::scoped_prefix sp("[main]");
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	// Parse the xml
	if (vars.count("test-script")) {
		std::string fname = vars["test-script"].as<std::string>();

		if (!filesystem::exists(fname)) {
			pserr << "Specified test-script file " << fname << " doesn't exist." << std::endl;
			return 1;
		}

		// Parse the config
		pugi::xml_document xml_doc;
		pugi::xml_parse_result result = xml_doc.load_file(fname.c_str());
		
		if (!result) {
			pserr << "Parsing the XML " << fname << " failed with errors;" << std::endl;
			pserr << "  error description : " << result.description() << std::endl;
			pserr << "  error offset : " << result.offset << std::endl;

			return 1;
		}

		psout << "Test configuration loaded" << std::endl;

		// Initialize random generators
		asws::common::random::initialize_seed();

		// Create the client
		psout << "Creating Symphony(tm) client" << std::endl;

		client::batch::grid_client client;

		try {
			client.load_config(xml_doc);
		} catch (std::exception const &e) {
			pserr << "Failed to load the batch configuration : " << e.what() << std::endl;

			return 1;
		}
	
		// Run the workload
		try {
			client.run();
		} catch (std::exception const &e) {
			pserr << "Test execution failed : " << e.what() << std::endl;

			return 1;
		}

		return 0;
	} else {
		pserr << "test-script file not specified" << std::endl; 
		pserr << opts << std::endl;

		return 1;
	}
}

//--------------------------------------------------------------------------------
