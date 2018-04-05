#pragma once

// soam_istream.h
//
//  (C) Copyright 2014 Marcin Kudzia
//

#include "serialization.h"

//--------------------------------------------------------------------------------

#include <soam.h>

//--------------------------------------------------------------------------------

namespace asws           {
namespace workload       {
namespace serialization  {

//--------------------------------------------------------------------------------

/*
	SOAM output stream
*/
class soam_ostream
	: public ostream_base
{
public:

	/* Constructor */
	soam_ostream(soam::OutputStreamPtr stream)
		: _stream(stream)
	{ }

	virtual ~soam_ostream()
	{ }

	// Read from stream
	virtual void write(void *buff, unsigned long size) { _stream->writeBytes(buff, size); }

	// Save functions for build in types
	virtual void write(char ch)				{ _stream->write(ch); }
	virtual void write(short sh)			{ _stream->write(sh); }
	virtual void write(unsigned short sh)	{ _stream->write(sh); }
	virtual void write(int i)				{ _stream->write(i); }
	virtual void write(unsigned int i)		{ _stream->write(i); }
	virtual void write(long l)				{ _stream->write(l); }
	virtual void write(unsigned long l)		{ _stream->write(l); }
	virtual void write(float f)				{ _stream->write(f); }
	virtual void write(double d)			{ _stream->write(d); }


private:

	soam::OutputStreamPtr _stream;
}; // class soam_ostream

//--------------------------------------------------------------------------------

} // namespace symphony
} // namespace workload
} // namespace asws

//--------------------------------------------------------------------------------