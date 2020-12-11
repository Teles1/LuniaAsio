#include <sstream>
#include <codecvt>
#include <iostream>
#include <fmt/format.h>
#include <string>
#include <vector>
#include <Network/Api/Api.h>
#include <Core/Utils/ThreadPool.h>
using namespace Lunia;
void Test(int threadId) {
	int64 value(0);
	for (int i = 0; i < 100000000; i++) {
		value += 2;
	}
	printf("done\n");
}
int main() {
	Utils::thread_pool::GetInstance().push(Test);
	Utils::thread_pool::GetInstance().push(Test);
	Utils::thread_pool::GetInstance().push(Test);
	printf("Done pushing work\n");
}
//return StringUtil::Format(L"%04d-%02d-%02d", dateValue.Year, dateValue.Month, dateValue.Day);