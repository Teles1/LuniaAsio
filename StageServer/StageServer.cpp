#include <iostream>
#include <fstream>
#include <string>
#include <Network/Api/Json.hpp>
#include <Core/GameConstants.h>
using json = nlohmann::json;
inline bool FileExists(const std::string& name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

bool CheckFlag(int i) {
    return true;
}
std::string ConfigFile = "config.json";
int main()
{
    auto helper = Lunia::XRated::Constants::ClassType::Bard;
    Lunia::XRated::Constants::IsDefaultClass(helper);
    return 0;
    if (!FileExists(ConfigFile))
        Logger::GetInstance().Exception("Config file not found <{0}>", ConfigFile);
    std::ifstream t("config.json", std::ios::in);
    std::string str((std::istreambuf_iterator<char>(t)),
        std::istreambuf_iterator<char>());
    t.close();
    json j = json::parse(str);
    std::cin;
    return 0;
}