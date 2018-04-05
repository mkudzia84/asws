
// container.cpp
//
// Container service for Advanced Symphony Workload Simulator (ASWS) formerly known as:
// Platform Independed Symphony Simulation Oriented Failure Framework (PISSOFF)
//  (C) Copyright 2014 Marcin Kudzia
//

#include <iostream>
#include <functional>

//--------------------------------------------------------------------------------

#include <boost/lambda/construct.hpp>

//--------------------------------------------------------------------------------

#include "../aswscommon/common/format/prefix_stream.h"
#include "../aswscommon/workload/symphony/message.h"

//--------------------------------------------------------------------------------
// Workload types
//--------------------------------------------------------------------------------

#include "../aswscommon/workload/interface_iworkload.h"
#include "../aswscommon/workload/factory.h"
#include "../aswscommon/workload/idle_workload.h"
#include "../aswscommon/workload/linpack_workload.h"
#include "../aswscommon/workload/common_data.h"

//--------------------------------------------------------------------------------

#include "container.h"

//--------------------------------------------------------------------------------

using namespace asws::service;
using namespace asws::common;

//--------------------------------------------------------------------------------

test_service::test_service(int argc, char** argv)
{
	format::prefix_push("[service]");
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	// Initialize the workload factory
	psout << "Initializing workload factory object" << std::endl;

	try {
		using namespace boost::lambda;
		auto factory = workload::factory::instance();
		factory->register_type(workload::interface_iworkload::idle, new_ptr<workload::idle_workload>());
		factory->register_type(workload::interface_iworkload::linpack_benchmark, new_ptr<workload::linpack_workload>());
		factory->register_type(workload::interface_iworkload::common_data, new_ptr<workload::common_data>());
	} catch (std::exception e) {
		pserr << "Error : "<< e.what() << std::endl;
		throw e;
	}
}

//--------------------------------------------------------------------------------

test_service::~test_service()
{ 
	format::prefix_pop();
}

//--------------------------------------------------------------------------------

void test_service::onCreateService(soam::ServiceContextPtr& serviceCtx)
{
	format::scoped_prefix sp("[create]");
	format::prefix_stream psout(std::cout);
		
	psout << "Registering workload types" << std::endl;
	psout << "SI created" << std::endl;
	_serviceCtx = serviceCtx;
}

//--------------------------------------------------------------------------------

void test_service::onSessionEnter(soam::SessionContextPtr& sessionCtx)
{
	// Prepare 
	std::stringstream prefix;
	prefix << "[session:" << sessionCtx->getSessionId() << "]";
	format::prefix_push(prefix.str());
	format::scoped_prefix sp("[enter]");
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	psout << "Binding the session to service instance" << std::endl;
	
	// Payload
	using namespace asws::workload;
	iworkload_ptr payload = nullptr;

	// Read the message
	try {
		symphony::message<symphony::message_type::input> msg;

		psout << "Deserializing the message" << std::endl;
		sessionCtx->populateCommonData(msg);

		payload = msg.payload();
	} catch(soam::SoamException& e) {
		pserr << "Error deserializing the message : " << e.what() << std::endl;
		throw e;
	}

	// Execute the payload
	try {
		psout << "Processing the payload" << std::endl;
		payload->execute();
	} catch(std::exception const &e) {
		pserr << "Error executing the payload : " << e.what() << std::endl;
		throw soam::FailureException(e.what());
	}

	format::prefix_stream(std::cout) << "Finished binding the session" << std::endl;
}

//--------------------------------------------------------------------------------

void test_service::onSessionUpdate(soam::SessionContextPtr& sessionCtx)
{
	// Prepare
	format::scoped_prefix sp("[update]");
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	if (sessionCtx->isUpdated(soam::SessionContext::CommonData)) {
		psout << "Executing a common data update" << std::endl;

		// payload
		using namespace asws::workload;
		iworkload_ptr payload = nullptr;

		// Read the message
		try {
			symphony::message<symphony::message_type::input> msg;
			
			format::prefix_stream(std::cout) << "Deserializing the message" << std::endl;
			sessionCtx->populateCommonData(msg);

			payload = msg.payload();
		} catch(soam::SoamException& e) {
			pserr << "Error deserializing the message : " << e.what() << std::endl;
			throw e;
		}

		// Execute the payload
		try {
			psout << "Processing the payload" << std::endl;
			payload->execute();
		} catch(std::exception const &e) {
			pserr << "Error executing the payload : " << e.what() << std::endl;
			throw soam::FailureException(e.what());
		}

		psout << "Finished executing the update" << std::endl;
	}
}

//--------------------------------------------------------------------------------

void test_service::onInvoke(soam::TaskContextPtr& taskCtx)
{
	// Prepare

	std::stringstream prefix;
	prefix << "[task:" << taskCtx->getTaskId() << "][invoke]";
	format::scoped_prefix sp(prefix.str());
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	psout << "Executing the invoke for task " << taskCtx->getTaskId() << std::endl;

	// payload
	using namespace asws::workload;
	iworkload_ptr payload = nullptr;

	// Read the message
	try {
		symphony::message<symphony::message_type::input> msg;

		psout << "Deserializing the message" << std::endl;
		taskCtx->populateTaskInput(msg);
		psout << "Task payload deserialized, extracting the payload" << std::endl;
		payload = msg.payload();
	} catch(soam::SoamException& e) {
		pserr << "Error deserializing the message : " << e.what() << std::endl;
		throw e;
	}

	// Execute the payload
	try {
		psout << "Processing the payload" << std::endl;
		payload->execute();
		psout << "Processing of the payload finished" << std::endl;
	} catch(std::exception const &e) {
		pserr << "Error executing the payload : " << e.what() << std::endl;
		throw soam::FailureException(e.what());
	}

	// Write the output message
	try {
		symphony::message<symphony::message_type::output> msg(payload);

		psout << "Serializing the message" << std::endl;
		taskCtx->setOutputMessage(msg);
	} catch(soam::SoamException& e) {
		pserr << "Error serializing the message : " << e.what() << std::endl;
		throw e;
	}

	psout << "Finished executing the update" << std::endl;
}

//--------------------------------------------------------------------------------

void test_service::onSessionLeave()
{
	format::prefix_stream psout(std::cout);
	psout << "Unbound the session" << std::endl;
	format::prefix_pop();
}

//--------------------------------------------------------------------------------

void test_service::onDestroyService()
{
	format::prefix_stream psout(std::cout);
	psout << "SI destroyed" << std::endl;
}

//--------------------------------------------------------------------------------

