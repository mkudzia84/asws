#pragma once

// interface_isession.h
//
//  (C) Copyright 2014 Marcin Kudzia
//

//--------------------------------------------------------------------------------

namespace asws              {
namespace client            {
namespace batch             {

//--------------------------------------------------------------------------------

/* interface_isession
 Used to execute the session workload
*/

class interface_isession
{
public:

	/* Destructor */
	virtual ~interface_isession()
	{ }

	/* Run the session */
	virtual void run() = 0;

}; // class interface_iaction

//--------------------------------------------------------------------------------

}	// namespace batch
}	// namespace client
}	// namespace asws
