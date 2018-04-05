#pragma once

#pragma warning ( push )
#pragma warning ( disable : 4244 )


// binary_archive.h
//
//  (C) Copyright 2017 Marcin Kudzia
//

//--------------------------------------------------------------------------------

#include <vector>

//--------------------------------------------------------------------------------

#include "serialization.h"

//--------------------------------------------------------------------------------

namespace asws              {
namespace workload          {
namespace serialization     {

//--------------------------------------------------------------------------------

/*
	Binary input archive
*/
class binary_iarchive
	: public serialization_level
{
public:

	/* Constructor */
	binary_iarchive(istream_base &stream, serialization_level::level level = serialization_level::all)
		: serialization_level(level), _stream(stream)
	{ }

	virtual ~binary_iarchive()
	{ }

	// Serialisation wrapper
	template <typename T>
	binary_iarchive& operator>>(T& data)
	{
		_stream.read(data);
		return *this;
	}

	/* Specialisation for utils */
	template <typename T>
	binary_iarchive& operator>>(const use_member<T> &obj)
	{
		obj.instance()->load(*this);
		return *this;
	}

	/* Specialisation */
	template <typename T>
	binary_iarchive& operator>>(std::vector<T> &vec)
	{
		unsigned int len = 0;
		_stream.read(len);
		vec.resize(len / sizeof(T));
		_stream.read((void*)&vec[0], len);
		return *this;
	}

private:
	istream_base &_stream;
};


/*
	Binary output archive
*/
class binary_oarchive
	: public serialization_level
{
public:

	/* Constructor */
	binary_oarchive(ostream_base &stream, serialization_level::level level = serialization_level::all)
		: serialization_level(level), _stream(stream)
	{ }

	virtual ~binary_oarchive()
	{ }

	// Operator overloading for serialisation
	template <typename T>
	binary_oarchive& operator<<(const T& data) {
		_stream.write(data);
		return *this;
	}

	/* Specialisation for utils */
	template <typename T>
	binary_oarchive& operator<<(const use_member<T> &obj)
	{
		obj.instance()->save(*this);
		return *this;
	}

	// Specialization
	template <typename T>
	binary_oarchive& operator<<(const std::vector<T> &vec)
	{
		unsigned int len = sizeof(T) * vec.size();

		_stream.write(len);
		_stream.write((void*)&vec[0], len);
		return *this;
	}

private:
	ostream_base &_stream;
};

//--------------------------------------------------------------------------------

} // namespace serialization
} // namespace workload
} // namespace asws

//--------------------------------------------------------------------------------

#pragma warning ( pop )
