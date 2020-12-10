#pragma once
#include "Core/Utils/Buffer.h"
namespace Lunia {
    namespace Net {
        namespace Crypt {
            static unsigned char HashToByte(const void* p, unsigned int size);
            template <typename T>
            static unsigned char HashToByte(const T& input) { return HashToByte(&input, sizeof(T)); }

            inline unsigned char EncryptWithoutTable(unsigned char byte) { return (byte * 73) ^ 21; }
            struct IndexTable
            {
                IndexTable();
                unsigned char EncData[256][256]; // indexes to encode
                unsigned char DecData[256][256]; // indexes to decode
            };
            class Box {
                IndexTable indexTable;
            public:
                Box(bool isEncode, unsigned int key = 0);
                inline void SetKey(unsigned int key);
                inline unsigned char Translate(unsigned char byte);
                inline void Translate(void* source /*in&out*/, unsigned int bytes);
                inline unsigned int GetKey() const { return key; }

            private:
                unsigned int Translate(Buffer& buffer);

            private:
                unsigned int key;
                const unsigned char(*table)[256];
                // Crypt coder;
            };
        }
    }
}
