#ifndef Random_GUARDER
#define Random_GUARDER

#pragma once
#include "PseudoRandom.h"

namespace Lunia {
	namespace Serializer {
		class ISerializable;
		class IStreamWriter;
		class IStreamReader;
	}
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
		class RandomByPortion
		{
		public:
			RandomByPortion();
			RandomByPortion(std::vector<float>::const_iterator begin, std::vector<float>::const_iterator end);

			void Assign(std::vector<float>::const_iterator begin, std::vector<float>::const_iterator end);
			void Clear();
			void Add(float portion);
			std::vector<float> GetPortion() const { return portions; }


			std::size_t Next() const; ///< @return index by the portion
			std::size_t GetSize() const; ///< @return size of the portion array

		public: /* Serializer::ISerializable implementation */
			void Serialize(Serializer::IStreamWriter& out) const;
			void Deserialize(Serializer::IStreamReader& in);

		private:
			void CalculateTotal();
			std::vector<float> portions;
			float total; /* calculated value from 'portions' */
		};

		/**
		Calculates a new random number using Mersenne Twister

		@return an integer value between half-closed interval of [0...MAX_LONG)
		*/
		unsigned long Random();

		/**
		Calculates a new random number using Mersenne Twister

		@param m a maximum value that will be returned
		@return an integer value between half-closed interval of [min..max)
		*/
		double RandomHalfClosed(double m);
	}
}


#endif // !Random_GUARDER