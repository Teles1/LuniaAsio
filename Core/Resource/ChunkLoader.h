#include <stack>
#include "../FileIO/File.h"
#include "IChunkLoaderListener.h"
#include "BinaryFormat1.h"

namespace Lunia {
	namespace Resource {
		struct ChunkLoader {
			typedef IRefCountedStreamReader IStream;

			BinaryFormat1::Header header;



			struct Instance {
				unsigned int* chunks;
				int chunkCount;

				inline Instance(unsigned int* pChunks, int pChunkCount) : chunks(pChunks), chunkCount(pChunkCount) {}
			};

			inline bool IsWanted(Instance& instance, unsigned int id) {
				for (int i = 0; i < instance.chunkCount; ++i) {
					if (instance.chunks[i] == id) return true;
				}
				return false;
			}



			virtual inline void Load(IStream& stream, unsigned int* chunks, int chunkCount, IChunkLoaderListener* listener) {

				Instance instance(chunks, chunkCount);
				//				ALLM_INFO((L"start stream cursor=%d, size=%d", stream.GetReadCursor(), stream.GetSize()));


				stream.ReadStruct<BinaryFormat1::Header>(&header);

				if ((header.id != BinaryFormat1::Constant::ID) || (header.version != BinaryFormat1::Constant::Version)) {
					throw std::runtime_error("Invalid file format");
				}


				std::stack<int>  stack;

#pragma warning( push )
#pragma warning( disable:4127 ) //conditional expression constant
				while (true) {
#pragma warning( pop )

					if (stream.GetReadCursor() == int(stream.GetSize())) return;



					BinaryFormat1::Chunk chunk;
					//					ALLM_INFO((L"stream cursor=%d, size=%d", stream.GetReadCursor(), stream.GetSize()));
					stream.ReadStruct<BinaryFormat1::Chunk>(&chunk);




					if (IsWanted(instance, chunk.id)) { //if the chunk is something we want, if its not we proceed to the next sibling and try our luck

						if (chunk.nextSibling != BinaryFormat1::Constant::Empty) stack.push(stream.GetReadCursor() + chunk.nextSibling);

						//if ( childPosition == -1) childPosition = siblingPosition;
						int childPosition = 0;

						if (chunk.nextChild != BinaryFormat1::Constant::Empty) childPosition = stream.GetReadCursor() + chunk.nextChild;

						IChunkLoaderListener::Iteration result = listener->OnChunk(chunk.id, stream);
						if (result == IChunkLoaderListener::Iteration::Terminate) return; //terminating file parsing. 


						if (result == IChunkLoaderListener::Iteration::Normal) { //continuing parsing of subchunks as well
							if (chunk.nextChild == BinaryFormat1::Constant::Empty) {            //no kids left..returning to the previous level in the hierarchy
								if (stack.empty()) return; // no chunks left, terminating
								int siblingPosition = stack.top();
								stack.pop();

								stream.SetReadCursor(siblingPosition, IStream::Begin);
							}
							else {
								stream.SetReadCursor(childPosition, IStream::Begin);
							}
						}
						else
							if (result == IChunkLoaderListener::Iteration::SkipChildren) { //skip all kid chunks and proceed to the next sibling
								if (stack.empty()) return; //no siblings left, terminating
								int siblingPosition = stack.top();
								stack.pop();
								stream.SetReadCursor(siblingPosition, IStream::Begin);
							}
					}
					else {
						if (chunk.nextSibling != BinaryFormat1::Constant::Empty) stack.push(stream.GetReadCursor() + chunk.nextSibling);
						if (stack.empty()) return; // no chunks left, terminating
						int siblingPosition = stack.top();
						stack.pop();

						stream.SetReadCursor(siblingPosition, IStream::Begin);

					}

				}

			}


		};

	}
}

