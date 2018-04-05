/*
	Implementation of XML traverser
*/

#include "xml_traverser.h"

#include "../format/prefix_stream.h"

//--------------------------------------------------------------------------------

using namespace asws::common;
using namespace asws::common::xml;

//--------------------------------------------------------------------------------

void xml_traverser::traverse(pugi::xml_node node)
{
	// Validate
	validation_result vres = _rules(node);
	if (!vres.result) {
		std::stringstream err;
		err << "Failed validating element <" << node.name() << "> : " << vres.desc;
		throw std::runtime_error(err.str().c_str());
	}

	// Validation succesful - execute the handler
	_parse_func(node, this);
}

//--------------------------------------------------------------------------------

void xml_traverser::pass(pugi::xml_node node)
{
	format::prefix_stream psout(std::cout);
	format::prefix_stream pserr(std::cerr);

	try {
		for (auto child = node.begin(); child != node.end(); child++) {
			// Find the handler
			auto sub_trav_itr = _sub_traversers.find(child->name());
			if (sub_trav_itr == _sub_traversers.end()) {
				std::stringstream err;
				err << "No subtraverser found for node <" << child->name() << ">";
				throw std::runtime_error(err.str().c_str());
			}

			xml_traverser& sub_traverser = sub_trav_itr->second;

			// Traverse
			sub_traverser.traverse(*child);
		}
	} catch (std::exception e) {
		pserr << "Failed parsing element <" << node.name() << "> : " << e.what() << std::endl;
		throw e;
	}
}

//--------------------------------------------------------------------------------
