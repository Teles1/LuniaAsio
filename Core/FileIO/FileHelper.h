#pragma once

#include "../helper.h"
#include "File.h"
#include "Directory.h"

#define THROWFILEEXCEPTION(filename) ThrowFileException(filename)
#define THROW_CUSTOM_FILE_

namespace Lunia {
	namespace FileIO {
		void ThrowFileException(const wchar_t* filename);
	}
}

