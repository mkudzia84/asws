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
	SOAM input stream
*/
class soam_istream
	: public istream_base
{
public:

	/* Constructor */
	soam_istream(soam::InputStreamPtr istream)
		: _stream(istream)
	{ }

	virtual ~soam_istream()
	{ }

	// Read from stream
	virtual void read(void *buff, unsigned long size) { _stream->readBytes(buff, size); }

	// Save functions for build in types
	virtual void read(char& ch)				{ _stream->read(ch); }
	virtual void read(short& sh)			{ _stream->read(sh); }
	virtual void read(unsigned short& sh)	{ _stream->read(sh); }
	virtual void read(int& i)				{ _stream->read(i); }
	virtual void read(unsigned int& i)		{ _stream->read(i); }
	virtual void read(long& l)				{ _stream->read(l); }
	virtual void read(unsigned long& l)		{ _stream->read(l); }
	virtual void read(float& f)				{ _stream->read(f); }
	virtual void read(double& d)			{ _stream->read(d); }

private:

	soam::InputStreamPtr _stream;
}; // class soam_istream

//--------------------------------------------------------------------------------

} // namespace symphony
} // namespace workload
} // namespace asws

//--------------------------------------------------------------------------------