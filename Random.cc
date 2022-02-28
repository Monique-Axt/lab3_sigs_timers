// Random.cc -- implementation of pseudo-random number generator

#include "Random.h"
#include <time.h>
#include <thread>
#include <iostream>
#include <cstdlib>
#include <math.h>

using std::cerr;
using std::exit;
using std::stringstream;

const int32_t s1max = 2147483562;
const int32_t s2max = 2147483398;

void Random::checkSeeds()
{
	if (sizeof(int32_t) != 4) {
		cerr << "\nRandom only works on 32-bit machines\n";
		exit(1);
	}
	if (s1 < 1 || s1 > s1max) s1 = (labs(s1) % s1max) + 1;
	if (s2 < 1 || s2 > s2max) s2 = (labs(s2) % s2max) + 1;
}

Random::Random()
{
	time_t tt;		
	time(&tt);
	s1 = (int32_t)tt;
	s2 = (int32_t)(intptr_t)this;
	checkSeeds();
}

Random::Random(int32_t seed1, int32_t seed2)
{
	s1 = seed1;  s2 = seed2;
	checkSeeds();
}

double Random::next()
{
	int32_t Z;
	int32_t k;

	k = s1/53668;
	s1 = 40014 * (s1 - k * 53668) - k * 12211;
	if (s1 < 0) s1 += s1max + 1;

	k = s2/52774;
	s2 = 40692 * (s2 - k * 52774) - k * 3791;
	if (s2 < 0) s2 += s2max + 1;

	Z = s1 - s2;
	if (Z < 1) Z += s1max;

	return Z * 4.656613E-10;
}




