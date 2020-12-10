#include "GeneralConversions.h"
namespace Lunia {
	namespace StringUtil{

#pragma region To Unicode From Number
		template<> std::wstring inline To(const unsigned char& value) { return std::to_wstring(value); }
		template<> std::wstring inline To(const unsigned short& value) { return std::to_wstring(value); }
		template<> std::wstring inline To(const unsigned int& value) { return std::to_wstring(value); }
		template<> std::wstring inline To(const unsigned long& value) { return std::to_wstring(value); }
		template<> std::wstring inline To(const unsigned long long& value) { return std::to_wstring(value); }

		template<> std::wstring inline To(const char& value) { return std::to_wstring(value); }
		template<> std::wstring inline To(const short& value) { return std::to_wstring(value); }
		template<> std::wstring inline To(const int& value) { return std::to_wstring(value); }
		template<> std::wstring inline To(const long& value) { return std::to_wstring(value); }
		template<> std::wstring inline To(const long long& value) { return std::to_wstring(value); }
		template<> std::wstring inline To(const long double& value) { return std::to_wstring(value); }
		template<> std::wstring inline To(const double& value) { return std::to_wstring(value); }
		template<> std::wstring inline To(const float& value) { return std::to_wstring(value); }
#pragma endregion

#pragma region To ASCII From Number
		template<> std::string inline To(const unsigned char& value) { return std::to_string(value); }
		template<> std::string inline To(const unsigned short& value) { return std::to_string(value); }
		template<> std::string inline To(const unsigned int& value) { return std::to_string(value); }
		template<> std::string inline To(const unsigned long& value) { return std::to_string(value); }
		template<> std::string inline To(const unsigned long long& value) { return std::to_string(value); }

		template<> std::string inline To(const char& value) { return std::to_string(value); }
		template<> std::string inline To(const short& value) { return std::to_string(value); }
		template<> std::string inline To(const int& value) { return std::to_string(value); }
		template<> std::string inline To(const long& value) { return std::to_string(value); }
		template<> std::string inline To(const long long& value) { return std::to_string(value); }

		template<> std::string inline To(const long double& value) { return std::to_string(value); }
		template<> std::string inline To(const double& value) { return std::to_string(value); }
		template<> std::string inline To(const float& value) { return std::to_string(value); }
#pragma endregion

#pragma region Unicode/ASCII
		template<> const std::string inline To(const String& utf16) {
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;
			return wcu8.to_bytes(utf16);
		}
		template<> const std::wstring inline To(const std::string& utf8) {
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;
			return wcu8.from_bytes(utf8);
		}
		template<> const std::string inline To(const wchar_t* utf16) {
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;
			return wcu8.to_bytes(utf16);
		}
		template<> const std::wstring inline To(const char* utf8) {
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;
			return wcu8.from_bytes(utf8);
		}
#pragma endregion

#pragma region To Number from Unicode

		template<> const unsigned char inline To(const std::wstring& str) = delete; // there is no such thing. Cant fit a short into a char lol
		template<> const unsigned short inline To(const std::wstring& str) {
			std::wstring temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::wistringstream ss(std::move(temp));
			unsigned short number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const unsigned int inline To(const std::wstring& str) {
			std::wstring temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::wistringstream ss(std::move(temp));
			unsigned int number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const unsigned long inline To(const std::wstring& str) {
			std::wstring temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::wistringstream ss(std::move(temp));
			unsigned long number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const unsigned long long inline To(const std::wstring& str) {
			std::wstring temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::wistringstream ss(std::move(temp));
			unsigned long long number(0);
			ss >> number;
			return std::move(number);
		}

		template<> const char inline To(const std::wstring& str) = delete; // there is no such thing. Cant fit a short into a char lol
		template<> const short inline To(const std::wstring& str) {
			std::wstring temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::wistringstream ss(std::move(temp));
			short number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const int inline To(const std::wstring& str) {
			std::wstring temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::wistringstream ss(std::move(temp));
			int number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const long inline To(const std::wstring& str) {
			std::wstring temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::wistringstream ss(std::move(temp));
			long number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const long long inline To(const std::wstring& str) {
			std::wstring temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::wistringstream ss(std::move(temp));
			long long number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const float inline To(const std::wstring& str) {
			std::wstring temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::wistringstream ss(std::move(temp));
			float number(0);
			ss >> number;
			return std::move(number);
		}

#pragma endregion

#pragma region To Number from ASCII

		template<> const unsigned char inline To(const std::string& str) {
			std::string temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::istringstream ss(std::move(temp));
			unsigned char number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const unsigned short inline To(const std::string& str) {
			std::string temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::istringstream ss(std::move(temp));
			unsigned short number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const unsigned int inline To(const std::string& str) {
			std::string temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::istringstream ss(std::move(temp));
			unsigned int number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const unsigned long inline To(const std::string& str) {
			std::string temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::istringstream ss(std::move(temp));
			unsigned long number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const unsigned long long inline To(const std::string& str) {
			std::string temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::istringstream ss(std::move(temp));
			unsigned long long number(0);
			ss >> number;
			return std::move(number);
		}

		template<> const char inline To(const std::string& str) {
			std::string temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::istringstream ss(std::move(temp));
			char number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const short inline To(const std::string& str) {
			std::string temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::istringstream ss(std::move(temp));
			short number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const int inline To(const std::string& str) {
			std::string temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::istringstream ss(std::move(temp));
			int number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const long inline To(const std::string& str) {
			std::string temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::istringstream ss(std::move(temp));
			long number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const long long inline To(const std::string& str) {
			std::string temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::istringstream ss(std::move(temp));
			long long number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const float inline To(const std::string& str) {
			std::string temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::istringstream ss(std::move(temp));
			float number(0);
			ss >> number;
			return std::move(number);
		}

#pragma endregion
		const char ToInt8(const std::string& str)
		{
			return To<char>(str);
		}
				
}
}