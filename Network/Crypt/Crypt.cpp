#include "Crypt.h"
/*
	Related to Cyphering the packets.
*/
namespace Lunia {
	namespace XRated {
		namespace Net {
			namespace Crypt {
				IndexTable::IndexTable()
				{
					for (int i = 0; i < 256; ++i)
					{
						unsigned char ii = EncryptWithoutTable(i);
						unsigned char ij = EncryptWithoutTable(ii);
						for (int j = 0; j < 256; ++j)
						{
							unsigned char jj = EncryptWithoutTable(j);
							jj += ij;
							jj ^= Constants::Version;
							EncData[ii][j] = jj;
							DecData[ii][jj] = j;
						}
					}
				}

				unsigned char HashToByte(const void* p, unsigned int size)
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
			}
		}
	}
}
