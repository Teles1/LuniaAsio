#include "FileHelper.h"

namespace Lunia {
	namespace FileIO {

		void ThrowFileException(const wchar_t* filename)
		{
			DWORD error = GetLastError();
			switch (error)
			{
			case ERROR_ACCESS_DENIED:
				Logger::GetInstance().Exception(L"AccessDeniedException, filename={0}", filename);
				throw(AccessDeniedException(filename));

			case ERROR_FILE_NOT_FOUND:
				Logger::GetInstance().Exception(L"FileNotFound, filename={0}", filename);
				throw(FileNotFoundException(filename));

			case ERROR_FILE_EXISTS:
				Logger::GetInstance().Exception(L"FileExists, filename={0}", filename);
				throw(FileExistsException(filename));

			case ERROR_PATH_NOT_FOUND:
				Logger::GetInstance().Exception(L"Path Not Found, filename={0}", filename);
				throw(PathNotFoundException(filename));

			case ERROR_TOO_MANY_OPEN_FILES:
				Logger::GetInstance().Exception(L"Too Many Open Files, filename={0}", filename);
				throw(TooManyOpenFilesException(filename));

			case ERROR_INVALID_NAME:
				Logger::GetInstance().Exception(L"Invalid name, filename={0}", filename);
				throw(InvalidNameException(filename));

			case ERROR_BAD_UNIT:
				Logger::GetInstance().Exception(L"Bad Unit, filename={0}", filename);
				throw(BadUnitException(filename));

			case ERROR_HANDLE_EOF:
				Logger::GetInstance().Exception(L"Error Handle EOF, filename={0}", filename);
				throw(HandleEofException(filename));

			default: // not defined file exception
				const wchar_t* errorMsg = std::to_wstring(error).c_str();
				Logger::GetInstance().Exception(L"File error : {0}, filename={1}", errorMsg, filename);
				throw(FileException(filename, errorMsg));
			}
		}
	}
}

