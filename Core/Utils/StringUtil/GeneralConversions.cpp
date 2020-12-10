#include "GeneralConversions.h"
namespace Lunia {
	namespace StringUtil{

#pragma region To Unicode From Number
		template<> std::wstring To(unsigned char value) { return std::to_wstring(value); }
		template<> std::wstring To(unsigned short value) { return std::to_wstring(value); }
		template<> std::wstring To(unsigned int value) { return std::to_wstring(value); }
		template<> std::wstring To(unsigned long value) { return std::to_wstring(value); }
		template<> std::wstring To(unsigned long long value) { return std::to_wstring(value); }

		template<> std::wstring To(char value) { return std::to_wstring(value); }
		template<> std::wstring To(short value) { return std::to_wstring(value); }
		template<> std::wstring To(int value) { return std::to_wstring(value); }
		template<> std::wstring To(long value) { return std::to_wstring(value); }
		template<> std::wstring To(long long value) { return std::to_wstring(value); }
		template<> std::wstring To(long double value) { return std::to_wstring(value); }
		template<> std::wstring To(double value) { return std::to_wstring(value); }
		template<> std::wstring To(float value) { return std::to_wstring(value); }
#pragma endregion

#pragma region To ASCII From Number
		template<> std::string To(const unsigned char& value) { return std::to_string(value); }
		template<> std::string To(const unsigned short& value) { return std::to_string(value); }
		template<> std::string To(const unsigned int& value) { return std::to_string(value); }
		template<> std::string To(const unsigned long& value) { return std::to_string(value); }
		template<> std::string To(const unsigned long long& value) { return std::to_string(value); }

		template<> std::string To(const char& value) { return std::to_string(value); }
		template<> std::string To(const short& value) { return std::to_string(value); }
		template<> std::string To(const int& value) { return std::to_string(value); }
		template<> std::string To(const long& value) { return std::to_string(value); }
		template<> std::string To(const long long& value) { return std::to_string(value); }

		template<> std::string To(const long double& value) { return std::to_string(value); }
		template<> std::string To(const double& value) { return std::to_string(value); }
		template<> std::string To(const float& value) { return std::to_string(value); }
#pragma endregion

#pragma region Unicode/ASCII
		template<> const std::string To(const String& utf16) {
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;
			return wcu8.to_bytes(utf16);
		}
		template<> const std::wstring To(const std::string& utf8) {
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;
			return wcu8.from_bytes(utf8);
		}
		template<> const std::string To(const wchar_t* utf16) {
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;
			return wcu8.to_bytes(utf16);
		}
		template<> const std::wstring To(const char* utf8) {
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;
			return wcu8.from_bytes(utf8);
		}
#pragma endregion

#pragma region To Number from Unicode

		template<> const unsigned char To(std::wstring str) = delete; // there is no such thing. Cant fit a short into a char lol
		template<> const unsigned short To( std::wstring str) {
			std::wstring temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::wistringstream ss(std::move(temp));
			unsigned short number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const unsigned int To( std::wstring str) {
			std::wstring temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::wistringstream ss(std::move(temp));
			unsigned int number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const unsigned long To( std::wstring str) {
			std::wstring temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::wistringstream ss(std::move(temp));
			unsigned long number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const unsigned long long To( std::wstring str) {
			std::wstring temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::wistringstream ss(std::move(temp));
			unsigned long long number(0);
			ss >> number;
			return std::move(number);
		}

		template<> const char To( std::wstring str) = delete; // there is no such thing. Cant fit a short into a char lol
		template<> const short To( std::wstring str) {
			std::wstring temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::wistringstream ss(std::move(temp));
			short number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const int To( std::wstring str) {
			std::wstring temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::wistringstream ss(std::move(temp));
			int number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const long To( std::wstring str) {
			std::wstring temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::wistringstream ss(std::move(temp));
			long number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const long long To( std::wstring str) {
			std::wstring temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::wistringstream ss(std::move(temp));
			long long number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const float To( std::wstring str) {
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

		template<> const unsigned char To(const std::string& str) {
			std::string temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::istringstream ss(std::move(temp));
			unsigned char number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const unsigned short To(const std::string& str) {
			std::string temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::istringstream ss(std::move(temp));
			unsigned short number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const unsigned int To(const std::string& str) {
			std::string temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::istringstream ss(std::move(temp));
			unsigned int number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const unsigned long To(const std::string& str) {
			std::string temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::istringstream ss(std::move(temp));
			unsigned long number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const unsigned long long To(const std::string& str) {
			std::string temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::istringstream ss(std::move(temp));
			unsigned long long number(0);
			ss >> number;
			return std::move(number);
		}

		template<> const char To(const std::string& str) {
			std::string temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::istringstream ss(std::move(temp));
			char number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const short To(const std::string& str) {
			std::string temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::istringstream ss(std::move(temp));
			short number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const int To(const std::string& str) {
			std::string temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::istringstream ss(std::move(temp));
			int number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const long To(const std::string& str) {
			std::string temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::istringstream ss(std::move(temp));
			long number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const long long To(const std::string& str) {
			std::string temp;
			for (size_t i = 0; i < str.size(); i++)
				if (std::isdigit(str[i]))
					temp.push_back(str[i]);
			std::istringstream ss(std::move(temp));
			long long number(0);
			ss >> number;
			return std::move(number);
		}
		template<> const float To(const std::string& str) {
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
		const char ToInt8( std::string str)
		{
			return To<char>(str);
		}
				
}
}