#pragma once

#include "../Serializer.h"
#include "../../com_ptr.h"
#include "../../ReferenceCounted.h"

namespace Lunia {
	namespace Serializer {
		/**
		@juhnu:
		There are now two versions for the Structured Binary Stream Reader.
		The version1 reader does interpret the size of the element in characters not in bytes, if string's are read. This behaviour is fixed in the version 2 format

		*/
		struct StructuredBinaryStreamHeader {
			struct Identifier {
				static const uint32 Version1 = 0xfe123;
				static const uint32 Version2 = 0xfe124;
			};
			inline bool IsValid() { return IsVersion1() || IsVersion2(); }
			inline bool IsVersion1() { return identifier == Identifier::Version1; }
			inline bool IsVersion2() { return identifier == Identifier::Version2; }
			inline StructuredBinaryStreamHeader() : identifier(Identifier::Version2) {}
			unsigned int identifier;
		};
	}
}