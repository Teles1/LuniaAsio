#include <Core/Utils/ConfigReader.h>
#include <fstream>
namespace Lunia {
    Config::Config(const char* filename) {
        if (!FileExists(filename))
            Logger::GetInstance().Exception("Could not find config file provided => {0}", filename);
        ReadConfigFile(filename);
    }
    bool Config::FileExists(const std::string& name){
        struct stat buffer;
        return (stat(name.c_str(), &buffer) == 0);
    }
    void Config::ReadConfigFile(const char* filename)
    {
        std::ifstream in(filename);
        json j = json::parse((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

        if (j.at("Config").is_null())
            Logger::GetInstance().Exception("Cannot launch server without settings.");
        this->m_Config = j.at("Config");
        if (!j.at("Config")["LobbyServer"].is_null()) {
            this->m_ServerKind = ServerKind::LobbyKind;
            this->m_BaseStr = "LobbyServer";
        } else if (!j.at("Config")["PvpServer"].is_null()) {
            this->m_ServerKind = ServerKind::PVPKind;
            this->m_BaseStr = "PvpServer";
        }else if (!j.at("Config")["SquareServer"].is_null()) {
            this->m_ServerKind = ServerKind::SquareKind;
            this->m_BaseStr = "SquareServer";
        }else if (!j.at("Config")["StageServer"].is_null()) {
            this->m_ServerKind = ServerKind::StageKind;
            this->m_BaseStr = "StageServer";
        }else
            Logger::GetInstance().Exception("Could not find a valid ServerKind");
        if (m_Config.at(m_BaseStr).is_null())
            throw;
        m_Config.get_to(Settings);
    }
    ServerKind Config::GetKind()
    {
        return this->m_ServerKind;
    }
    Config& ConfigInstance(const char* filename) {
        return Config::GetInstance(filename);
    }
    void to_json(json& j, const LoggingStruct& o){
        j["Filename"] = o.Filename;
        j["LoggingLevel"] = o.LoggingLevel;
    }
    void from_json(const json& j, LoggingStruct& o){
        j.at("Filename").get_to(o.Filename);
        j.at("LoggingLevel").get_to(o.LoggingLevel);
    }
    void to_json(json& j, const ServerAddressStruct& o) {
        j["ServerIp"] = o.ServerIp;
        j["ServerPort"] = o.ServerPort;
    }
    void from_json(const json& j, ServerAddressStruct& o) {
        j.at("ServerIp").get_to(o.ServerIp);
        j.at("ServerPort").get_to(o.ServerPort);
    }
    void to_json(json& j, const GeneralSettings& t) {
        j["Logging"] = t.Logging;
        j["ServerAddress"] = t.ServerAddress;
        j["ApiUrl"] = t.ApiUrl;
        j["ServerName"] = t.ServerName;
        j["Capacity"] = t.Capacity;
        j["PingTimeout"] = t.PingTimeout;
        j["ShowPacket"] = t.ShowPacket;
        j["AchievementAddress"] = t.AchievementAddress;
    }
    void from_json(const json& j, GeneralSettings& t) {
        j.at("Logging").get_to(t.Logging);
        j.at("ServerAddress").get_to(t.ServerAddress);
        j.at("ApiUrl").get_to(t.ApiUrl);
        j.at("ServerName").get_to(t.ServerName);
        j.at("Capacity").get_to(t.Capacity);
        j.at("PingTimeout").get_to(t.PingTimeout);
        j.at("ShowPacket").get_to(t.ShowPacket);
        j.at("AchievementAddress").get_to(t.AchievementAddress);
    }
    void to_json(json& j, const SquareStruct& t) {
        j["Name"] = t.Name;
        j["StageGroupHash"] = t.StageGroupHash;
        j["AccessLevel"] = t.AccessLevel;
        j["Capacity"] = t.Capacity;
        j["OrderNumber"] = t.OrderNumber;
    }
    void from_json(const json& j, SquareStruct& t) {
        j.at("Name").get_to(t.Name);
        j.at("StageGroupHash").get_to(t.StageGroupHash);
        j.at("AccessLevel").get_to(t.AccessLevel);
        j.at("Capacity").get_to(t.Capacity);
        j.at("OrderNumber").get_to(t.OrderNumber);
    }
}