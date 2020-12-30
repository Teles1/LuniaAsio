#ifndef ConfigReader_H
#define ConfigReader_H

#include <Network/Api/Json.hpp>

using json = nlohmann::json;
namespace Lunia {
    enum ServerKind {
        LobbyKind,
        PVPKind,
        SquareKind,
        StageKind
    };
    struct ServerAddress {
    public:
        std::string ServerIp;
        uint16 ServerPort;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(ServerAddress, ServerIp, ServerPort);
    };
    struct GeneralSettings{
    public:
        ServerAddress ServerAddress;
        std::string ApiUrl;
        std::string ServerName;
        uint32 Capacity;
        uint32 PingTimeout;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(GeneralSettings, ServerAddress, ApiUrl, ServerName, Capacity, PingTimeout);
    };
    struct SquareStruct {
    public:
        std::string Name;
        uint32 StageGroupHash;
        uint16 AccessLevel;
        uint8 OrderNumber;
        uint16 Capacity;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(SquareStruct, Name, StageGroupHash, AccessLevel, Capacity, OrderNumber);
    };

    struct Config {
    private:
        bool FileExists(const std::string& name);
        void ReadConfigFile(const char* filename);
        ServerKind m_ServerKind;
        const char* m_BaseStr; // store the string that will be the base for the configParser
    public:
        GeneralSettings Settings;
        ServerKind GetKind();
        template <typename T>
        T Get(const char* name) {
            if (m_Config[m_BaseStr][name].is_null()) {
                Logger::GetInstance().Exception("{0} is not present within {1} on the config file.", name, m_BaseStr);
            }
            return m_Config[m_BaseStr][name].get<T>();
        }
        template <typename T>
        T Get(const char* name, const T& defaulValue) {
            if (m_Config[m_BaseStr][name].is_null()) {
                return defaulValue;
            }
            return m_Config[m_BaseStr][name].get<T>();
        }
    private:
        json m_Config;
    private://singleton
        Config(const char* filename);
    public://singleton
        Config(const Config&) = delete; //anti creation  of a copy
        Config& operator= (const Config&) = delete; // anti copy
        Config(Config&&) = delete;
        Config& operator=(Config&&) = delete;
        ~Config() { }
        inline static Config& GetInstance(const char* filename = "") {
            static Config	m_instance(filename);
            return m_instance;
        }
    };
}
#endif // ! ConfigReader_H