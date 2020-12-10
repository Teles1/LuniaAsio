#ifndef PseudoRandom_GUARDER
#define PseudoRandom_GUARDER

#pragma once
namespace Lunia {
	namespace Math {
		class MersenneTwister {
		public:
			// default constructor: uses default seed only if this is the first instance
			MersenneTwister(bool useCurrentTime = true);

			// constructor with 32 bit int as seed
			MersenneTwister(unsigned long s) { SetSeed(s); }
			// constructor with array of size 32 bit ints as seed
			MersenneTwister(const unsigned long* array, int size) { SetSeed(array, size); }
			// the two seed functions
			void SetSeed(unsigned long); // seed with 32 bit integer
			void SetSeed(const unsigned long*, int size); // seed with array
			// overload operator() to make this a generator (functor)
			//unsigned long operator()() { return GenerateRandom(); }

			//unsigned long Next() { return (*this)(); }
			~MersenneTwister() {} // destructor

		protected: // used by derived classes, otherwise not accessible; use the ()-operator
			unsigned long GenerateRandom() { // generate 32 bit random integer
				if (position == Constant1) GenerateState(); // new state vector needed
				// GenerateState() is split off to be non-inline, because it is only called once
				// in every 624 calls and otherwise irand() would become too big to get inlined
				unsigned long x = state[position++];
				x ^= (x >> 11);
				x ^= (x << 7) & 0x9D2C5680UL;
				x ^= (x << 15) & 0xEFC60000UL;
				return x ^ (x >> 18);
			}

		private:
			enum { Constant1 = 624, Constant2 = 397 };

			unsigned long state[Constant1]; // state vector array
			int position; // position in state array

			// private functions used to generate the pseudo random numbers
			unsigned long Twiddle(unsigned long u, unsigned long v) {
				return (((u & 0x80000000UL) | (v & 0x7FFFFFFFUL)) >> 1) ^ ((v & 1UL) ? 0x9908B0DFUL : 0x0UL);
			} // used by GenerateState()
			void GenerateState(); // generate new state
			// make copy constructor and assignment operator unavailable, they don't make sense
			MersenneTwister(const MersenneTwister&); // copy constructor not defined
			void operator=(const MersenneTwister&); // assignment operator not defined
		};

		class PseudoRandomInt : public MersenneTwister {
		public:
			PseudoRandomInt(bool useCurrentTime = true) : MersenneTwister(useCurrentTime) {}
			PseudoRandomInt(unsigned long seed) : MersenneTwister(seed) {}
			PseudoRandomInt(const unsigned long* seed, int size) : MersenneTwister(seed, size) {}

			unsigned long operator()() { return GenerateRandom(); }

			unsigned long Next() { return (*this)(); }

			unsigned long Next(unsigned long minval, unsigned long maxval) {
				return minval + (Next() % (maxval - minval));
			}

		};
		// generates double floating point numbers in the half-open interval [0, 1)
		class PseudoRandomFloat : public MersenneTwister {
		public:
			PseudoRandomFloat(bool useCurrentTime = true) : MersenneTwister(useCurrentTime) {}
			PseudoRandomFloat(unsigned long seed) : MersenneTwister(seed) {}
			PseudoRandomFloat(const unsigned long* seed, int size) : MersenneTwister(seed, size) {}
			~PseudoRandomFloat() {}
			double operator()() {
				return static_cast<double>(GenerateRandom()) * (1. / 4294967296.);
			} // divided by 2^32
			double Next() { return (*this)(); }
			double Next(double mul) { return Next() * mul; }
		private:
			PseudoRandomFloat(const PseudoRandomFloat&); // copy constructor not defined
			void operator=(const PseudoRandomFloat&); // assignment operator not defined
		};

		// generates double floating point numbers in the closed interval [0, 1]
		class PseudoRandomFloatClosed : public MersenneTwister {
		public:
			PseudoRandomFloatClosed(bool useCurrentTime = true) : MersenneTwister(useCurrentTime) {}
			PseudoRandomFloatClosed(unsigned long seed) : MersenneTwister(seed) {}
			PseudoRandomFloatClosed(const unsigned long* seed, int size) : MersenneTwister(seed, size) {}
			~PseudoRandomFloatClosed() {}
			double operator()() {
				return static_cast<double>(GenerateRandom()) * (1. / 4294967295.);
			} // divided by 2^32 - 1
			double Next() { return (*this)(); }
			double Next(double mul) { return Next() * mul; }
		private:
			PseudoRandomFloatClosed(const PseudoRandomFloatClosed&); // copy constructor not defined
			void operator=(const PseudoRandomFloatClosed&); // assignment operator not defined
		};

		// generates double floating point numbers in the open interval (0, 1)
		class PseudoRandomFloatOpen : public MersenneTwister {
		public:
			PseudoRandomFloatOpen(bool useCurrentTime = true) : MersenneTwister(useCurrentTime) {}
			PseudoRandomFloatOpen(unsigned long seed) : MersenneTwister(seed) {}
			PseudoRandomFloatOpen(const unsigned long* seed, int size) : MersenneTwister(seed, size) {}
			~PseudoRandomFloatOpen() {}
			double operator()() {
				return (static_cast<double>(GenerateRandom()) + .5) * (1. / 4294967296.);
			} // divided by 2^32
			double Next() { return (*this)(); }
			double Next(double mul) { return Next() * mul; }
		private:
			PseudoRandomFloatOpen(const PseudoRandomFloatOpen&); // copy constructor not defined
			void operator=(const PseudoRandomFloatOpen&); // assignment operator not defined
		};

		// generates 53 bit resolution doubles in the half-open interval [0, 1)
		class PseudoRandomFloat53Bit : public MersenneTwister {
		public:
			PseudoRandomFloat53Bit(bool useCurrentTime = true) : MersenneTwister(useCurrentTime) {}
			PseudoRandomFloat53Bit(unsigned long seed) : MersenneTwister(seed) {}
			PseudoRandomFloat53Bit(const unsigned long* seed, int size) : MersenneTwister(seed, size) {}
			~PseudoRandomFloat53Bit() {}
			double operator()() {
				return (static_cast<double>(GenerateRandom() >> 5) * 67108864. +
					static_cast<double>(GenerateRandom() >> 6)) * (1. / 9007199254740992.);
			}
			double Next() { return (*this)(); }
			double Next(double mul) { return Next() * mul; }
		private:
			PseudoRandomFloat53Bit(const PseudoRandomFloat53Bit&); // copy constructor not defined
			void operator=(const PseudoRandomFloat53Bit&); // assignment operator not defined
		};
	}
}


#endif // !PseudoRandom_GUARDER