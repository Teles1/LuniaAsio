#include "FloodChecker.h"
namespace Lunia {
	namespace XRated {
		namespace Net {
			FloodChecker::FloodChecker(float collectingPeriodSeconds, uint16 limitKBperSec)
				: collectingPeriod(collectingPeriodSeconds), bandwidthLimit(static_cast<float>(limitKBperSec * 1024)), bandwidth(0)
				, timer(0.001f, 30.f)
			{
				timer.GetTimeDelta();
			}

			void FloodChecker::Clear(float collectingPeriodSeconds, uint16 limitKBperSec)
			{
				bandwidth = 0;
				collectingPeriod = collectingPeriodSeconds;
				bandwidthLimit = static_cast<float>(limitKBperSec * 1024);
				timer.GetTimeDelta();
			}


			bool FloodChecker::IsAdmitable(uint32 receivedBytes, float costFactor)
			{
				float dt = timer.GetTimeDelta();
				float received = receivedBytes * costFactor;

				if (dt >= collectingPeriod)
				{
					bandwidth = received / dt;
				}
				else
				{
					bandwidth = (bandwidth * (collectingPeriod - dt) + (receivedBytes * costFactor)) / collectingPeriod;
				}

				if (bandwidth > bandwidthLimit) return false;
				return true;
			}
		}
	}
}