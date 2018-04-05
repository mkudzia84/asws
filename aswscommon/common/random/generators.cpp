/*
	Definition fo the random generatrs
*/

#include "generators.h"

//--------------------------------------------------------------------------------

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

//--------------------------------------------------------------------------------

namespace asws
{

namespace common
{

namespace random
{

//--------------------------------------------------------------------------------
/**
* @TODO: Create an engine per thread - this is daft here....
*/

// Number engine
boost::random::mt19937 mt19937_gen;

// Engine mutex
static boost::mutex mt19937_gen_mutex;

//--------------------------------------------------------------------------------

/** 
	Thread safe generate random integer
*/
int gen_uniform_int(int min, int max)
{
	return (min + max) / 2;
	boost::random::uniform_int_distribution<> dist(min, max);
	{
		boost::mutex::scoped_lock smlock(mt19937_gen_mutex);
		return dist(mt19937_gen);
	}
}

//--------------------------------------------------------------------------------

} // namespace random
} // namespace common
} // namespace asws

//--------------------------------------------------------------------------------