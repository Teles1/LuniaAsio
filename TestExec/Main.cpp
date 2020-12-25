#pragma once
#include <Info/Info.h>
#include <Core/Utils/ConfigReader.h>
using namespace Lunia;
int main(int argc, char* argv[]) {
	Lunia::Config::GetInstance("Config_Stage.json");
	Lunia::Resource::ResourceSystemInstance().AddPath(L"C:\\Users\\WINDOWS\\Desktop\\Lunia\\x64\\Debug");
	XRated::Database::DatabaseInstance().Init();
	XRated::Database::DatabaseInstance().InfoCollections.stageInfos.SaveXml();
	return 0;
}