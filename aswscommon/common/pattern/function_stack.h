#pragma once

// function_stack.h
//
//  (C) Copyright 2014 Marcin Kudzia
//

#include <functional>
#include <stack>

//--------------------------------------------------------------------------------

namespace asws              {
namespace common            {
namespace pattern           {

//--------------------------------------------------------------------------------

/* function_stack
 Stores specific handler functions stack on heap
 allowing execution only to the top one
*/
template <class func_def>
class function_stack
{
	friend class scoped;
public:

	// handler function
	typedef std::function<func_def> function_type;

	// Execute the top function
	function_type& operator()() 
	{
		if (_functions.empty())
			throw std::runtime_error("no functions on the stack");
		else 
			return _functions.top();
	}

	// Add function to the stack
	void push(function_type fnc)
	{
		_functions.push(fnc);
	}

	// Pop the function from the stack
	void pop()
	{
		_functions.pop();
	}

	// Get the function from the top
	function_type& top()
	{
		if (_functions.empty())
			throw std::runtime_error("no functions on the stack");
		return _functions.top();
	}

private:

	// Stack of the handlers
	std::stack<function_type> _functions;
}; // class function_stack

//--------------------------------------------------------------------------------

} // namespace pattern
} // namespace common
} // namespace asws

//--------------------------------------------------------------------------------


