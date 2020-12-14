#ifndef LUNIA_RESOURCE_BINARYLOADER_H
#define LUNIA_RESOURCE_BINARYLOADER_H

#include <string>
namespace Lunia {
	namespace Resource {

		/*
		Loader for the chunk based binary format.

		There is also a loader for text based data.
		@see AllM::Resource::ILoader

		@author juhnu
		@date 1.1.2004

		**/

		class IBinaryLoader {
		public:

			class IListener {
			public:
				class IStream {
				public:
					virtual void Read(unsigned char* buffer, unsigned int size) = 0;

					template <typename T> inline void ReadStruct(T* buffer, int count = 1) {
						Read(reinterpret_cast<unsigned char*>(buffer), sizeof(T) * count);
					}
				};

				enum Iteration {
					Normal,
					SkipChildren,
					Terminate
				};

				virtual Iteration OnChunk(unsigned int chunkid, IStream&) = 0;

			};

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
			virtual void Load(const std::wstring& filename, unsigned int* chunks, int chunkCount, IListener* listener) = 0;

		};

		/**
		Factory function for binary loader
		*/
		IBinaryLoader& GetBinaryLoaderInstance();

	}
}


#endif