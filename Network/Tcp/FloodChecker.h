#pragma once
#include <Core/DeltaTimer.h>
namespace Lunia {
	namespace XRated {
		namespace Net {
			class FloodChecker
			{
			public:
				FloodChecker(float collectingPeriodSeconds = 3.f, uint16 limitKBperSec = 16);
				void Clear(float collectingPeriodSeconds = 3.f, uint16 limitKBperSec = 16);

				/// returns false for over-amount received data
				bool IsAdmitable(/*float dt, */uint32 receivedBytes, float costFactor = 1.f);
				float GetCurrentBandwidth() const { return bandwidth; }
				float GetRatio() const { return bandwidth / bandwidthLimit; }

			private:
				float collectingPeriod;
				float bandwidthLimit;
				float bandwidth;

				DeltaTimer<float> timer;
			};
		}
	}
}