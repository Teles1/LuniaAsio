#include <sstream>
#include <codecvt>
#include <iostream>
#include <fmt/format.h>
#include <string>
#include <Core/Utils/StringUtil.h>
int main() {
	unsigned int aux = 0;
	std::wcout << Lunia::StringUtil::To<std::wstring>(aux);
}
//return StringUtil::Format(L"%04d-%02d-%02d", dateValue.Year, dateValue.Month, dateValue.Day);