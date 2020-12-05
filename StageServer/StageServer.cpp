#include "Header.h"

class later
{
public:
    template <class callable, class... arguments>
    later(int after, bool async, callable&& f, arguments&&... args)
    {
        std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));

        if (async)
        {
            std::thread([after, task]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(after));
                task();
                }).detach();
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(after));
            task();
        }
    }

};
void test1(std::string &a)
{
    a = "alpha";
    return;
}

void test2(int a)
{
    printf("%i\n", a);
    return;
}

void Parse(uint8* buffer, size_t size) {
	INFO_LOG("Trying to Parse Message {0}", size);
	Network::NetStreamReader reader(buffer, size);
	for (int i = 1; i < 128; i++) {
		printf("%02x ", buffer[i - 1]);
		//if ((i % 16 == 0))
		//	printf("\n");
	}
}

const uint16 Head::TypeHash = 10;
const uint16 Auth::TypeHash = 11;

void HandleHead(Head& in) {
	in.Print();
}
void HandleAuth(Auth& in) {
	std::cout << "I got called!" << std::endl;
	//
}
std::map<uint16, Protocol*> holder;
template<typename Type>
void Bind(void (Type&) ) {
	std::cout << Type::TypeHash << std::endl;
	auto x = new Type;
}
void func(void (*f)(Head& input)) {
}
uint16 HeaderSize = sizeof(HashType) + sizeof(LengthType);
int main()
{	

    Network::Crypt::Box decryptor(false);
    decryptor.SetKey(0);
	bool EncryptKey = true;
	uint8 aSrc[] = { 0x07 , 0x91 , 0xba , 0x1c , 0xb9 , 0xab , 0xe2 , 0x33 , 0x10 , 0xd9 , 0xa9 , 0x57 , 0x59 , 0xcd , 0x36 , 0x9b , 0x75 , 0xe1 , 0xf8 , 0x2f , 0x61 , 0x0c , 0x2c , 0x91 , 0xb7 , 0x1b , 0x6b , 0xd0 , 0x1d , 0xf1 , 0x38 , 0xb9 , 0xf1 , 0x15 , 0xd2 , 0xac , 0x4f , 0x60 , 0xe5 , 0xf0 , 0x89 , 0x5b , 0xad , 0x39 , 0x40 , 0xb1 , 0xdd , 0x1f , 0xdc , 0xfa , 0x1e , 0x4f , 0x8f , 0x0e , 0x5b , 0x93 , 0xda , 0xad , 0xd2 , 0x40 , 0x20 , 0xbd , 0xe6 , 0x2a , 0x74 , 0x61 , 0x1a , 0xc2 , 0x68 , 0x63 , 0x0e , 0x47 , 0xdc , 0x56 , 0x22 , 0xa2 , 0xf0 , 0x5a , 0x76 , 0x98 , 0x04 , 0xe8 , 0x6a , 0xbc , 0x38 , 0x75 , 0xde , 0xfe , 0x0e , 0x65 , 0x33 , 0x27 , 0x40 , 0xdd , 0x06 };
	uint8* src = &aSrc[0];
	size_t length(sizeof(aSrc));

	try {
		int total = 0;
		try {
			int total = 0;
			uint8* work = new uint8[Serializer::HeaderSize];
			do {
				if (length - total >= Serializer::HeaderSize) {
					if (EncryptKey) {

						uint32 keyBackup = decryptor.GetKey();
						memcpy(work, &src[total], Serializer::HeaderSize); // not referencing the pointer in memory because we don't wanna alter src before we are sure that we can.

						decryptor.Translate(work, Serializer::HeaderSize); // translate the header only

						LengthType* work_length = reinterpret_cast<LengthType*>(work);
						LengthType pSize = *work_length;

						HashType* header_length = reinterpret_cast<HashType*>(work + 2);
						HashType header = *header_length;
						printf("Psize[%.2X], Hash[%.2X]\n", pSize, header);
						if (header == Serializer::NetStreamHash) {
							//Translate only the bytes related to the packet
							if (pSize - Serializer::HeaderSize > length) {
								//Back up the packet for the next iteration. There is not enough bytes to parse this data, or, the client is trolling?
								WARN_LOG("Could not read entire packet. We should back up the packet for the next iteration WARN!!!!! pSize[{0}] > Length[{1}]", pSize - Serializer::HeaderSize, length);
							}
							else {
								//Packet can be deserialized safely.
								//Packet can be fully read because lenght > pSize
								decryptor.Translate(&src[size_t(total) + size_t(Serializer::HeaderSize)], pSize - Serializer::HeaderSize); // Decryption done
								Parse(&src[size_t(total) + size_t(Serializer::HeaderSize)], pSize - Serializer::HeaderSize);
								total += pSize;
							}
							//length - total - HeaderSize > lenght  means that there is more data to be processed.
							// Eventually we gonna have to add a system to simple add data to a buffer or maybe a pointer to hold the data?
						}
						else {
							decryptor.SetKey(keyBackup);
							total += 1;
						}
					}
					else { // If there is no cryptography we keep on adding 1 to the buffer until we find a NetStream
						LengthType* work_length = reinterpret_cast<LengthType*>(src + total);
						LengthType pSize = *work_length;

						HashType* header_length = reinterpret_cast<HashType*>(src + total + 2);
						HashType header = *header_length;
						printf("NonCrypt Psize[%.2X], Hash[%.2X]\n", pSize, header);
						if (header == Serializer::NetStreamHash) {
							printf("length[%d] total[%d] pSize[%d]\n", length, total, pSize);
							Parse(&src[size_t(total) + size_t(Serializer::HeaderSize)], pSize - Serializer::HeaderSize);
							total = +pSize;
						}
						else
							total += 1;
					}
					/*
					if (length - total < Serializer::HeaderSize) //Read Enough
						break;
					*/
				}
				else //Cant read header.
					break;
				/*
				else {
					WARN_LOG("Cannot read beyond stream");
					for (int i = 0; i < length; i++) {
						printf("%02x ", src[i]);
					}
					printf("\n");
					break;
				}*/
				if (total >= length)
					break;
			} while (true);
		}
		catch (...) { std::cout << "Exception trown\n"; }
	}
	catch (...) { ERROR_LOG("Exception trown parsing protocol."); }
    return 0;
}
