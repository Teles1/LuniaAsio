#include <Core/Resource/Resource.h>
#include <Core/FileIO/Directory.h>
#include <Core/FileIO/FileStream.h>
#include <Info/Info.h>
#include <fstream>
#include <LZMA/LzmaLib.h>
using namespace Lunia;
using namespace XRated::Database::Info;
uint32 UNCOMPRESSED_SIZE;

int decodeLzma(std::vector<unsigned char>& outBuf,const std::vector<unsigned char>& inBuf,UInt64 srcSize)
{
	size_t dstLen = outBuf.size();
	size_t srcLen = inBuf.size() - LZMA_PROPS_SIZE;
	SRes res = LzmaUncompress(&outBuf[0], &dstLen, &inBuf[LZMA_PROPS_SIZE], &srcLen, &inBuf[0], LZMA_PROPS_SIZE);
	//outBuf.resize(dstLen); // If uncompressed data can be smaller
	return res;
}

int main()
{
	Lunia::Resource::ResourceSystemInstance().AddPath(L"C:\\Users\\WINDOWS\\Desktop\\Lunia\\x64\\Debug");
	Logger::GetInstance().Info(L"startup directory is '{0}'", Lunia::Resource::ResourceSystemInstance().GetPrimaryDataPath());
	Lunia::XRated::Database::DatabaseInstance();

	Resource::StreamReader cbfreader = Resource::ResourceSystemInstance().CreateStreamReader(L"C:\\Users\\WINDOWS\\Desktop\\Lunia\\x64\\Debug\\Database\\ActionInfos.cbf"); 
	cbfreader->SetReadCursor(0, Lunia::IStream::Begin);
	/* CompressedBlockSizeInBytes */
	uint8* Buffer = reinterpret_cast<uint8*>(new char[4]);
	cbfreader->Read(Buffer, 4);
	/* BlockLength */
	size_t compSize = *(int*)Buffer;
	/* BlockRead */
	uint8* Block = reinterpret_cast<uint8*>(new char[compSize]);
	cbfreader->Read(Block, compSize);
	/* Block Reader */
	Resource::StreamReader reader = new FileIO::RefCountedMemoryStreamReader(Block, compSize);
	uint32 count(0);
	/* Loop in Block Size Left */
	while (reader->GetSizeLeft() > 0)
	{
		/* Reading and setting a first block data in ReplayBuffer*/
		reader->Read(Block, 4);
		size_t srcSize = *(int*)Block + LZMA_PROPS_SIZE;
		reader->Read(Block, 4);
		uint32 UNCOMPRESSED_SIZE = *(int*)Block;
		uint8* lReplayBuffer = reinterpret_cast<uint8*>(new char[srcSize]);
		reader->Read(lReplayBuffer, srcSize);

		/* Setting buffer input and output sizes*/
		std::vector<uint8> inBuf(srcSize);
		std::vector<uint8> outBuf;
		outBuf.resize(UNCOMPRESSED_SIZE);
		memcpy(&inBuf[0], lReplayBuffer, srcSize);

		/*decoding and decrypting the binary owo*/
		decodeLzma(outBuf, inBuf, compSize);
		Resource::SerializerStreamReader BlockDecrypted = Serializer::CreateBinaryStreamReader(new FileIO::RefCountedMemoryStreamReader(&outBuf[0], compSize));
	}


	std::cout << Buffer << std::endl;
	
	return 0;
}