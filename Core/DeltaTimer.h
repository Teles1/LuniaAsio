#pragma once
#include <windows.h>
namespace Lunia {
	template <typename TPrecision> 
	class DeltaTimer {
		TPrecision freq;
		TPrecision minDelta;
		TPrecision maxDelta;
		__int64 last;
	public:
		DeltaTimer(TPrecision minDelta = 0.001, TPrecision maxDelta = 0.2);
		TPrecision GetAbsoluteTime() const;
		TPrecision GetTimeDelta();
	};
}


