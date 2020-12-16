#include <Core/Resource/Resource.h>
#include <Core/FileIO/Directory.h>
#include <Core/FileIO/FileStream.h>
#include <Info/Info.h>
#include <Info/Info/Actions/CompressedActionsManager.h>
#include <Info/Info/Items/CompressedItemInfoManager.h>

int main()
{
	Lunia::Resource::ResourceSystemInstance().AddPath(L"C:\\Users\\WINDOWS\\Desktop\\Lunia\\x64\\Debug");
	Logger::GetInstance().Info(L"startup directory is '{0}'", Lunia::Resource::ResourceSystemInstance().GetPrimaryDataPath());
	Lunia::XRated::Database::DatabaseInstance();
	Lunia::XRated::Database::Info::CompressedItemInfoManager itemInfos;

	Lunia::XRated::Database::Info::CompressedActionInfoManager a;
	//a.Retrieve(0); // yeah, this works fine
	//a.Retrieve(0x07A6); // yeah, this works fine
	//a.Retrieve(0x3bec);
	return 0;
}