#ifndef Random_GUARDER
#define Random_GUARDER

#pragma once
#include "PseudoRandom.h"
namespace Math {
	class RandomInt
	{
	private:
		PseudoRandomInt random;

		// unable to copy
		RandomInt(const RandomInt&);
		void operator =(const RandomInt&);

	public:
		RandomInt();
		RandomInt(unsigned long seed);

		virtual unsigned int Next();
		virtual unsigned int Next(unsigned int excludedMax);
		virtual unsigned int Next(unsigned int includedMin, unsigned int excludedMax);
	};
	/**
	Calculates a new random number using Mersenne Twister

	@return an integer value between half-closed interval of [0...MAX_LONG)
	*/
	unsigned long Random();
}

#endif // !Random_GUARDER