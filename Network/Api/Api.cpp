#include "Api.h"
#include <Core/Utils/ConfigReader.h>
namespace Lunia {
    namespace XRated {
        namespace Net {
            template <> void Api::Append(const std::string& param) { m_Url.push_back(param); }
            template <> void Api::Append(const std::wstring& param) { m_Url.push_back(StringUtil::ToASCII(param)); }

            template <> void Api::Append(const DateTime& param) { Append(param.ToString()); }

            template <> void Api::Append(const float& param) { Append(StringUtil::ToASCII(param)); }
            template <> void Api::Append(const double& param) { Append(StringUtil::ToASCII(param)); }

            template <> void Api::Append(const uint64& param) { Append(StringUtil::ToASCII(param)); }
            template <> void Api::Append(const int64& param) { Append(StringUtil::ToASCII(param)); }

            template <> void Api::Append(const uint32& param) { Append(StringUtil::ToASCII(param)); }
            template <> void Api::Append(const int32& param) { Append(StringUtil::ToASCII(param)); }

            template <> void Api::Append(const uint16& param) { Append(StringUtil::ToASCII(param)); }
            template <> void Api::Append(const int16& param) { Append(StringUtil::ToASCII(param)); }

            template <> void Api::Append(const uint8& param) { Append(StringUtil::ToASCII(param)); }
            template <> void Api::Append(const int8& param) { Append(StringUtil::ToASCII(param)); }

            template <> void Api::Append(const bool& param) { Append(StringUtil::ToASCII(param)); }

            template <> void Api::Append(const char* param) { m_Url.push_back(param); }
            template <> void Api::Append(const wchar_t* param) { m_Url.push_back(StringUtil::ToASCII(param)); }

            std::string Api::ApiUrl = "http://localhost:51542/Lobby";
            Api::Api(const std::string& reqPage, const Methods& method) {
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
                cpr::Response r = cpr::Get(cpr::Url(BuildUrl()), m_Header, cpr::Timeout{ m_TimeOut });
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
                cpr::Response r = cpr::Post(cpr::Url(BuildUrl()), cpr::Body{ value.dump() }, m_Header, cpr::Timeout{ m_TimeOut });
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
            void Api::GetAsync(const json& payLoad) const {
                if(payLoad.is_null())
                    cpr::GetAsync( cpr::Url(BuildUrl()), m_Header, cpr::Timeout{ m_TimeOut } );
                else
                    cpr::PostAsync(cpr::Url(BuildUrl()), cpr::Body{ payLoad.dump() }, m_Header, cpr::Timeout{ m_TimeOut });
            }
            std::string Api::BuildUrl() const {
                std::string ret;
                for (size_t i = 0; i < m_Url.size(); i++) {
                    if (i > 1)
                        ret.append("|");
                    ret.append(m_Url[i]);
                }
                if (ApiUrl[ApiUrl.size() - 1] == '/')
                    return ApiUrl + ret;
                else
                    return ApiUrl + '/' + ret;
            }
            void Api::AddHeaders() {
                m_Header.emplace("ServerName", Config::GetInstance().Settings.ServerName);
                m_Header.emplace("ServerIp", Config::GetInstance().Settings.ServerAddress.ServerIp);
                m_Header.emplace("Content-Type", "application/json");
            }
            Answer::operator bool() const
            {
                return this->errorCode == 0 ? true : false;
            }
            Answer::operator int() const
            {
                return this->errorCode;
            }
            bool Answer::operator!() const
            {
                return this->errorCode != 0 ? true : false;
            }
            void to_json(json& j, const Answer& o) {
                j["errorMessage"] = o.errorMessage;
                j["errorCode"] = o.errorCode;
                j["data"] = o.resultObject;
            }
            void from_json(const json& j, Answer& o) {
                j.at("errorMessage").get_to(o.errorMessage);
                j.at("errorCode").get_to(o.errorCode);
                o.resultObject = j.at("data");
            }

            void to_json(json& j, const DateTime& o)
            {
                j = o.ToString();
            }
            void from_json(const json& j, DateTime& o)
            {
                o.Parse(j.get<std::string>());
            }
        }
    }
}
