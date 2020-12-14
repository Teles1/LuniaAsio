#ifndef LUNIA_RESOURCE_ICHUNKLOADERLISTENER_H
#define LUNIA_RESOURCE_ICHUNKLOADERLISTENER_H

#include <string>

#include "../IStream.h"

namespace Lunia {
	namespace Resource {

		/*
		Loader for the chunk based binary format.

		There is also a loader for text based data.
		@see Resource::ITextLoader

		@author juhnu
		@date 1.1.2004

		**/

		class IChunkLoaderListener {
		public:

			enum Iteration {
				Normal,
				SkipChildren,
				Terminate
			};

			virtual Iteration inline OnChunk(unsigned int chunkid, IStream&) = 0;


			template<int mainLevel, int subLevel, int version> struct ChunkID {
				static const unsigned int Value = (mainLevel << 24) + (subLevel << 8) + (version);
			};

			/**
			Loads binary file and parses its, the format is chunk based

			@param filename name of the file
			@param chunks supported chunk ids
			@param chunkCount size of the list you just gave
			@param listener pointer to a class that handles the callback
			*/

		};

	}
}






#endif