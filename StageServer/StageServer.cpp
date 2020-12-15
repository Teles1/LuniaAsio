#include <Core/Resource/Resource.h>
#include <Info/Info.h>

int main()
{
    Lunia::Resource::ResourceSystemInstance().AddPath(L"./");
    Lunia::XRated::Database::DatabaseInstance();
    return 0;
}