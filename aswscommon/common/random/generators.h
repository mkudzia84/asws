#pragma once

// generators.h
//
//  (C) Copyright 2017 Marcin Kudzia
//

//--------------------------------------------------------------------------------

#include <ctime>

//--------------------------------------------------------------------------------

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

//--------------------------------------------------------------------------------

namespace asws              {
namespace common            {
namespace random            {

// generators to be initialized
extern boost::random::mt19937 mt19937_gen;

//--------------------------------------------------------------------------------

/*
	Initialize the generator seeds with now()
*/
inline void initialize_seed()
{
	mt19937_gen.seed(static_cast<unsigned int>(::time(nullptr)));
}

/* 
	Thread safe int generator
*/
int gen_uniform_int(int min, int max);

//--------------------------------------------------------------------------------

} // namespace randon
} // namespace common
} // namespace asws

//--------------------------------------------------------------------------------

