#include "Api.h"
#include <Core/Utils/ConfigReader.h>
namespace Lunia {
    namespace Net {
        std::string Api::ApiUrl = "http://localhost:51542/Lobby";
        Api::Api(const std::string& reqPage) {
            std::string aux = reqPage; // I need it to be a non const lol
            if (aux == "") {
                Logger::GetInstance().Exception("The requested Procedure shouldn't be empty!");
            }
            if (ApiUrl.size() == 0) {
                Logger::GetInstance().Error("The api url shouldn't be empty!");
            }
            if (aux[aux.size() - 1] != '/')
                aux.push_back('/');
            m_Url.push_back(std::move(aux)); //getting rid of aux and pushing it inside m_url
            AddHeaders();
        }
        Answer Api::RequestApi() const {
            cpr::Response r = cpr::Get(cpr::Url(BuildUrl()), m_Header, cpr::Timeout{ 1000 });
            Logger::GetInstance().Info("Api returned [{0}] =>  {1}", r.status_code, r.text);
            if (r.status_code == 200) {
                try {
                    json result = json::parse(r.text);
                    if (result != NULL && result.is_object()) {
                        uint16 errorCode = result["errorCode"].get<uint16>();
                        std::string errorMessage = result["errorMessage"].get<std::string>();
                        return std::move(Answer(errorMessage, errorCode, result["data"].get<json>()));
                    }
                }
                catch (...) {
                    Logger::GetInstance().Error("Could not parse json!");
                }
            }
            return std::move(Answer("Whoops!", -1));
        }
        Answer Api::RequestPost(const json& value) const {
            cpr::Response r = cpr::Post(cpr::Url(BuildUrl()), cpr::Body{ value.dump() }, m_Header, cpr::Timeout{ 1000 });
            Logger::GetInstance().Info("Api returned [{0}] =>  {1}", r.status_code, r.text);
            if (r.status_code == 200) {
                try {
                    json result = json::parse(r.text);
                    if (result != NULL && result.is_object()) {
                        uint16 errorCode = result["errorCode"].get<uint16>();
                        std::string errorMessage = result["errorMessage"].get<std::string>();
                        return std::move(Answer(errorMessage, errorCode, result["data"].get<json>()));
                    }
                }
                catch (...) {
                    Logger::GetInstance().Error("Could not parse json!");
                }
            }
            return std::move(Answer("Whoops!", -1));
        }
        std::string Api::BuildUrl() const {
            std::string ret;
            for (size_t i = 0; i < m_Url.size(); i++) {
                if (i > 1)
                    ret.append("|");
                ret.append(m_Url[i]);
            }
            return ApiUrl + "/" + ret;
        }
        void Api::AddHeaders() {
            m_Header.emplace("ServerName", Config::GetInstance().Settings.ServerName);
            m_Header.emplace("ServerIp", Config::GetInstance().Settings.ServerAddress.ServerIp);
            m_Header.emplace("Content-Type", "application/json");
        }
    }
}