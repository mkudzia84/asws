#pragma once

// xml_traverser.h
//
//  (C) Copyright 2014 Marcin Kudzia
//

#include <map>

//--------------------------------------------------------------------------------

#include "../../../3rdparty/pugixml-1.4/src/pugixml.hpp"

//--------------------------------------------------------------------------------

#include "xml_rule_set.h"

//--------------------------------------------------------------------------------

namespace asws              {
namespace common            {
namespace xml               {

//--------------------------------------------------------------------------------

/* XML node traverser 
 Traverses the tree, passes the control to
 the underlying traversers specified for specific nodes

 Performs the validation
*/
class xml_traverser 
{
public:
		 
	/* Function executed while traversing the tree
	 Should call traverser->descend() to descend down the tree
	*/
	typedef std::function<void(pugi::xml_node, xml_traverser*)> parse_func;

	/* Constructor */
	xml_traverser(xml_rule_set& rules, parse_func fnc)
		: _parse_func(fnc), _rules(rules)
	{ }

	/* Set handler for specific child node */
	void set_sub_traverser(const std::string& node, xml_traverser& traverser)
	{
		// if item exists - erase
		auto itr = _sub_traversers.find(node);
		if (itr != _sub_traversers.end())
			_sub_traversers.erase(itr);
	
		_sub_traversers.insert(
			std::map<std::string, xml_traverser&>::value_type(node, traverser) 
		); 
	}

	/* Traverse the node 
	 Executes:
	 - Validator
	 - Parse function
	   - Parse function should execute pass call

	 If any of them throws an exception parsing is terminated
	*/
	void traverse(pugi::xml_node node);

	/* Returns control to the traverser
	 Continues parsing the children nodes
	*/
	void pass(pugi::xml_node node);

	/* Traverser initializer 
	 Overloads operator() for clever initialization
	*/
	class xml_sub_traverser_init
	{
		friend class xml_traverser;
	private:

		/* Constructor */
		xml_sub_traverser_init(xml_traverser* traverser)
			: _owner(traverser)
		{ }

	public:

		/* Add new handler */
		xml_sub_traverser_init& operator()
			(std::string node, xml_traverser& traverser)
		{
			_owner->set_sub_traverser(node, traverser);
			return *this;
		}

	private:

		// parent traverser
		xml_traverser* _owner;
	}; // class xml_sub_traverser_initializer

	/* Set handlers */
	xml_sub_traverser_init set_sub_traversers()
	{
		return xml_sub_traverser_init(this);
	}

private:

	// Specified parse function 
	parse_func _parse_func;

	// Specified rule set
	xml_rule_set& _rules;

	// Sub-traversers
	std::map<std::string, xml_traverser&> _sub_traversers;
}; // class xml_traverser

//--------------------------------------------------------------------------------

} // namespace xml
} // namespace common
} // namespace asws

//--------------------------------------------------------------------------------
