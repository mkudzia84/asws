#pragma once

// session_flags.h
//
//  (C) Copyright 2014 Marcin Kudzia
//

//--------------------------------------------------------------------------------

namespace asws              {
namespace client            {
namespace symphony          {

//--------------------------------------------------------------------------------

class session_flags
{
public:
	
	// Attibute flags
	typedef enum {
		none            = 0x00,
		send_overlapped = 0x01
	} flag;

	/* Check if flag is set */
	static bool is_set(int flags, flag f)
	{
		return (flags & f) == f;
	}

}; // class session flags

//--------------------------------------------------------------------------------

}	// namespace symphony
}	// namespace client
}	// namespace asws

//--------------------------------------------------------------------------------
