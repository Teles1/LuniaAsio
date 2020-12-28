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

        if (j["Config"].is_null())
            Logger::GetInstance().Exception("Cannot launch server without settings.");
        this->m_Config = j["Config"];
        if (!j["Config"]["LobbyServer"].is_null()) {
            this->m_ServerKind = ServerKind::LobbyKind;
            this->m_BaseStr = "LobbyServer";
        } else if (!j["Config"]["PvpServer"].is_null()) {
            this->m_ServerKind = ServerKind::PVPKind;
            this->m_BaseStr = "PvpServer";
        }else if (!j["Config"]["SquareServer"].is_null()) {
            this->m_ServerKind = ServerKind::SquareKind;
            this->m_BaseStr = "SquareServer";
        }else if (!j["Config"]["StageServer"].is_null()) {
            this->m_ServerKind = ServerKind::StageKind;
            this->m_BaseStr = "StageServer";
        }else
            Logger::GetInstance().Exception("Could not find a valid ServerKind");
        if (m_Config[m_BaseStr].is_null())
            throw;
        m_Config[m_BaseStr].get_to(Settings);
    }
    ServerKind Config::GetKind()
    {
        return this->m_ServerKind;
    }
}