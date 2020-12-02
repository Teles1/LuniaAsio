#include "Random.h"
namespace Math {
	namespace Private {
		PseudoRandomInt randomInt;
		PseudoRandomFloat randomFloat;

		PseudoRandomFloatClosed closedFloat;
	};
	RandomInt::RandomInt() {}
	RandomInt::RandomInt(unsigned long seed) : random(seed) {}

	unsigned int RandomInt::Next() { return random.Next(); }
	unsigned int RandomInt::Next(unsigned int excludedMax) { return Next(0, excludedMax); }
	unsigned int RandomInt::Next(unsigned int includedMin, unsigned int excludedMax) { return random.Next(includedMin, excludedMax); }

	unsigned long Random()
	{
		return Private::randomInt.Next();
	}
}
