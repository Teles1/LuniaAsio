#pragma once
#include <Info/Info.h>
#include <Core/Utils/ConfigReader.h>
using namespace Lunia;
int main(int argc, char* argv[]) {
	Lunia::Config::GetInstance("Config_Stage.json");
	Lunia::Resource::ResourceSystemInstance().AddPath(L"C:\\Users\\WINDOWS\\Desktop\\Lunia\\x64\\Debug");
	XRated::Database::DatabaseInstance().Init();
	//XRated::Database::DatabaseInstance().InfoCollections.Stages.SaveXml();
	//XRated::Database::DatabaseInstance().InfoCollections.Skills.Init(false);
	//XRated::Database::DatabaseInstance().InfoCollections.EnchantTables.Init(false);
	//XRated::Database::DatabaseInstance().InfoCollections.Items.Init(false);
	//XRated::Database::DatabaseInstance().InfoCollections.EquipSet.Init(false);
	//XRated::Database::DatabaseInstance().InfoCollections.Pets.Init(false);
	return 0;
}