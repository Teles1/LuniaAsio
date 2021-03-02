#include "UrlEncoder.h"
#include <Core/Utils/StringUtil.h>
#include <Core/Utils/StringUtil/utf8.h>
#include <sstream>
#include <iomanip>
namespace Lunia {
	namespace XRated {
		namespace Http {
			const std::string UrlEncoder::unsafeCharacters(" <>%{}|\\^~[]`'$+:\n"); // except ";", "/", "?", ":", "@", "=", "#" and "&"

			bool UrlEncoder::IsUnsafe(const char compareChar)
			{
				const int code = static_cast<const int>(compareChar);
				//if (code<33 || code>122) return true;
				if (unsafeCharacters.find(compareChar) != std::string::npos) return true;
				return false;
			}

			std::string UrlEncoder::Encode(const char* src, int size)
			{
				std::ostringstream escaped;
				escaped.fill('0');
				escaped << std::hex;

				for (auto i = 0; i < size; i++) {
					std::string::value_type c = src[i];

					// Keep alphanumeric and other accepted characters intact
					if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
						escaped << c;
						continue;
					}

					// Any other characters are percent-encoded
					escaped << std::uppercase;
					escaped << '%' << std::setw(2) << int((unsigned char)c);
					escaped << std::nouppercase;
				}

				return escaped.str();

				//std::string buffer;

				//std::string input(StringUtil::UTF8::Encode(StringUtil::ToUnicode(std::string(src, size))));

				//for (std::string::iterator i = input.begin(); i != input.end(); ++i) {
				//	if (IsUnsafe(*i)) {
				//		buffer += StringUtil::Format((char*)"%%%02x", static_cast<unsigned char>(*i));
				//	}
				//	else
				//		buffer += *i;
				//}
				//return StringUtil::ToASCII(StringUtil::UTF8::Decode(buffer));
				////return buffer;
			}

			std::string UrlEncoder::Encode(const std::string& src)
			{
				return Encode(src.c_str(), static_cast<int>(src.size()));
			}
		}
	}
}
