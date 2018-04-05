#pragma once

// workload_factory.h
//
//  (C) Copyright 2014 Marcin Kudzia
//

#include "interface_iworkload.h"

#include "../../common/pattern/factory.h"
#include "../../common/pattern/singleton.h"

//--------------------------------------------------------------------------------

namespace asws           {
namespace workload       {

//--------------------------------------------------------------------------------

using namespace asws::common;

//--------------------------------------------------------------------------------

/*
	Factory
*/
class factory
	: public pattern::factory<interface_iworkload, interface_iworkload::type>, public pattern::singleton<factory>
{
	friend class pattern::singleton<factory>;
private:

	/* Constructor */
	factory() 
	{ }

public:

	/* Destructor */
	virtual ~factory() 
	{ }

}; // class factory

//--------------------------------------------------------------------------------

/*
	Factory with parameters
*/
class factory_ex
	: public pattern::factory_ex<interface_iworkload, std::string, std::map<std::string, std::string>>, public pattern::singleton<factory_ex>
{
	friend class pattern::singleton<factory_ex>;
private:

	/* Constructor */
	factory_ex()
	{ }

public:

	/* Destructor */
	virtual ~factory_ex()
	{ }

}; // class factory_ex

//--------------------------------------------------------------------------------

} // namespace workload
} // namespace asws

//--------------------------------------------------------------------------------
