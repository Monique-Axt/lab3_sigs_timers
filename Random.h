#ifndef	RANDOM_H
#define	RANDOM_H


/* Random.h -- declarations for pseudo-random number generator


A psuedo-random number generator.  The function next() returns random
numbers uniformly distributed over the interval (0.0,1.0).

Reference:

Pierre L'ecuyer, "Efficient and Portable Combined Random Number
Generators", Commun. ACM 31, 6 (June 1988), 742-749.

   Modified jan 2005 / Orjan Sterner

*/

#include <stdint.h>

class Random {
	int32_t s1;
	int32_t s2;
	void checkSeeds();
public:
	Random();
	Random(int32_t seed1, int32_t seed2);
	double next();
	double operator()(void) { return next(); }

};

#endif
