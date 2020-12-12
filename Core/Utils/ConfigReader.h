#include <Network/Api/Json.hpp>
#include <codecvt>
#include <fstream>
#include <sstream>
using json = nlohmann::json;
static std::string ConfigFile = "config.json";
namespace Lunia {
    bool FileExists(const std::string& name);
    struct ServerAddress {
    public:
        std::string ServerIp = "127.0.0.1";
        uint16 ServerPort = 15550;
    };
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ServerAddress, ServerIp, ServerPort)
    /*
        Default Config settings for now. I'll be changing this very soon lol It looks like actual shit.
    */
    struct Config {
    public:
        Config() {}
        struct ServerAddress ServerAddress;
        std::string ApiBase = "http://localhost:51542/Lobby";
        std::string ServerIp = "127.0.0.1";
        uint16 ServerPort = 15550;
        std::string ServerName = "ServerNameDefault";
        uint16 PingTimeout = 30;
        bool ShowPacket = false;
        uint16 Capacity = 250;
        struct ServerAddress AchievementAddress;
        struct Locale {
            std::vector<std::string> FobbidenNames;
            std::vector<std::string> FobbidenStrings;
        }Locale;
    };

    void ReadConfigFile(const char* filename, Config& config);
}
