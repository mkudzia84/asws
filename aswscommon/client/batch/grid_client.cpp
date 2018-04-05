/*
	Implementation of grid client
*/

//--------------------------------------------------------------------------------

#include <stack>

//--------------------------------------------------------------------------------

#include "grid_client.h"

#include "session.h"
#include "session_scheduler.h"

#include "task.h"
#include "task_sequence.h"
#include "task_repeater.h"

//--------------------------------------------------------------------------------

#include <boost/lexical_cast.hpp>
#include <boost/lambda/construct.hpp>

//--------------------------------------------------------------------------------

#include "../../common/format/prefix_stream.h"
#include "../../common/random/generators.h"

#include "../../common/xml/xml_rule_set.h"
#include "../../common/xml/xml_traverser.h"

#include "../../common/pattern/function_stack.h"

#include "../../workload/factory.h"

#include "../../workload/interface_iworkload.h"
#include "../../workload/idle_workload.h"
#include "../../workload/linpack_workload.h"
#include "../../workload/common_data.h"

//--------------------------------------------------------------------------------

using namespace asws::common;
using namespace asws::client::batch;

//--------------------------------------------------------------------------------

/* Constructor
 Initialize the factory for creation of workload
*/
grid_client::grid_client()
{
	format::scoped_prefix sp("[grid-client]");
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	// Register workloads
	psout << "Registering workload types" << std::endl;

	try {
		auto factory_ex = workload::factory_ex::instance();

		factory_ex->register_type("workload_idle", 
			[](workload::factory_ex::params_type params) -> workload::factory_ex::value_ptr
		{
			try {
				int runtime = boost::lexical_cast<int>(params.at("runtime"));
				int input_size = 0, output_size = 0;

				// Check if need to randomize 
				if (params.find("minInputSize") != params.end() &&
					params.find("maxInputSize") != params.end() &&
					params.find("minOutputSize") != params.end() &&
					params.find("maxOutputSize") != params.end())
				{
					input_size = asws::common::random::gen_uniform_int(
						boost::lexical_cast<int>(params.at("minInputSize")),
						boost::lexical_cast<int>(params.at("maxInputSize"))
						);
					output_size = asws::common::random::gen_uniform_int(
						boost::lexical_cast<int>(params.at("minOutputSize")),
						boost::lexical_cast<int>(params.at("maxOutputSize"))
						);
				}

				if (params.find("inputSize") != params.end() &&
					params.find("outputSize") != params.end())
				{
					input_size = boost::lexical_cast<int>(params.at("inputSize"));
					output_size = boost::lexical_cast<int>(params.at("outputSize"));
				}

				return new workload::idle_workload(runtime, input_size, output_size);
			} catch (boost::bad_lexical_cast e) {
				throw std::runtime_error("specified parameters are not integers");
			}
		});

		factory_ex->register_type("linpack_benchmark",
			[](workload::factory_ex::params_type params) -> workload::factory_ex::value_ptr
		{
			try {
				int nreps = boost::lexical_cast<int>(params.at("nReps"));
				int arsize = boost::lexical_cast<int>(params.at("arraySize"));

				return new workload::linpack_workload(nreps, arsize);
			} catch (boost::bad_lexical_cast e) {
				throw std::runtime_error("specified parameters are not integers");
			}
		});

		// Register basic factory types
		using namespace boost::lambda;
		auto factory = workload::factory::instance();
		factory->register_type(workload::interface_iworkload::idle, new_ptr<workload::idle_workload>());
		factory->register_type(workload::interface_iworkload::linpack_benchmark, new_ptr<workload::linpack_workload>());
		factory->register_type(workload::interface_iworkload::common_data, new_ptr<workload::common_data>());

	} catch (std::exception const &e) {
		pserr << "Failed to initialize the workload type : " << e.what() << std::endl;
		throw e;
	}
}

//--------------------------------------------------------------------------------

/* Destructor
 Clean up the workload types
*/
grid_client::~grid_client()
{
	workload::factory::instance()->release();
}

//--------------------------------------------------------------------------------

/* Load the batch configuration for execution 
 Parses the XML config

 Expects:
 <sym-test> root tag with attributes
	- app-name
	- username
	- password

 Child nodes:
	<client-thread>
 passes control down 
*/
void grid_client::load_config(pugi::xml_document& config)
{
	format::scoped_prefix sp("[config]");
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	// Read the root
	psout << "Parsing the SOAM application configuration" << std::endl;

	if (!config.child("sym-test"))
		throw std::runtime_error("<sym-test> node doesn't exist");

	pugi::xml_node sym_test_node = config.child("sym-test");

	try {
		parse_xml(sym_test_node);
	} catch (std::exception const &e) {
		pserr << "Failed to parse the <sym-test> node" << std::endl;
		throw e;
	}

	// Finished loading the config
	psout << "Finished loading the configuation" << std::endl;
}

//--------------------------------------------------------------------------------

/* Parses the root node
 Expects:
 <sym-test> root tag with attributes
	- app-name
	- username
	- password

 Child nodes:
	<client-thread>
 passes control down 
 */
void grid_client::parse_xml(pugi::xml_node st_node)
{
	/* Logger setup */
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	/* Stack for handling new sessions and tasks */
	pattern::function_stack<void(interface_isession*)> session_handlers;
	pattern::function_stack<void(interface_itask_queue*)> task_queue_handlers;
	
	//-------------------------------------------------------------------------------------
	// XML rule settings here
	//-------------------------------------------------------------------------------------

	/* <sym-test> level settings */
	xml::xml_rule_set sym_test_rules;
	sym_test_rules.add_rules()
		("app-name", xml::attribute_validator::attribute_mandatory)
		("username", xml::attribute_validator::attribute_mandatory)
		("password", xml::attribute_validator::attribute_mandatory)
		("client-thread", xml::element_validator::element_mandatory);

	/* <client-thread> level settings */
	xml::xml_rule_set client_thread_rules;
	client_thread_rules.add_rules()
		("name", xml::attribute_validator::attribute_mandatory)
		("session", xml::element_validator::element_optional)
		("session-scheduler", xml::element_validator::element_optional)
		("repeat", xml::element_validator::element_optional);
	
	/* <session> level settings */
	xml::xml_rule_set session_rules;
	session_rules.add_rules()
		("name", xml::attribute_validator::attribute_mandatory)
		("sym-session-name", xml::attribute_validator::attribute_mandatory)
		("sym-type", xml::attribute_validator::attribute_mandatory, xml::enum_of("sync")("sync-overlapped")("async")("async-overlapped"))
		("sym-common-data-size", xml::attribute_validator::attribute_optional)
		("repeat", xml::element_validator::element_optional)
		("task", xml::element_validator::element_optional);

	/* <session-scheduler> level settings */
	xml::xml_rule_set scheduler_rules;
	scheduler_rules.add_rules()
		("type", xml::attribute_validator::attribute_mandatory, xml::enum_of("sequential")("round_robin"))
		("repeat", xml::element_validator::element_optional)
		("session", xml::element_validator::element_optional);

	/* <task> level settings */
	xml::xml_rule_set task_rules;
	task_rules.add_rules()
		("workload-type", xml::attribute_validator::attribute_mandatory, xml::enum_of("workload_idle")("linpack_benchmark"))
		("param", xml::element_validator::element_optional);
	
	/* <param> level settings */
	xml::xml_rule_set param_rules;
	param_rules.add_rules()
		("name", xml::attribute_validator::attribute_mandatory);

	/* <repeat> level settings 
	 - version for <client-thread> */
	xml::xml_rule_set repeat_client_thread_rules;
	repeat_client_thread_rules.add_rules()
		("n-times", xml::attribute_validator::attribute_mandatory)
		("session", xml::element_validator::element_optional)
		("session-scheduler", xml::element_validator::element_optional);

	/* <repeat> level settings 
	 - version for <session-scheduler> */
	xml::xml_rule_set repeat_scheduler_rules;
	repeat_scheduler_rules.add_rules()
		("n-times", xml::attribute_validator::attribute_mandatory)
		("session", xml::element_validator::element_optional);

	/* <repeat> level settings
	 - version for <task> */
	xml::xml_rule_set repeat_task_rules;
	repeat_task_rules.add_rules()
		("n-times", xml::attribute_validator::attribute_mandatory)
		("task", xml::element_validator::element_optional)
		("repeat", xml::element_validator::element_optional);

	//-------------------------------------------------------------------------------------
	// XML parsing functions here
	//-------------------------------------------------------------------------------------

	/* <sym-test> parsing function */
	xml::xml_traverser::parse_func sym_test_func = [&](pugi::xml_node node, xml::xml_traverser* trv)->void {
		_app_name = node.attribute("app-name").as_string();
		_username = node.attribute("username").as_string();
		_password = node.attribute("password").as_string();
		trv->pass(node);
	};

	/* <client-thread> parsing function */
	xml::xml_traverser::parse_func client_thread_func = [&](pugi::xml_node node, xml::xml_traverser* trv) -> void 
	{
		std::string thread_name = node.attribute("name").as_string();

		// Setup logging
		std::stringstream prefix;
		prefix << "[" << thread_name << "]";
		format::scoped_prefix sp(prefix.str());

		// Create new worker
		std::unique_ptr<worker> new_worker(new worker(thread_name));
		
		// Pass control
		psout << "Loading workload definition for the thread" << std::endl;

		/*------------------------------------------------------------------------------*/
		/* Add the new created worker as current action handler on the stack            */
		session_handlers.push([&new_worker](interface_isession* sp) -> void 
			{
				new_worker->add_action(std::unique_ptr<interface_isession>(sp));
			}
		);

		// Pass control to the traverser
		try {
			trv->pass(node);
		} catch (std::exception const &e) {
			pserr << "Failed to parse element <client-thread> : " << e.what() << std::endl;
			throw e;
		}

		session_handlers.pop();
		/*------------------------------------------------------------------------------*/

		// Add to worker pool
		_wpool.add(std::move(new_worker));
	};

	/* <session> parsing function */
	xml::xml_traverser::parse_func session_func = [&](pugi::xml_node node, xml::xml_traverser* trv) -> void 
	{
		std::string session_name = node.attribute("name").as_string();
		std::string sym_name = node.attribute("sym-session-name").as_string();
		std::string sym_type = node.attribute("sym-type").as_string();

		// Setup logging
		std::stringstream prefix;
		prefix << "[" << session_name << "]";
		format::scoped_prefix sp(prefix.str());

		// Create the session
		psout << "Initializing the session" << std::endl;

		symphony::isession_ptr sym_session;
		int sym_flags = 0;

		if (!sym_type.compare("sync") || !sym_type.compare("sync-overlapped"))
			sym_session = _sym_client.create_session(symphony::interface_isession::sync);
		if (!sym_type.compare("async") || !sym_type.compare("async-overlapped"))
			sym_session = _sym_client.create_session(symphony::interface_isession::async);

		if (!sym_type.compare("sync-overlapped") || !sym_type.compare("async-overlapped"))
			sym_flags |= symphony::session_flags::send_overlapped;

		// Common data size
		int cdata_size = 0;

		if (node.attribute("sym-common-data-size")) {
			cdata_size = node.attribute("sym-common-data-size").as_int();
			psout << "Common data requested, size : " << cdata_size << std::endl;
		}

		// Create the session - a bit dirty but works
		std::unique_ptr<session> new_session(new session(sym_session, 
			symphony::session_attributes(session_name, sym_name, sym_flags, cdata_size)
		));

		// Pass the control 
		psout << "Loading session payload definition" << std::endl;

		/*------------------------------------------------------------------------------*/
		/* Add new created session as handler for the tasks workload                    */
		task_queue_handlers.push([&new_session](interface_itask_queue* tqp) -> void 
			{
				new_session->add_workload(std::unique_ptr<interface_itask_queue>(tqp));
			}
		);
		trv->pass(node); // traverse children
		task_queue_handlers.pop();
		/*------------------------------------------------------------------------------*/

		psout << "Payload definition loaded - adding to the worker" << std::endl;

		// Pass the parsed session to current session handler
		session_handlers.top()(new_session.release());
	};

	/* <session-scheduler> parsing function */
	xml::xml_traverser::parse_func scheduler_func = [&](pugi::xml_node node, xml::xml_traverser *trv) -> void 
	{
		std::string type = node.attribute("type").as_string();

		// Create the new scheduler
		session_scheduler::mode scheduler_mode;

		if (!type.compare("round_robin"))
			scheduler_mode = session_scheduler::round_robin;
		
		if (!type.compare("sequential"))
			scheduler_mode = session_scheduler::sequential;

		std::unique_ptr<session_scheduler> new_scheduler(new session_scheduler(scheduler_mode));

		/*------------------------------------------------------------------------------*/
		/* Add the session scheduler as new session handler                             */
		session_handlers.push([&new_scheduler](interface_isession* sp) -> void 
			{
				new_scheduler->add_session(std::unique_ptr<session>(dynamic_cast<session*>(sp)));
			}
		);
		trv->pass(node); // traverse children
		session_handlers.pop();
		/*------------------------------------------------------------------------------*/

		// Add to the higher level handler
		session_handlers.top()(new_scheduler.release());
	};

	/* <task> parsing func */
	xml::xml_traverser::parse_func task_func = [&](pugi::xml_node node, xml::xml_traverser *trv) -> void 
	{
		std::string type = node.attribute("workload-type").as_string();

		// Setup the param parser
		workload::factory_ex::params_type params;

		xml::xml_traverser::parse_func param_func = [&params](pugi::xml_node node, xml::xml_traverser *trv) -> void 
		{
			std::string name = node.attribute("name").as_string();
			std::string text = node.text().as_string();

			params.insert(workload::factory_ex::params_type::value_type(name, text));
		};

		// Traverse the underlying node
		xml::xml_traverser param_trv(param_rules, param_func);
		trv->set_sub_traversers()
			("param", param_trv);
		trv->pass(node);

		// Add to the task sequence
		task_queue_handlers.top()(new task(type, params));
	};

	/* <repeat> function */
	xml::xml_traverser::parse_func repeat_func = [&](pugi::xml_node node, xml::xml_traverser *trv) -> void 
	{
		int n_times = node.attribute("n-times").as_int();

		// Do it n-times
		for (int i = 0; i < n_times; i++) {
			// Setup logging
			std::stringstream prefix;
			prefix << "[#" << (i+1) << "]";
			format::scoped_prefix sp(prefix.str());

			// Pass control
			trv->pass(node);
		}
	};

	/* <repeat> under <task> function */
	xml::xml_traverser::parse_func repeat_task_func = [&](pugi::xml_node node, xml::xml_traverser *trv) -> void
	{
		int n_times = node.attribute("n-times").as_int();

		// Prepare the new task sequence
		std::unique_ptr<task_sequence> new_sequence(new task_sequence());

		/*------------------------------------------------------------------------------*/
		/* Add new task sequence as task handler to the queue */
		task_queue_handlers.push([&new_sequence](interface_itask_queue* tqp) -> void
			{
				new_sequence->add_workload(std::unique_ptr<interface_itask_queue>(tqp));
			}
		);
		trv->pass(node);
		task_queue_handlers.pop();
		/*------------------------------------------------------------------------------*/

		// Prepare new repeater
		std::unique_ptr<task_repeater> new_repeater(new task_repeater(n_times));
		new_repeater->set_workload(std::move(new_sequence));

		// Add to sequence
		task_queue_handlers.top()(new_repeater.release());
	};

	//-------------------------------------------------------------------------------------
	// XML traverser setup here
	//-------------------------------------------------------------------------------------

	/* Setup the traversers */
	xml::xml_traverser 
		sym_test_trv				(sym_test_rules, sym_test_func),
		client_thread_trv			(client_thread_rules, client_thread_func),
		session_trv					(session_rules, session_func),
		scheduler_trv				(scheduler_rules, scheduler_func),
		task_trv					(task_rules, task_func),
		repeat_client_thread_trv	(repeat_client_thread_rules, repeat_func),
		repeat_scheduler_trv		(repeat_scheduler_rules, repeat_func),
		repeat_task_trv				(repeat_task_rules, repeat_task_func);

	sym_test_trv.set_sub_traversers()
		("client-thread", client_thread_trv);
	
	client_thread_trv.set_sub_traversers()
		("session", session_trv)
		("session-scheduler", scheduler_trv)
		("repeat", repeat_client_thread_trv);

	session_trv.set_sub_traversers()
		("task", task_trv)
		("repeat", repeat_task_trv);

	scheduler_trv.set_sub_traversers()
		("session", session_trv)
		("repeat", repeat_scheduler_trv);

	repeat_client_thread_trv.set_sub_traversers()
		("session", session_trv)
		("session-scheduler", scheduler_trv);

	repeat_scheduler_trv.set_sub_traversers()
		("session", session_trv);

	repeat_task_trv.set_sub_traversers()
		("task", task_trv)
		("repeat", repeat_task_trv);

	// <task> subtraverser set in the task_func

	psout << "Loading the test configuration" << std::endl;

	try {
		sym_test_trv.traverse(st_node);
	} catch (std::exception const &e) {
		pserr << "Failed to load the test configuration : " << e.what() << std::endl;
		throw e;
	}

	psout << "Test configuration loaded" << std::endl;
}

//--------------------------------------------------------------------------------

void grid_client::run()
{
	format::scoped_prefix sp("[grid-client::run]");
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	// Connect to the application
	psout << "Attempting to connect to the grid" << std::endl;

	try {
		_sym_client.connect(_app_name, _username, _password);
	} catch (std::exception const &e) {
		pserr << e.what();
		throw e;
	}

	// Connection established - execute the batch
	psout << "Executing the workload" << std::endl;
	_wpool.start();
	
	psout << "Waiting for workload to finish" << std::endl;
	_wpool.finish();

	// Disconnect
	psout << "Finished, terminating connection to the grid" << std::endl;

	try {
		_sym_client.close();
	} catch (std::exception const &e) {
		pserr << e.what();
		throw e;
	}

	psout << "Test run completed..." << std::endl;
}

//--------------------------------------------------------------------------------
