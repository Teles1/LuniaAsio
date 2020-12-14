#include "Resource.h"
#include <stack>
#include <Core/FileIO/File.h>
#include <Core/Utils/StringUtil.h>
#include "BInaryFormat1.h"

namespace Lunia {
	namespace Resource {
		struct BinaryLoader : IBinaryLoader {
		public:
			static BinaryLoader& GetInstance() {
				static BinaryLoader m_Instance;
				return m_Instance;
			};
			struct Constant {
				static const int BinaryFormat1ID = 1979;
			};

			BinaryFormat1::Header header;

			struct Instance {
				unsigned int* chunks;
				int chunkCount;

				Instance(unsigned int* pChunks, int pChunkCount) : chunks(pChunks), chunkCount(pChunkCount) {}
			};

			bool IsWanted(Instance& instance, unsigned int id) {
				for (int i = 0; i < instance.chunkCount; ++i) {
					if (instance.chunks[i] == id) return true;
				}
				return false;
			}

			virtual void Load(const std::wstring& filename, unsigned int* chunks, int chunkCount, IListener* listener) {

				Instance instance(chunks, chunkCount);

				FileIO::File file(filename, FileIO::File::ReadMode);

				class FileStream : public IListener::IStream {

					void operator=(const FileStream&) {}
					FileIO::File& file;
				public:

					void SetAbsolute(int index) {
						file.Seek(index, FileIO::File::Top);
					}
					void Forward(int offset) {
						file.Seek(offset, FileIO::File::Cur);
					}

					int GetAbsolute(int offset) {
						return file.Seek(0, FileIO::File::Cur) + offset;
					}

					virtual void Read(unsigned char* buffer, unsigned int size) {
						file.Read(buffer, size);
					}
					FileStream(FileIO::File& pFile) : file(pFile) {}
				} stream(file);

				stream.ReadStruct<BinaryFormat1::Header>(&header);

				if ((header.id != BinaryFormat1::Constant::ID) || (header.version != BinaryFormat1::Constant::Version)) {
					throw Exception(L"Invalid file format, filename={0}", filename.c_str());
				}


				std::stack<int>  stack;


				for (;;) {
					BinaryFormat1::Chunk chunk;
					stream.ReadStruct<BinaryFormat1::Chunk>(&chunk);


					if (IsWanted(instance, chunk.id)) { //if the chunk is something we want, if its not we proceed to the next sibling and try our luck

						if (chunk.nextSibling != BinaryFormat1::Constant::Empty) stack.push(stream.GetAbsolute(chunk.nextSibling));

						//if ( childPosition == -1) childPosition = siblingPosition;
						int childPosition = 0;

						if (chunk.nextChild != BinaryFormat1::Constant::Empty) childPosition = stream.GetAbsolute(chunk.nextChild);

						IListener::Iteration result = listener->OnChunk(chunk.id, stream);
						if (result == IListener::Iteration::Terminate) return; //terminating file parsing. 


						if (result == IListener::Iteration::Normal) { //continuing parsing of subchunks as well
							if (chunk.nextChild == BinaryFormat1::Constant::Empty) {            //no kids left..returning to the previous level in the hierarchy
								if (stack.empty()) return; // no chunks left, terminating
								int siblingPosition = stack.top();
								stack.pop();

								stream.SetAbsolute(siblingPosition);
							}
							else {
								stream.SetAbsolute(childPosition);
							}
						}
						else
							if (result == IListener::Iteration::SkipChildren) { //skip all kid chunks and proceed to the next sibling
								if (stack.empty()) return; //no siblings left, terminating
								int siblingPosition = stack.top();
								stack.pop();
								stream.SetAbsolute(siblingPosition);
							}
					}
					else {
						if (chunk.nextSibling != BinaryFormat1::Constant::Empty) stack.push(stream.GetAbsolute(chunk.nextSibling));
						if (stack.empty()) return; // no chunks left, terminating
						int siblingPosition = stack.top();
						stack.pop();

						stream.SetAbsolute(siblingPosition);

					}

				}

			}
		};
		IBinaryLoader& GetBinaryLoaderInstance() {
			return BinaryLoader::GetInstance();
		}
	}
}

