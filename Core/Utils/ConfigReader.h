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
    struct LoggingStruct {
    public:
        std::string Filename;
        uint8 LoggingLevel;
        friend void to_json(json& j, const LoggingStruct& o);
        friend void from_json(const json& j, LoggingStruct& o);
    };
    struct ServerAddressStruct {
    public:
        std::string ServerIp;
        uint16 ServerPort;
        friend void to_json(json& j, const ServerAddressStruct& o);
        friend void from_json(const json& j, ServerAddressStruct& o);
    };
    struct GeneralSettings{
    public:
        LoggingStruct Logging;
        ServerAddressStruct ServerAddress;
        std::string ApiUrl;
        std::string ServerName;
        uint32 Capacity;
        uint32 PingTimeout;
        bool ShowPacket;
        ServerAddressStruct AchievementAddress;
        friend void to_json(json& j, const GeneralSettings& t);
        friend void from_json(const json& j, GeneralSettings& t);
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
            if (m_Config.at(m_BaseStr).at(name).is_null()) {
                Logger::GetInstance().Exception("{0} is not present within {1} on the config file.", name, m_BaseStr);
            }
            return m_Config.at(m_BaseStr).at(name).get<T>();
        }
        template <typename T>
        T Get(const char* name, const T& defaulValue) {
            if (m_Config.at(m_BaseStr)[name].is_null()) {
                return defaulValue;
            }
            return m_Config.at(m_BaseStr)[name].get<T>();
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
    Config& ConfigInstance(const char* filename = "");
}
#endif // ! ConfigReader_H