/*
	Implementation of prefix stream
*/

#include "prefix_stream.h"

//--------------------------------------------------------------------------------

using namespace asws::common::format;

//--------------------------------------------------------------------------------

boost::mutex prefix_stream::prefix_buffer::_mutex;

//--------------------------------------------------------------------------------