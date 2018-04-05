#pragma once

#pragma warning ( disable : 4290 )

//  factory.h
//
//  (C) Copyright 2014 Marcin Kudzia
//

#include <map>
#include <string>
#include <functional>

//--------------------------------------------------------------------------------

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

//--------------------------------------------------------------------------------

namespace asws              {
namespace common            {
namespace pattern           {

//--------------------------------------------------------------------------------

/*
	Factory template
*/
template <typename T, typename TypeId, typename Params>
class factory_ex
{
public:

	// Cleanup the map
	virtual ~factory_ex() { _funcs.clear(); }

	// Types
	typedef Params  params_type;

	typedef T		value_type;
	typedef T*		value_ptr;
	typedef TypeId	identifier_type;

	typedef std::function<T*(Params)> factory_func;
	/*
		Add new creation method

		@TODO: T_DELIVERED shoulde delivered from T and have method - type() which 
		returns std::string, in C++0x use concepts
	*/
	void register_type(identifier_type id, factory_func func) 
		throw (std::exception)
	{
		boost::mutex::scoped_lock lock(_mutex);
		
		auto item = _funcs.find(id);
		if (item != _funcs.end())
			throw std::runtime_error("Func with given id already exists in the function map");

		auto res = _funcs.insert(std::make_pair(id, func));
		if (!res.second)
			throw std::runtime_error("failed to add the function onto the map");
	}

	// Create instance of the given type
	value_ptr instantiate(identifier_type id, const params_type& params)
		throw (std::exception)
	{
		boost::mutex::scoped_lock lock(_mutex);

		auto item = _funcs.find(id);
		if (item == _funcs.end())
			throw std::runtime_error("No func with given id found in the factory");

		return item->second(params);
	}

private:

	std::map<identifier_type, factory_func> _funcs;  // map of methods

	boost::mutex _mutex;                               // lock on the map
}; // class factory

//--------------------------------------------------------------------------------

template <typename T, typename TypeId>
class factory
{
public:

	// Cleanup the map
	virtual ~factory() { _funcs.clear(); }

	// Types
	typedef T		value_type;
	typedef T*		value_ptr;
	typedef TypeId	identifier_type;

	typedef std::function<T*(void)> factory_func;

	/*
		Add new creation method

		@TODO: T_DELIVERED shoulde delivered from T and have method - type() which 
		returns std::string, in C++0x use concepts
	*/
	void register_type(identifier_type id, factory_func func) 
		throw (std::exception)
	{
		boost::mutex::scoped_lock lock(_mutex);
		
		auto item = _funcs.find(id);
		if (item != _funcs.end())
			throw std::runtime_error("Func with given id already exists in the function map");

		auto res = _funcs.insert(std::make_pair(id, func));
		if (!res.second)
			throw std::runtime_error("failed to add the function onto the map");
	}

	/* Register default constructor */
	template <TypeId id, typename TYPE>
	void register_type_use_constructor()
	{
		register_type(id, [](void) -> factory::value_ptr
		{
			return new TYPE();
		});
	}

	// Create instance of the given type
	value_ptr instantiate(identifier_type id)
		throw (std::exception)
	{
		boost::mutex::scoped_lock lock(_mutex);

		auto item = _funcs.find(id);
		if (item == _funcs.end())
			throw std::runtime_error("No func with given id found in the factory");

		return item->second();
	}

private:

	std::map<identifier_type, factory_func> _funcs;	   // map of methods

	boost::mutex _mutex;                               // lock on the map
}; // class factory
//--------------------------------------------------------------------------------

} // namespace pattern
} // namespace common
} // namespace asws

//--------------------------------------------------------------------------------
