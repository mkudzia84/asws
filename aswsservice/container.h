#pragma once

// container.h
//
// Container service for Advanced Symphony Workload Simulator (ASWS) formerly known as:
// Platform Independed Symphony Simulation Oriented Failure Framework (PISSOFF)
//
//  (C) Copyright 2014 Marcin Kudzia
//

#include <soam.h>

//--------------------------------------------------------------------------------

namespace asws         {
namespace service     {

//--------------------------------------------------------------------------------

/* 
	Container class
*/
class test_service
	: public soam::ServiceContainer
{
public:

	// Constructor
	test_service(int argc, char** argv);

	// Destructor
	virtual ~test_service();

	//----------------------------------------------------------------------------
	// Soam callbacks
	//----------------------------------------------------------------------------

	/*
		onCreateService
	*/
	virtual void onCreateService(soam::ServiceContextPtr& serviceCtx);

	/*
		onSessionEnter
	*/
	virtual void onSessionEnter(soam::SessionContextPtr& sessionCtx);

	/*
		onSessionUpdate
	*/
	virtual void onSessionUpdate(soam::SessionContextPtr& sessionCtx);

	/*
		onInvoke
	*/
	virtual void onInvoke(soam::TaskContextPtr& taskCtx);

	/*
		onSessionLeave
	*/
	virtual void onSessionLeave();

	/*
		onDestroyService
	*/
	virtual void onDestroyService();

private:

	soam::ServiceContextPtr _serviceCtx;		// Service context

}; // class service

//--------------------------------------------------------------------------------

} // namespace service
} // namespace asws

//--------------------------------------------------------------------------------
