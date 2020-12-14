/* internal helper header - not exposed */
#pragma once

#include "File.h"

#define THROWFILEEXCEPTION(filename) ThrowFileException(filename)
#define THROW_CUSTOM_FILE_

namespace Lunia {
	namespace FileIO {
		void ThrowFileException(const wchar_t* filename);
	}
}

