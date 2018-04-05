/*
	Implementation of the bookkeeper
*/

#include "bookkeeper.h"

//--------------------------------------------------------------------------------

#include <cctype>

//--------------------------------------------------------------------------------

#include "../../common/format/prefix_stream.h"

//--------------------------------------------------------------------------------

using namespace asws::common;
using namespace asws::client::symphony;

//-------------------------------------------------------------------------------


void bookkeeper::start()
{
	_tp_execution_start = std::chrono::high_resolution_clock::now();
}

//-------------------------------------------------------------------------------

void bookkeeper::reg_submission(soam::TaskInputHandlePtr ptr)
{
	boost::mutex::scoped_lock smlock(_sm);
	_submitted++;
	_inp_handles.push_back(ptr);
}

//-------------------------------------------------------------------------------

void bookkeeper::reg_retrieval(soam::TaskOutputHandlePtr ptr)
{
	boost::mutex::scoped_lock smlock(_rm);
	_retrieved++;
	_out_handles.push_back(ptr);
}

//-------------------------------------------------------------------------------

void bookkeeper::async_submission_start(const std::string& name)
{
	_name = name;
	_tp_submission_start = std::chrono::high_resolution_clock::now();

	{
		boost::mutex::scoped_lock smlock(_sm);
		_is_async_submission_done = false;		
	}

	_submission_thread = boost::thread(std::bind(&bookkeeper::async_submission_func, this));
}

//-------------------------------------------------------------------------------

void bookkeeper::async_submission_end()
{
	{
		boost::mutex::scoped_lock smlock(_sm);
		_is_async_submission_done = true;
	}

	_retrieval_thread = boost::thread(std::bind(&bookkeeper::async_retrieval_func, this));
}

//-------------------------------------------------------------------------------

void bookkeeper::async_submission_wait()
{
	_submission_thread.join();
	_tp_submission_end = std::chrono::high_resolution_clock::now();
}

//-------------------------------------------------------------------------------

void bookkeeper::async_retrieval_wait()
{
	_retrieval_thread.join();
	_tp_execution_end = std::chrono::high_resolution_clock::now();
}

//-------------------------------------------------------------------------------

void bookkeeper::async_throttle_submission(int throttle)
{
	std::stringstream prefix;
	prefix << "[bookkeeping::throttle:" << throttle << "]";
	format::scoped_prefix sp(prefix.str());
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	int waiting_for_ack = 0;
	while (true) {
		{
			boost::mutex::scoped_lock(_sm);
			waiting_for_ack = _submitted - (_confirmed + _failed);

			if (waiting_for_ack > throttle) {
				psout << "submission queue [" << (_confirmed + _failed) << " / " << _submitted << "]" 
					<< ", waiting..." << std::endl;
			} else
				break;
		}

		boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
	}
}

//-------------------------------------------------------------------------------

void bookkeeper::async_submission_func()
{
	std::stringstream prefix;
	prefix << "[bookkeeping-" << _name << "::submission]";
	format::scoped_prefix sp(prefix.str());
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	bool done = false;

	while (!done) {
		// Go over all of the items in the queue - thread safe way
		{
			boost::mutex::scoped_lock smlock(_sm);

			_inp_handles.remove_if(
				[&](soam::TaskInputHandlePtr& handle) -> bool {
					soam::TaskInputHandle::SubmissionState state = handle->waitForSubmissionComplete(100);
					switch(state) {
					case soam::TaskInputHandle::SubmitCompleted:
						_confirmed++;
						return true;
					case soam::TaskInputHandle::SubmitFailed:
						_failed++;
						return true;
					default:
						/**
						* @TODO: @HACK to work around the SOAM issue
						* where status is stuck at pending but 
						* submission was successful
						*/
						const char* task_id = handle->getId();
						if (task_id != nullptr) {
							psout << "WARN: warkaround, status pending but taskId available : " << task_id << std::endl;
							_confirmed++;
							return true;
						} else {
							return false;
						}
					}
				}
			);

			if (_is_async_submission_done && ((_confirmed + _failed) == _submitted))
				done = true;

			psout << "submitted [" << (_confirmed + _failed) << " / " << _submitted << "], monitoring queue: " << _inp_handles.size() << std::endl;
		}

		// Wait for next iteration
		boost::this_thread::sleep_for(boost::chrono::milliseconds(50));
	}
}

//-------------------------------------------------------------------------------

void bookkeeper::async_retrieval_func()
{
	std::stringstream prefix;
	prefix << "[bookkeeping-" << _name << "::retrieval]";
	format::scoped_prefix sp(prefix.str());
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	bool done = false;

	while (!done) {
		// Go over all the items in the queue - thread safe way
		{
			boost::mutex::scoped_lock rmlock(_rm);

			while (!_out_handles.empty()) {
				soam::TaskOutputHandlePtr handle = _out_handles.front();
				if (handle->isSuccessful())
					_succeded++;
				else {
					soam::SoamExceptionPtr e = handle->getException();
					pserr << handle->getId() << ":error:" << e->what() << std::endl;
					_errors++;
				}

				_out_handles.pop_front();
			}

			// Check if retrieval done
			if (is_submission_completed())
				done = (_retrieved == _confirmed);

			psout << "retrieved [" << (_succeded + _errors) << "/" << _confirmed << "]" << std::endl;
		}
		
		// Wait for next iteration
		boost::this_thread::sleep_for(boost::chrono::milliseconds(50));
	}
}

//-------------------------------------------------------------------------------

int bookkeeper::get_tasks_outstanding() const
{
	boost::mutex::scoped_lock smlock(_sm);
	boost::mutex::scoped_lock rmlock(_rm);
	return _confirmed - _retrieved;
}

//-------------------------------------------------------------------------------

bool bookkeeper::is_submission_completed() const
{
	boost::mutex::scoped_lock smlock(_sm);

	if (!_is_async_submission_done)
		return false;
	else 
		return _submitted == (_confirmed + _failed);
}

//-------------------------------------------------------------------------------

bool bookkeeper::is_retrieval_completed() const
{
	bool submitted = is_submission_completed();
	if (!submitted)
		return false;
	else
	{
		boost::mutex::scoped_lock rmlock(_rm);
		return _retrieved == _confirmed;
	}
}

//-------------------------------------------------------------------------------