#include "../Core/Common.h"
#include "../Network/Crypt/Crypt.h"

std::vector<uint8> buffer = { 0xBB, 0xA1, 0xCA, 0x6C, 0x09, 0xFB, 0xC0, 0x83, 0xB0, 0xE9, 0x3F, 0xA7, 0x8C, 0x5D, 0x43, 0xAB, 0xDB, 0x71, 0xBA, 0xCF, 0xB9, 0x85, 0x86, 0xC5, 0xD0, 0x1D, 0xBB, 0xF9, 0x48, 0xA4, 0x86, 0x06, 0x4F, 0x6E, 0xBA, 0x2D, 0x41, 0x9E, 0x75, 0x4E, 0xD9, 0x77, 0x26, 0x4E, 0x8B, 0xED, 0xB3, 0x99, 0xBD, 0x3C, 0x18, 0xC7, 0x8A, 0x55, 0x22, 0x00, 0x75, 0x07, 0xDB, 0x00, 0x20, 0x0B, 0x36, 0xC4, 0xC4, 0xE1, 0x2A, 0xDE, 0xF8, 0xC5, 0x9E, 0x66, 0xEC, 0xD7, 0xB2, 0xCB, 0x00, 0xAA, 0xC6, 0x26, 0x54, 0xCE, 0xFA, 0xCC, 0x48, 0x8C, 0xEE, 0x10, 0xBC, 0xF9, 0x02, 0x22, 0xB2, 0xF9, 0x97, 0xBB, 0xE4, 0x01, 0xA9 };

int main() {
	Network::Crypt::Box sender(false);
	sender.SetKey(328931486);
	bool EncryptKey = true;
	// in case of a packet containing multiple messages we gotta wrap this in a loop.
	size_t length = buffer.size(); // size of the packet received
	try {
		int total = 0;
		int result = 0;
		do {
			//check if still connected.
			if (length - total > sizeof(LengthType)) {
				if (EncryptKey) {
					unsigned char* work = &buffer[total]; // starting from where we left after running the loop n times
					sender.Translate(work, sizeof(LengthType)); // translate the header only
					LengthType* work_length = reinterpret_cast<LengthType*>(work);

					//backup to get roll back
					LengthType backup = *work_length;
					uint32 key = sender.GetKey(); // key gonna be changed right after Translate

					// partial decryption

					LengthType size = *work_length;
					if (size < sizeof(LengthType)) throw;
					if (size > length - total) { // not big enought to be parsed
						sender.SetKey(key);
						*work_length = backup;
						break;
					}

					sender.Translate(work + sizeof(LengthType), size - sizeof(LengthType));

					for (int i = 0; i < buffer.size(); i++) {
						printf("%.2X", work[i]);
					}
				}
			}
		} while (result);
	}
	catch (...) { std::cout << "Exception trown\n"; }
	

	
	std::cout << std::endl;
	return 0;
}

