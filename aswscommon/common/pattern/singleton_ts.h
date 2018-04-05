#pragma once

// singleton_ts.h
//
//  (C) Copyright 2014 Marcin Kudzia
//

#include <boost/thread/tss.hpp>

//--------------------------------------------------------------------------------

namespace asws              {
namespace common            {
namespace pattern           {

//--------------------------------------------------------------------------------

/*
	Thread Specific Singleton
	Marshal single instance of the object per thread
*/
template <typename T>
class singleton_ts
{
public:

	/* Get the instance
	 Requires the object to have default constructor

	 No need for locking as this guarantees one per thread
	*/
	static T* instance()
	{
		T* inst = _instance.get();

		// No need for locking as it's one p
		// Value not present - check again but with lock
		if (!inst) {
			inst = new T;
			_instance.reset(inst);
		}

		return inst;
	}

	/* Release the instance */
	static void release()
	{
		T* inst = _instance.release();
		
		if (!inst)
			delete inst;
	}

private:

	static boost::thread_specific_ptr<T> _instance;
}; // class singleton_ts

//--------------------------------------------------------------------------------

template <typename T>
boost::thread_specific_ptr<T> singleton_ts<T>::_instance;

//--------------------------------------------------------------------------------

} // namespace pattern
} // namespace common
} // namespace asws

//--------------------------------------------------------------------------------
