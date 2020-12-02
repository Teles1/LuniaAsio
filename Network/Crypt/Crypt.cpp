#include "Crypt.h"
#include "../Protocol.h"
/*
	Related to Cyphering the packets.


*/
using namespace Network;
using namespace Crypt;

unsigned char Network::Crypt::HashToByte(const void* p, unsigned int size)
{
	if (size == 0) throw;
	const unsigned char* src = reinterpret_cast<const unsigned char*>(p);
	unsigned char index = *src;
	index += static_cast<unsigned char>(4);
	while (--size)
	{
		index += EncryptWithoutTable(*(++src));
	}
	return index;
}

struct IndexTable
{
	IndexTable()
	{
		for (int i = 0; i < 256; ++i)
		{
			unsigned char ii = EncryptWithoutTable(i);
			unsigned char ij = EncryptWithoutTable(ii);
			for (int j = 0; j < 256; ++j)
			{
				unsigned char jj = EncryptWithoutTable(j);
				jj += ij;
				jj ^= Protocol::Version;
				EncData[ii][j] = jj;
				DecData[ii][jj] = j;
			}
		}
	}

	unsigned char EncData[256][256]; // indexes to encode
	unsigned char DecData[256][256]; // indexes to decode
} indexTable;						 // global static data
Box::Box(bool isEncode, unsigned int key)
	: table(isEncode ? indexTable.EncData : indexTable.DecData)
{
	SetKey(key);
}

void Box::SetKey(unsigned int key)
{
	this->key = key;
}

inline unsigned char Box::Translate(unsigned char byte)
{
	return table[HashToByte(key++)][byte];
}

inline void Box::Translate(void* source /*in&out*/, unsigned int bytes)
{
	unsigned char* buffer = reinterpret_cast<unsigned char*>(source);
	unsigned int backup = bytes;

	while (bytes--)
	{
		*buffer = Translate(*buffer);
		++buffer;
	}
}

unsigned int Box::Translate(Buffer& buffer)
{
	Translate(buffer.GetData(), buffer.GetLength());
	return buffer.GetLength();
}
