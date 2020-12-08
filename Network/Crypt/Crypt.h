#pragma once
#include "Core/Utils/Buffer.h"
namespace Network {
    namespace Crypt {
        static unsigned char HashToByte(const void* p, unsigned int size);
        template <typename T>
        static unsigned char HashToByte(const T& input) { return HashToByte(&input, sizeof(T)); }

        inline unsigned char EncryptWithoutTable(unsigned char byte) { return (byte * 73) ^ 21; }
        class Box {
        public:
            Box(bool isEncode, unsigned int key = 0);
            inline void SetKey(unsigned int key);
            inline unsigned char Translate(unsigned char byte);
            inline void Translate(void* source /*in&out*/, unsigned int bytes);
            inline unsigned int GetKey() const { return key; }

        private:  // AllM::Net::ITranslator implementation
            unsigned int Translate(Buffer& buffer);

        private:
            unsigned int key;
            const unsigned char(*table)[256];
            // Crypt coder;
        };
    }
}
