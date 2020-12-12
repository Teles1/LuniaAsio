#include <Core/Utils/ConfigReader.h>
namespace Lunia {
    bool FileExists(const std::string& name){
        struct stat buffer;
        return (stat(name.c_str(), &buffer) == 0);
    }
    void ReadConfigFile(const char* filename, ConfigType& config)
    {
        if (!FileExists(ConfigFile))
            Logger::GetInstance().Exception("Config file not found <{0}>", ConfigFile);
        //Reading the file to a string
        std::wifstream wif(filename);
        wif.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
        std::wstringstream wss;
        wss << wif.rdbuf();
        // Done reading. Now we gonna pass the contents to the json parser and then read the config file.
        std::wstring str(wss.str());
        json j = json::parse(str);
        if (j["Config"].is_object()) {
            auto& j_config = j["Config"];
            if (!j_config["LobbyServer"].is_null()) {
                Logger::GetInstance().Info("Instance loaded as LobbyServer");
                auto& lobby = j_config["LobbyServer"];

                config.ApiBase = lobby["ApiBase"].get<std::string>();
                config.ServerAddress = lobby["ServerAddress"].get<ServerAddress>();
                config.ServerName = lobby["ServerName"].get<std::string>();
                config.PingTimeout = lobby["PingTimeout"].get<uint32>();
                config.ShowPacket = lobby["ShowPacket"].get<bool>();
                config.Capacity = lobby["Capacity"].get<uint16>();
                config.AchievementAddress = lobby["AchievementAddress"].get<ServerAddress>();
            }
            else if (!j_config["StageServer"].is_null()) {
                Logger::GetInstance().Info("StageServer");

            }
            else if (!j_config["SquareServer"].is_null()) {
                Logger::GetInstance().Info("SquareServer");

            }
            else
                Logger::GetInstance().Exception("Instance for this server was not reconized.");
            //Here we are already sure that the config exists and the file is valid. lets keep on extracting.
            {
                if (j_config["Locale"].is_null())
                    Logger::GetInstance().Warn("There is no Locale to be loaded into this Server");
                else
                {
                    auto& locale = j_config["Locale"];
                    if (!locale["FobbidenNames"].is_null())
                        config.Locale.FobbidenNames = locale["FobbidenNames"].get_to(config.Locale.FobbidenNames);
                    if (!locale["FobbidenStrings"].is_null())
                        config.Locale.FobbidenStrings = locale["FobbidenStrings"].get_to(config.Locale.FobbidenStrings);
                    Logger::GetInstance().Info("Locales loaded");
                }
            }
        }
        else
            Logger::GetInstance().Exception("Cannot launch server without settings.");
        Logger::GetInstance().Info("Config loaded!");
    }
}