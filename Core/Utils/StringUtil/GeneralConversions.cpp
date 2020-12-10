#include "GeneralConversions.h"
namespace Lunia {
	namespace StringUtil{
		char ToInt8(const std::string& str)
		{
			return To<char>(str);
		}
		char ToInt8(const char* str)
		{
			return To<char>(str);
		}
	}
}