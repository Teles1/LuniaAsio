#include <sstream>
#include <codecvt>
#include <iostream>
#include <fmt/format.h>
#include <string>
#include <Core/Utils/StringUtil.h>
int main() {
	short a = 10;
	short b = 20;
	int* c = new int[10];
	memcpy(c, &a + &b, 4);
}
//return StringUtil::Format(L"%04d-%02d-%02d", dateValue.Year, dateValue.Month, dateValue.Day);