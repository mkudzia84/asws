#pragma once

#pragma warning ( disable : 4290 )

#include "wrapper.h"
#include <iostream>

//  connection.h
//
//  (C) Copyright 2017 Marcin Kudzia
//

//--------------------------------------------------------------------------------

namespace uc                {
namespace common            {
namespace wrapper           {

//--------------------------------------------------------------------------------

/*
	Wrapper arround connection object
	Casts back to soam::Connection using the wrapper_soam functionality
*/
class Connection
	: public wrapper_soam<soam::Connection>
{
public:
	// Construct
	Connection(soam::ConnectionPtr conn)
		: wrapper_soam<soam::Connection>(conn)
	{}

	soam::Session* createSession(
		soam::SessionCreationAttributes& attributes) throw (soam::SoamException)
	{
		std::cout << "Creating session" << std::endl;
		soam::Session* session = get_ptr()->createSession(attributes);
		std::cout << "Session with ID " << session->getId() << " created" << std::endl;
		return session;
	}

	/* Usage
		uc::common::wrapper::Connection conn(SoamFactory::connect());
		conn->createSession(...) - this will call on the internal soam::Connection object
		conn.createSession(...) - will call on the decorated one

		Can pass uc::commmon::wrapper::Connection to any function that expects ConnectionPtr - it will just cast 
		and pass the internal object (loosing the wrapper functionality)
		Not the most flexible 

		to do it properly, but extra maintenance FAF.....
		look at soam.h - and all the stuff there

		SoamConnection.h
		SoamConnection.cpp
		etc.

		You will it's actually see it's a think wrapper over an internal API that
		maps objects 1-1 to some internal libsoam ones with adding some extra error handling
		Because the code is available (and is always included into the source code)
		And has no regard for whats the idiomatic way of doing thins in c++

		We could basically fork it off, add all the monitoring/debugging/logging functionality
		and provide like for like replacement.
		so the users instead of including <soam.h> and <SoamSrc.cpp> would inlcude <ucSoam.h> <ucSoamSrc.cpp> with our fork
		that interfaces directly with the internal API (as their layer does) and adds extra bells and whistles

		If we would do it this way; honestly I would want to do it in my freetime and whack it on github....
	*/
};

//--------------------------------------------------------------------------------
} // namespace wrapper
} // namespace common
} // namespace uc