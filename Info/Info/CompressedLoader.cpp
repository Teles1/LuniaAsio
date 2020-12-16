#include "./CompressedLoader.h"
#include <Lzma/LzmaLib.h>
#include <iostream>
#include <Core/Serializer/Binary/BinaryStream.h>
#include <Core/FileIO/FileStream.h>


namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				int CompressedLoader::UnCompress(std::vector<uint8>& outBuf, const std::vector<uint8>& inBuf)
				{
					
					size_t dstLen = outBuf.size();
					size_t srcLen = inBuf.size() - LZMA_PROPS_SIZE;
					SRes res = LzmaUncompress(&outBuf[0], &dstLen, &inBuf[LZMA_PROPS_SIZE], &srcLen, &inBuf[0], LZMA_PROPS_SIZE);
					//outBuf.resize(dstLen); // If uncompressed data can be smaller
					return res;
					
				}

				/*
					Getting the Struct name based on the file name.
					Example: L"./Database/UpgradeTables.cbf" would return L"UpgradeTables" as result.
				*/
				String CompressedLoader::GetStructName() {
					std::wstring base_filename = filename.substr(filename.find_last_of(L"/") + 1);
					return base_filename.substr(0, base_filename.find_last_of(L"."));
				}

			}
		}
	}
}