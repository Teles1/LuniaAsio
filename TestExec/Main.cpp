#pragma once
#include <Info/Info.h>
#include <Core/Utils/ConfigReader.h>
using namespace Lunia;
int main(int argc, char* argv[]) {
	Lunia::Config::GetInstance("Config_Stage.json");
	Lunia::Resource::ResourceSystemInstance().AddPath(L"C:\\Users\\WINDOWS\\Desktop\\Lunia\\x64\\Debug");
	auto start = std::chrono::high_resolution_clock::now();
	XRated::Database::DatabaseInstance().Init();
	auto finish = std::chrono::high_resolution_clock::now();
	auto microseconds = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
	Logger::GetInstance().Info("Took {0}ms", microseconds.count());

	//XRated::Database::DatabaseInstance().InfoCollections.Stages.SaveXml();
	//XRated::Database::DatabaseInstance().InfoCollections.Skills.Init(false);
	//XRated::Database::DatabaseInstance().InfoCollections.EnchantTables.Init(false);
	//XRated::Database::DatabaseInstance().InfoCollections.Items.Init(false);
	//XRated::Database::DatabaseInstance().InfoCollections.EquipSet.Init(false);
	//XRated::Database::DatabaseInstance().InfoCollections.Pets.Init(false);
	return 0;
}