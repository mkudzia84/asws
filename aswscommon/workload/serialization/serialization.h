#pragma once

// serialization.h
//
//  (C) Copyright 2014 Marcin Kudzia
//

#include <memory>

//--------------------------------------------------------------------------------

namespace asws              {
namespace workload          {
namespace serialization     {

//--------------------------------------------------------------------------------

/*
	Serialization stream helper
	Notifies archives to use the member function save/load
*/
template <typename T>
class use_member
{
public:
	explicit use_member(T *ptr)
		: _instance(ptr)
	{ }

	explicit use_member(T &ref)
		: _instance(&ref)
	{ }

	explicit use_member(std::shared_ptr<T> &sptr)
		: _instance(sptr.get())
	{ } 

	T* instance() const {
		return _instance;
	}
protected:
	mutable T* _instance;
};

//--------------------------------------------------------------------------------

/*
	Serialization level concept helper
	Stores information about serialization level
*/
class serialization_level
{
public:

	typedef enum {
		all				= 0,	// Serialize whole object
		request			= 1,	// Serialize the request part
		response		= 2,	// Serialize the response part
		state_process	= 3,	// Serialize the process level state (use in future)
		state_session	= 4		// Serialize the session level state (use in future)
	} level;

	// Constructor
	serialization_level(level l = all)
		: _level(l)
	{ }

	// Setter / getter
	void set_level(level l) 
	{ _level = l; }

	level get_level() const 
	{ return _level; }

private:

	level _level;
}; // class serialization_level

//--------------------------------------------------------------------------------

/*
	Serialization stream interface
*/
class istream_base
{
public:
	virtual ~istream_base()
	{ }

	// Load function
	virtual void read(void *buff, unsigned long size) = 0;

	// Save functions for build in types
	virtual void read(char& ch) = 0;
	virtual void read(short& sh) = 0;
	virtual void read(unsigned short& sh) = 0;
	virtual void read(int& i) = 0;
	virtual void read(unsigned int& i) = 0;
	virtual void read(long& l) = 0;
	virtual void read(unsigned long& l) = 0;
	virtual void read(float& f) = 0;
	virtual void read(double& d) = 0;

};

/*
	Deserialization stream interface
*/
class ostream_base
{
public:
	virtual ~ostream_base()
	{ }

	// Save function for buffer
	virtual void write(void *buff, unsigned long size) = 0;

	// Save functions for build in types
	virtual void write(char ch) = 0;
	virtual void write(short sh) = 0;
	virtual void write(unsigned short sh) = 0;
	virtual void write(int i) = 0;
	virtual void write(unsigned int i) = 0;
	virtual void write(long l) = 0;
	virtual void write(unsigned long l) = 0;
	virtual void write(float f) = 0;
	virtual void write(double d) = 0;
};

//--------------------------------------------------------------------------------

} // namespace serialization
} // namespace workload
} // namespace asws

//--------------------------------------------------------------------------------
