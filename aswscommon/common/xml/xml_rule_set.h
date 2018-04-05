#pragma once

// xml_rule_set.h
//
//  (C) Copyright 2014 Marcin Kudzia
//

//--------------------------------------------------------------------------------

#include <functional>
#include <map>
#include <set>
#include <list>
#include <sstream>


//--------------------------------------------------------------------------------

#include "../../../3rdparty/pugixml-1.4/src/pugixml.hpp"

//--------------------------------------------------------------------------------

namespace asws              {
namespace common            {
namespace xml               {

//--------------------------------------------------------------------------------

/* Validation result */
struct validation_result {

	/* Constructor */
	explicit validation_result(bool r, const std::string& d = "")
		: result(r), desc(d) {}

	bool result;
	std::string desc;
};

/* Validator function */
typedef std::function<validation_result(pugi::xml_node element)> 
	element_validation_func;
typedef std::function<validation_result(pugi::xml_attribute attribute)>
	attribute_validation_func;

//--------------------------------------------------------------------------------

/* element_validator
 Checks the elements against the list of rules
*/
class element_validator 
{
public:

	/* Element parameters */
	typedef enum {
		element_optional,
		element_mandatory
	} element_rules;

	// Rule map
	typedef std::map<std::string, element_rules> rule_map;

	/* Construct the validator */
	element_validator() { }

	/* Add item to the rule map */
	void add(std::string element, element_rules rule) 
	{
		_rules[element] = rule;
	}

	/* validation function */
	validation_result operator()(pugi::xml_node node);

private:

	// Map of element rules
	rule_map _rules;
}; // class element_validator

/* attribute_validator
 Checks the attributes against the list of rules
*/
class attribute_validator
{
public:
	
	/* Attribute parameters */
	typedef enum {
		attribute_optional,
		attribute_mandatory
	} attribute_rules;

	// Rule map
	typedef std::map<std::string, attribute_rules> rule_map;

	// Validator map
	typedef std::map<std::string, attribute_validation_func> validator_map;

	/* Construct the validator */
	attribute_validator() { }

	/* Add item to the rule map */
	void add(std::string attribute, attribute_rules rule)
	{
		_rules[attribute] = rule;
	}

	/* Add item to rule map with extra validator */
	void add(std::string attribute, attribute_rules rule, attribute_validation_func validator)
	{
		_rules[attribute] = rule;
		_validators[attribute] = validator;
	}

	/* validation function */
	validation_result operator()(pugi::xml_node node);

private:

	// Map of attribute rules
	rule_map _rules;

	// Map of validators
	validator_map _validators;
}; // class attribute_validator

/* enum_of
 Checks the value of an attribute against a list
*/
class enum_of
{
public:

	// List of allowed values
	typedef std::list<std::string> enum_list;

	/* Construct the validator */
	explicit
	enum_of(const std::string& val)
	{ 
		_values.push_back(val);
	}

	/* Validation function */
	validation_result operator()(pugi::xml_attribute node);

	/* For smart construction */
	enum_of& operator()(const std::string val)
	{
		_values.push_back(val);
		return *this;
	}

private:

	/* allowed values */
	enum_list _values;

}; // class enum_validator

//--------------------------------------------------------------------------------

/* xml_rule_set
 Verifies the rule set on a node
*/
class xml_rule_set
{
	friend class xml_rule_init;
public:

	/* Parent rule set */
	xml_rule_set() {}

	/* xml_rule_set_init
	 Overloads operator() for clever initialization */
	class xml_rule_init
	{
		friend class xml_rule_set;
	private:

		/* Construct the initializer */
		xml_rule_init(xml_rule_set* rule_set) 
			: _owner(rule_set) 
		{ }

	public:

		xml_rule_init& operator()(std::string element, element_validator::element_rules rules)
		{
			_owner->_node_validator.add(element, rules);
			return *this;
		}

		xml_rule_init& operator()(std::string attribute, attribute_validator::attribute_rules rules)
		{
			_owner->_attr_validator.add(attribute, rules);
			return *this;
		}

		xml_rule_init& operator()(std::string attribute, attribute_validator::attribute_rules rules, attribute_validation_func func)
		{
			_owner->_attr_validator.add(attribute, rules, func);
			return *this;
		}


	private:

		// Owner xml_rule_set
		xml_rule_set* _owner;
	}; // class xml_rule_initializer

	/* Perform validation */
	validation_result operator()(pugi::xml_node node);

	/* Add options helper function */
	xml_rule_init add_rules() 
	{
		return xml_rule_init(this);
	}

private:

	// Attribute validator
	element_validator		_node_validator;
	attribute_validator		_attr_validator;
}; // class xml_rule_set

//--------------------------------------------------------------------------------

} // namespace xml
} // namespace common
} // namespace asws

//--------------------------------------------------------------------------------
