#pragma once

// singleton.h
//
//  (C) Copyright 2014 Marcin Kudzia
//

#include <boost/atomic.hpp>						// @TODO: Replace by C++0x
#include <boost/thread/mutex.hpp>				// @TODO: Replace by C++0x

//--------------------------------------------------------------------------------

namespace asws              {
namespace common            {
namespace pattern           {

//--------------------------------------------------------------------------------

/*
	Singleton
	Marshal single global instance of the object
*/
template <typename T>
class singleton
{
public:

	/* Get the instance
	 Requires the object to have default constructor
	*/
	static T* instance()
	{
		T* inst = _instance.load(boost::memory_order_consume);

		// Value not present - check again but with lock
		if (!inst) {
			boost::mutex::scoped_lock lock(_mutex);
			inst = _instance.load(boost::memory_order_consume);

			if (!inst) {
				inst = new T;
				_instance.store(inst, boost::memory_order_release);
			}
		}

		return inst;
	}

	/* Release the instance */
	static void release() 
	{
		boost::mutex::scoped_lock lock(_mutex);

		T* inst = _instance.load(boost::memory_order_consume);

		if (inst) {
			delete inst;
			_instance.store(nullptr, boost::memory_order_release);
		}
	}

private:

	static boost::atomic<T*> _instance;
	static boost::mutex _mutex;
}; // class Singleton

//--------------------------------------------------------------------------------

template <typename T>
boost::atomic<T*> singleton<T>::_instance(nullptr);

template <typename T>
boost::mutex singleton<T>::_mutex;

//--------------------------------------------------------------------------------

} // namespace pattern
} // namespace common
} // namespace asws

//--------------------------------------------------------------------------------
