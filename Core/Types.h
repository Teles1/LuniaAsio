#pragma once
#include <string>
#include "Utils/Logger.h"
#include "Exceptions/Expcetion.h"
#ifndef MAX_BUFFER_SIZE 
	#define MAX_BUFFER_SIZE 4096
#endif
namespace Lunia {
	typedef unsigned __int64	    uint64;
	typedef unsigned int 			uint32;
	typedef unsigned short int		uint16;
	typedef unsigned char			uint8;

	typedef __int64					int64;
	typedef int						int32;
	typedef short int				int16;
	typedef char					int8;

	typedef std::wstring			String;
	typedef uint16					HashType;
	typedef unsigned short			LengthType;
}

namespace Lunia {
	namespace Constants {
		const uint32 Version = 0x17;
		const HashType NetStreamHash = 0xE055;
		const uint16 HeaderSize = sizeof(HashType) + sizeof(LengthType);
	}
}