#ifndef LUNIA_RESOURCE_BINARYFORMAT1_H
#define LUNIA_RESOURCE_BINARYFORMAT1_H

namespace Lunia {
	namespace Resource {

		struct BinaryFormat1 {

			struct Constant {
				static const int ID = 1979;
				static const int Version = 0;
				static const int Empty = -1;
			};

			struct Header {
				unsigned int id;
				unsigned int version;

				inline Header() : id(Constant::ID), version(Constant::Version) {}
			};

			struct Chunk {
				unsigned int id;
				int nextSibling;
				int nextChild;

				inline Chunk(unsigned int pID = 0, int pNextSibling = Constant::Empty, int pNextChild = Constant::Empty) :id(pID), nextSibling(pNextSibling), nextChild(pNextChild) {}
			};
		};
	}
}

#endif