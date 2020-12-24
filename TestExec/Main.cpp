#pragma once
#include <Info/Info.h>
using namespace Lunia;
int main(int argc, char* argv[]) {
	Lunia::Resource::ResourceSystemInstance().AddPath(L"C:\\Users\\WINDOWS\\Desktop\\Lunia\\x64\\Debug");
	XRated::Database::DatabaseInstance().Init();
	return 0;
}