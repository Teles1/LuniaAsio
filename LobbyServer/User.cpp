#include "User.h"
namespace Lobby 
{
	void User::Parse(uint8* buffer, size_t size)
	{
		std::cout << "Parser called" << std::endl;
		/*
			We should technically see but i think that if the server doesn't recognize the packet I say we let the client connection dies. 
			That's why I want to keep this call in here
			Parse only known packets. At least the ones which follow NetStream Standarts.
		*/
		HandleRead();
	}
}
