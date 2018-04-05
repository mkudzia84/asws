#pragma once

// message.h
//
//  (C) Copyright 2014 Marcin Kudzia
//

#include <memory>

//--------------------------------------------------------------------------------

#include "../interface_iworkload.h"
#include "../serialization/binary_archive.h"
#include "../serialization/soam_istream.h"
#include "../serialization/soam_ostream.h"
#include "../factory.h"

//--------------------------------------------------------------------------------

namespace asws        {
namespace workload    {
namespace symphony    {

//--------------------------------------------------------------------------------

/* 
	Message type
*/
namespace message_type {

typedef enum  {
	input    = 1,
	output   = 2
} type;

} // namespace message_type

//--------------------------------------------------------------------------------

/*
	Message class - wraps the IWorkload delivered class
*/
template <message_type::type type>
class message :
	public soam::Message
{
public:

	/* Create empty message */
	message()
		: _workload(nullptr)
	{ }

	/* Create message with workload */
	explicit message(iworkload_ptr workload)
		: _workload(workload)
	{ }

	/* Destructor */
	virtual ~message() 
	{ }
		
	/* Get the workload */
	iworkload_ptr payload()
	{
		return _workload;
	}

	/*
		Soam serialization code 
	*/
	virtual void onSerialize(soam::OutputStreamPtr& stream)
		throw (soam::SoamException);

	virtual void onDeserialize(soam::InputStreamPtr& stream)
		throw (soam::SoamException);

private:

	iworkload_ptr _workload;
}; // class message

//--------------------------------------------------------------------------------

/* 
	Serialization code
*/
template <>
inline void message<message_type::input>::onSerialize(soam::OutputStreamPtr& stream) throw (soam::SoamException)
{
	using namespace asws::workload::serialization;
	soam_ostream ostream(stream);
	binary_oarchive oar(ostream, serialization_level::request);
	// Save the type
	unsigned short type = _workload->get_type();
	oar << type << use_member<interface_iworkload>(_workload);
}

template <>
inline void message<message_type::output>::onSerialize(soam::OutputStreamPtr& stream) throw (soam::SoamException)
{
	using namespace asws::workload::serialization; 
	soam_ostream ostream(stream);
	binary_oarchive oar(ostream, serialization_level::response);
	unsigned short type = _workload->get_type();
	oar << type << use_member<interface_iworkload>(_workload);
}

template <>
inline void message<message_type::input>::onDeserialize(soam::InputStreamPtr& stream) throw (soam::SoamException)
{
	auto factory = workload::factory::instance();

	using namespace asws::workload::serialization;
	soam_istream istream(stream);
	binary_iarchive iar(istream, serialization_level::request);
	
	unsigned short type = 0;
	iar >> type;
	
	_workload.reset(factory->instantiate((interface_iworkload::type)type));
	iar >> use_member<interface_iworkload>(_workload);
	
}

template <>
inline void message<message_type::output>::onDeserialize(soam::InputStreamPtr& stream) throw (soam::SoamException)
{
	auto factory = workload::factory::instance();

	using namespace asws::workload::serialization;
	soam_istream istream(stream);
	binary_iarchive iar(istream, serialization_level::response);

	unsigned short type = 0;
	iar >> type;
	
	_workload.reset(factory->instantiate((interface_iworkload::type)type));
	iar >> use_member<interface_iworkload>(_workload);
}	

		
//--------------------------------------------------------------------------------

} // namespace symphony
} // namespace workload
} // namespace asws

//--------------------------------------------------------------------------------
