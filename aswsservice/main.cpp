
// main.cpp
//
//  (C) Copyright 2014 Marcin Kudzia
//

#include <iostream>

//--------------------------------------------------------------------------------

#include "../aswscommon/common/format/prefix_stream.h"
#include "../aswscommon/common/random/generators.h"

//--------------------------------------------------------------------------------

#include "container.h"

//--------------------------------------------------------------------------------

/*
	@PLATFORM Thank you Platform for this little snippet here
	But seriously, I can't believe you are such a bunch of muppets
	Did someone read C++ Anti-patterns and decided to apply them????
*/
#include <SoamSrc.cpp> 

//--------------------------------------------------------------------------------

using namespace asws::common;

int main(int argc, char** argv)
{
	// Setup logger
	format::scoped_prefix sp("[main]");
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cout);
	
	try {
		psout << "Creating test service container" << std::endl;

		random::initialize_seed();

		asws::service::test_service service(argc, argv);
		service.run();
	} catch (soam::SoamException& e) {
		pserr << e.what() << std::endl;
		throw e;
	} catch (std::exception& e) {
		pserr << e.what() << std::endl;
		throw soam::FailureException(e.what());
	} catch (...) {
		pserr << "Unhandled exception..." << std::endl;
		throw soam::FailureException("Unahndled exception");
	}

	return 0;
}

//--------------------------------------------------------------------------------
