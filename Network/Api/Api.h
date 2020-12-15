#ifndef Api_GUARD
#define Api_GUARD
#pragma warning(disable : 4996)
#pragma once
#include "./Core/Core.h"
#include <cpr/cpr.h>
#include "Json.hpp"
#include <string>

// for convenience
using json = nlohmann::json;
namespace Lunia {
    namespace Net {
        struct Answer {
        public:
            Answer(const std::string& errorMessage, const uint16& errorCode, const json& jsonAnswer = json())
                : errorMessage(errorMessage), errorCode(errorCode), resultObjet(jsonAnswer){}
            Answer() = delete;
            std::string errorMessage;
            uint16 errorCode;
            json resultObjet;
        };
        struct Api {
            static std::string ApiUrl;
        public:
            Api(const std::string& reqPage = "");

            template <typename T> inline void Append(const T& param) { Append(StringUtil::ToASCII(param)); }
            template <> inline void Append(const std::string& param) { m_Url.push_back(param); }

            template<typename T> inline Api& operator<< (const T& param)
            {
                Append(param);
                return *this;
            }

            Answer RequestApi() const;
            Answer RequestPost(json value) const;
            std::string BuildUrl() const;

            template<typename F>
            void GetAsync(F callback)
            {
                auto cb = cpr::GetCallback([callback](cpr::Response r)
                    {
                        Logger::GetInstance().Info("Api::GetAsync => status_code = {0}, text = {1}", r.status_code, r.text);

                        Answer answ("Whoops!", -1);

                        if (r.status_code == 200)
                        {
                            try 
                            {
                                json result = json::parse(r.text);

                                if (result != NULL && result.is_object()) {

                                    uint16 errorCode = result["errorCode"].get<uint16>();
                                    std::string errorMessage = result["errorMessage"].get<std::string>();

                                    answ = Answer(errorMessage, errorCode, result["data"].get<json>());
                                }
                            }
                            catch (...) {
                                Logger::GetInstance().Error("Could not parse json!");
                            }
                        }

                        callback(answ);

                    }, cpr::Url(BuildUrl()), m_Header, cpr::Timeout{ 1000 });
            }

            ~Api() {}
            void AddHeaders();
        private:
            cpr::Header                                     m_Header;
            std::vector<std::string>                        m_Url;
            json                                            m_Body;
        };
    }
}
#endif // !Api_GUARD