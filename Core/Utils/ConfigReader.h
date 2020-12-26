#include <Network/Api/Json.hpp>
#include <codecvt>
#include <fstream>
#include <sstream>
using json = nlohmann::json;
namespace Lunia {
    enum ServerType {
        Stage,
        Square,
        Lobby,
        PVP
    };
    struct ServerAddress {
    public:
        std::string ServerIp = "127.0.0.1";
        uint16 ServerPort = 15550;
    };
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ServerAddress, ServerIp, ServerPort)

    struct SquareStruct {
    public:
        std::string Name;
        uint32 StageGroupHash = 53518598;
        uint16 AccessLevel = 0;
        uint8 OrderNumber;
        uint16 Capacity = 70;
    };
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SquareStruct, Name, StageGroupHash, AccessLevel, Capacity, OrderNumber)
    /*
        Can be accessed from anywhere using Config::GetInstance()
        Not copiable, not clonable. One instance per run. MUST be unique.
    
    
    */
    struct Config {
    private:
        Config(const char* filename);
    public:
        Config(const Config&) = delete; //anti creation  of a copy
        Config& operator= (const Config&) = delete; // anti copy
        Config(Config&&) = delete;
        Config& operator=(Config&&) = delete;
        ~Config() { }
        inline static Config& GetInstance(const char* filename = "") {
            static Config	m_instance(filename);
            return m_instance;
        }
        bool FileExists(const std::string& name);
        /*
            Default Config for now. I'll be changing this very soon lol It looks like actual shit.
        */
        struct ServerAddress m_ServerAddress;
        std::string m_ApiBase = "http://localhost:51542/Lobby";
        std::string m_ServerName = "ServerNameDefault";
        uint16 m_PingTimeout = 30;
        bool m_ShowPacket = false;
        uint16 m_Capacity = 250;
        struct ServerAddress m_AchievementAddress;
        bool m_PreloadScripts;
        bool m_PreloadMovemap;
        std::vector<SquareStruct> m_SquareList;
        struct Locale {
        public:
            std::vector<std::string> m_FobbidenNames;
            std::vector<std::string> m_FobbidenStrings;
        }m_Locale;
        ServerType m_ServerType;
        void ReadConfigFile(const char* filename);
        ServerType GetType();
    };
}
