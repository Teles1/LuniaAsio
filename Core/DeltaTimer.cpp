#include "DeltaTimer.h"
namespace Lunia {
	template <> 
	DeltaTimer<float>::DeltaTimer(float minDelta, float maxDelta) {
		__int64 largo = 0;
		last = 0;
		this->minDelta = minDelta;
		this->maxDelta = maxDelta;
		QueryPerformanceFrequency((LARGE_INTEGER*)&largo);

		freq = float(largo);
	}

	template<typename TPrecision>
	DeltaTimer<TPrecision>::DeltaTimer(TPrecision minDelta, TPrecision maxDelta)
	{
	}

	template <>
	float  DeltaTimer<float>::GetAbsoluteTime() const {
		__int64 largo;
		QueryPerformanceCounter((LARGE_INTEGER*)&largo);
		return float((float)largo / freq);
	}

	template <> 
	float DeltaTimer<float>::GetTimeDelta() {
		__int64 largo = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&largo);
		__int64 diff = largo - last;
		last = largo;
		float dif = float((float)diff / freq);
		if (dif > maxDelta) dif = maxDelta;
		if (dif < minDelta) dif = minDelta;
		return dif;

	}


	template <> 
	DeltaTimer<double>::DeltaTimer(double minDelta, double maxDelta) {
		__int64 largo = 0;
		last = 0;
		this->minDelta = minDelta;
		this->maxDelta = maxDelta;
		QueryPerformanceFrequency((LARGE_INTEGER*)&largo);

		freq = double(largo);
	}

	template <> 
	double  DeltaTimer<double>::GetAbsoluteTime() const {
		__int64 largo = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&largo);
		return double((float)largo / freq);
	}

	template <> 
	double DeltaTimer<double>::GetTimeDelta() {
		__int64 largo = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&largo);
		__int64 diff = largo - last;
		last = largo;
		double dif = double((double)diff / freq);
		if (dif > maxDelta) dif = maxDelta;
		if (dif < minDelta) dif = minDelta;
		return dif;

	}
}