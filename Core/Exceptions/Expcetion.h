#ifndef Excpetion_GUARD
#define Excpetion_GUARD
#pragma once
#include <Core/Core.h>
#include <exception>
#include <codecvt>

namespace Lunia {
	class Exception : public std::exception {
	private:
		std::wstring Message;
	public:

		template<typename... Args>
		inline Exception(const std::wstring& message, const Args &... args) {
			fmt::wformat_args x = fmt::make_format_args<fmt::wformat_context>(args...);
			std::wstring aux = fmt::vformat(std::wstring(message), x);
			this->Message = aux;
		}

		template<typename... Args>
		inline Exception(const wchar_t* message, const Args &... args) {
			fmt::wformat_args x = fmt::make_format_args<fmt::wformat_context>(args...);
			std::wstring aux = fmt::vformat(std::wstring(message), x);
			this->Message = aux;
		}

		template<typename... Args>
		inline Exception(const std::string& message, const Args &... args) {
			fmt::format_args x = fmt::make_format_args<fmt::format_context>(args...);
			std::string aux = fmt::vformat(std::string(message), x);
			this->Message = StringUtil::ToUnicode(aux);
		}

		template<typename... Args>
		inline Exception(const char* message, const Args &... args) {
			fmt::format_args x = fmt::make_format_args<fmt::format_context>(args...);
			std::string aux = fmt::vformat(std::string(message), x);
			this->Message = StringUtil::ToUnicode(aux);
		}

		const char* what() const throw () {
			return StringUtil::ToASCII(this->Message).c_str();
		}
	};
}
#endif //Excpetion_GUARD