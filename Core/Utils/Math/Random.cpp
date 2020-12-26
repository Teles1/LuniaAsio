#include "Random.h"
#include <Core/Serializer/Serializer.h>

namespace Lunia {
	namespace Math {
		namespace Private {
			PseudoRandomInt randomInt;
			PseudoRandomFloat randomFloat;
			std::mutex cs;
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
		double RandomHalfClosed(double m)
		{
			AutoLock lock(Private::cs);
			return Private::randomFloat.Next(m);
		}

		RandomByPortion::RandomByPortion()
		{
		}

		RandomByPortion::RandomByPortion(std::vector<float>::const_iterator begin, std::vector<float>::const_iterator end)
			: portions(begin, end)
		{
			CalculateTotal();
		}

		void RandomByPortion::Assign(std::vector<float>::const_iterator begin, std::vector<float>::const_iterator end)
		{
			portions.assign(begin, end);
			CalculateTotal();
		}

		void RandomByPortion::Clear()
		{
			portions.clear();
			CalculateTotal();
		}

		void RandomByPortion::Add(float portion)
		{
			portions.push_back(portion);
			CalculateTotal();
		}

		std::size_t RandomByPortion::Next() const
		{
			float r = (float)Math::RandomHalfClosed(1.0f);
			float cumulation = 0;
			for (std::size_t i = 0; i < portions.size(); ++i)
			{
				cumulation += (portions[i] / total);
				if (cumulation >= r) return i;
			}

			/* this happens */
			return portions.size() - 1; // choose last element, assumed that net portion should be 1
		}

		std::size_t RandomByPortion::GetSize() const
		{
			return portions.size();
		}

		void RandomByPortion::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"RandomByPortion");
			out.Write(L"portions", portions);
		}

		void RandomByPortion::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"RandomByPortion");
			in.Read(L"portions", portions);
			CalculateTotal();
		}

		void RandomByPortion::CalculateTotal()
		{
			total = 0; // make sure to initialize total though portions is empty
			for (auto& i : portions)
				total += i;
		}
	}
}