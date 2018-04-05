#pragma once

#pragma warning ( disable : 4290 )

#include <soam.h>

//  wrapper.h
//
//  (C) Copyright 2017 Marcin Kudzia
//

//--------------------------------------------------------------------------------

namespace uc                {
namespace common            {
namespace wrapper           {

//--------------------------------------------------------------------------------

/*
	Wrapper template for soam objects
	Used as base class for all wrappers to eliminate boiler plate
	Rellies on fact all soam::ObjectPtr objects
	are actually soam::SoamSmartPtr<Object> typedefs
*/
template <typename T>
class wrapper_soam {
public:

	// Constructor
	wrapper_soam(soam::SoamSmartPtr<T> instance)
		: _object(instance)
	{}

	// Overload operator -> to access internal one
	// in C++ operators overloading is inherited
	soam::SoamSmartPtr<T> operator->() const
	{
		return _object;
	}

	// Explicit get internal
	soam::SoamSmartPtr<T> get_ptr() const
	{
		return _object;
	}

	// Casts 
	// Will be inherited as well
	operator soam::SoamSmartPtr<T>() const
	{
		return get_ptr();
	}

private:
	soam::SoamSmartPtr<T> _object;
};

//--------------------------------------------------------------------------------
} // namespace wrapper
} // namespace common
} // namespace uc