/*
	Implementation of XML rule set
*/

#include "xml_rule_set.h"

//--------------------------------------------------------------------------------

using namespace asws::common::xml;

//--------------------------------------------------------------------------------

validation_result xml_rule_set::operator()(pugi::xml_node node)
{
	validation_result vres(true);

	vres = _node_validator(node);
	if (!vres.result)
		return vres;

	vres = _attr_validator(node);
	if (!vres.result)
		return vres;

	return vres;
}

//--------------------------------------------------------------------------------

validation_result element_validator::operator()(pugi::xml_node node)
{
	// list of elements found
	std::set<std::string> element_found;

	// Check for unspecified children
	for (auto child_node = node.begin(); child_node != node.end(); child_node++) {
		if (child_node->type() == pugi::xml_node_type::node_element) {
			if (_rules.find(child_node->name()) == _rules.end()) {
				std::stringstream desc;
				desc << "Unexpected element <" << child_node->name() << ">";
				return validation_result(false, desc.str());
			}

			element_found.insert(child_node->name());
		}
	}

	// Check for mandatory ones
	for (auto element_req = _rules.begin(); element_req != _rules.end(); element_req++) {
		if (element_req->second == element_validator::element_mandatory) {
			if (element_found.find(element_req->first) == element_found.end()) {
				std::stringstream desc;
				desc << "Mandatory element <" << element_req->first << "> not found";
				return validation_result(false, desc.str());
			}
		}
	}

	return validation_result(true);
}

//--------------------------------------------------------------------------------

validation_result attribute_validator::operator()(pugi::xml_node node)
{
	// List of attributes found
	std::set<std::string> attribute_found;

	// Check for unspecified children
	for (auto attr = node.attributes_begin(); attr != node.attributes_end(); attr++) {
		if (_rules.find(attr->name()) == _rules.end()) {
			std::stringstream desc;
			desc << "Unexpected attribute " << attr->name();
			return validation_result(false, desc.str());
		}

		// Apply validators
		if (_validators.find(attr->name()) != _validators.end()) {
			validation_result vres = _validators[attr->name()](*attr);

			if (!vres.result)
				return vres;
		}

		attribute_found.insert(attr->name());
	}

	// Check for mandatory ones
	for (auto attr_req = _rules.begin(); attr_req != _rules.end(); attr_req++) {
		if (attr_req->second == attribute_validator::attribute_mandatory) {
			if (attribute_found.find(attr_req->first) == attribute_found.end()) {
				std::stringstream desc;
				desc << "Mandatory attribute " << attr_req->first << " not found";
				return validation_result(false, desc.str());
			}
		}
	}

	return validation_result(true);
}

//--------------------------------------------------------------------------------

validation_result enum_of::operator()(pugi::xml_attribute attr)
{
	for (auto val_itr = _values.begin(); val_itr != _values.end(); val_itr++) {
		if (!val_itr->compare(attr.as_string()))
			return validation_result(true);
	}

	std::stringstream desc;
	desc << "invalid value of the attribute " << attr.name() << " = " << attr.as_string() << std::endl;
	desc << "expected : {";
	std::copy(_values.begin(), _values.end(), std::ostream_iterator<std::string>(desc, "|"));
	desc << "}";

	return validation_result(false, desc.str());
}

//--------------------------------------------------------------------------------