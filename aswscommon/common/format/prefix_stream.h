#pragma once

// prefix_stream.h
//
//  (C) Copyright 2014 Marcin Kudzia
//

#include <iostream>
#include <sstream>
#include <deque>

//--------------------------------------------------------------------------------

#include <boost/thread/mutex.hpp>

//--------------------------------------------------------------------------------

#include "../pattern/singleton_ts.h"

//--------------------------------------------------------------------------------

namespace asws              {
namespace common            {
namespace format            {

//--------------------------------------------------------------------------------

namespace detail            {

//--------------------------------------------------------------------------------

/* Prefix stack
 Stores the prefixes in a stack

 Is a thread specific singleton - one instance of prefix stck per thread
*/
class prefix_stack
	: public pattern::singleton_ts<prefix_stack>
{
	friend class pattern::singleton_ts<prefix_stack>;
private:

	/* Constructor
	 Only by singleton
	*/
	prefix_stack() 
		: _rebuild(true)
	{ }

public:

	/* Add prefix to the stack */
	void push(const std::string& prefix)
	{
		_prefixes.push_back(prefix);
		_rebuild = true;
	}

	/* Remove prefix */
	void pop()
	{
		_prefixes.pop_back();
		_rebuild = true;
	}

	/* Return current prefix */
	const std::string& prefix() const 
	{
		if (_rebuild) 
			accumulate();
		return _prefix;
	}

private:

	/* Rebuild the prefix */
	void accumulate() const
	{
		if (_prefixes.empty())
			_prefix = "[root]";
		else {
			std::ostringstream prefix;
			std::copy(_prefixes.begin(), _prefixes.end(), std::ostream_iterator<std::string>(prefix));
			_prefix = prefix.str();
		}
		
		_rebuild = false;
	}

	// Stack of prefixes
	std::deque<std::string> _prefixes;

	// accumulated prefix
	mutable std::string _prefix;

	// identify if the prefix has to be rebuilt
	mutable bool _rebuild;
}; // class prefix_stack

//--------------------------------------------------------------------------------

} // namespace detail

//--------------------------------------------------------------------------------

/* Scoped prefix
 Pushes and pops the stream prefix begining/end of the scope

 Warning:
 Make sure the scoped_prefix is not mixed together
 with manual detail::prefix_stack::instance()->push/pop calls or macros
 as this will mess up the order of push/pops
*/
class scoped_prefix
{
public:

	/* Constructor
	 Push the prefix on creation
	*/
	scoped_prefix(const std::string& prefix)
	{
		detail::prefix_stack::instance()->push(prefix);
	}

	/* Pop the prefix 
	 From the scope
	*/
	~scoped_prefix()
	{
		detail::prefix_stack::instance()->pop();
	}
}; // class scoped_prefix

//--------------------------------------------------------------------------------

/* Push the prefix to the namespace */
inline void prefix_push(const std::string& prefix)
{
	detail::prefix_stack::instance()->push(prefix);	
}

/* Pop the prefix from the stack */
inline void prefix_pop()
{
	detail::prefix_stack::instance()->pop();
}

//--------------------------------------------------------------------------------

/*
	Stream with a prefix
*/
class prefix_stream
	: public std::ostream
{
private:

	/* Prefix buffer
	 Uses stacked prefixes when flushing, uses a single mutex to guarantee 
	 thread safety
	*/
	class prefix_buffer
		: public std::stringbuf
	{
	public:

		prefix_buffer(std::ostream& stream)
			: _stream(stream)
		{ }

		// Override the sync - add 
		virtual int sync()
		{
			// Guarantee synchronous execution here 
			// we will be using one/two different underlying streams here
			// so potential hit is ok
			boost::mutex::scoped_lock lock(_mutex);

			// Get the prefix from current stream and flush
			_stream << detail::prefix_stack::instance()->prefix() << " " << str();
			std::stringbuf::str("");
			_stream.flush();
			return 0;
		}

	private:

		// Underlying stream buffer
		std::ostream&	_stream;

		// Lock - a bit dirty but does the job
		static boost::mutex _mutex;
	}; // class stream_buffer

public:

	/* Constructor */
	explicit prefix_stream(std::ostream& stream)
		: std::ostream(&_buf), _buf(stream)
	{ }

private:

	// Underlying buffer
	prefix_buffer _buf;	
}; // class prefix_stream

//--------------------------------------------------------------------------------

} // namespace format
} // namespace common
} // namespace asws

//--------------------------------------------------------------------------------
