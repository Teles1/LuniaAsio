#ifndef Api_GUARD
#define Api_GUARD
#pragma warning(disable : 4996)
#pragma once
#include "./Core/Core.h"
#include <cpr/cpr.h>
#include "Json.hpp"
#include <Core/Utils/DateTime.h>
// for convenience
using json = nlohmann::json;
namespace nlohmann {
    template <>
    struct adl_serializer<std::wstring> {
        static void to_json(json& j, const std::wstring& str) {
            j = Lunia::StringUtil::ToASCII(str);
        }

        static void from_json(const json& j, std::wstring& str) {
            str = Lunia::StringUtil::ToUnicode(j.get<std::string>());
        }
    };
}
namespace Lunia {
    namespace XRated {
        namespace StageServer {
            class User;
            typedef std::shared_ptr<User> UserSharedPtr;
        }
        namespace Net {
            struct Answer {
            public:
                Answer(const std::string& errorMessage, const uint16& errorCode, const json& jsonAnswer = json())
                    : errorMessage(errorMessage), errorCode(errorCode), resultObject(jsonAnswer) {}
                Answer() = delete;
                const json& at(const char name[]) const {
                    return resultObject.at(name);
                }
                template<typename T>
                void get_to(const char name[], const T& value) const {
                    return resultObject.at(name).get_to(value);
                }
                std::string errorMessage;
                int16 errorCode;
                json resultObject;
                explicit operator bool() const;
                operator int() const;
                bool operator!() const;
                friend void to_json(json& j, const Answer& o);
                friend void from_json(const json& j, Answer& o);
            };
            struct Api {
                static std::string ApiUrl;
            public:
                Api(const std::string& reqPage = "");

                template <typename T> inline void Append(const T& param);
                template <typename T> inline void Append(const T* param);

                template<typename T>
                inline Api& operator<< (const T& param)
                {
                    Append(param);
                    return *this;
                }

                Answer RequestApi() const;
                Answer RequestPost(const json& value) const;
                std::string BuildUrl() const;

                template<typename F>
                inline void GetAsync(const F& callback) const
                {
                    cpr::GetCallback(
                        [callback](const cpr::Response& r) {
                            Logger::GetInstance().Info("Api::GetAsync => status_code = {0}, text = {1}", r.status_code, r.text);

                            Answer answ("Whoops!", -1);

                            if (r.status_code == 200)
                                try
                            {
                                json result = json::parse(r.text);
                                if (result != NULL && result.is_object())
                                    answ = Answer(result["errorMessage"].get<std::string>(), result["errorCode"].get<int16>(), result["data"].get<json>());
                            }
                            catch (...) {
                                Logger::GetInstance().Error("Could not parse json!");
                            }

                            callback(answ);

                        }, cpr::Url(BuildUrl()), m_Header, cpr::Timeout{ m_TimeOut });
                }
                void GetAsync() const;
                template<typename F>
                inline void PostAsync(const F& callback, const json& value) const
                {
                    auto cb = cpr::PostCallback(
                        [callback](const cpr::Response& r) {
                            Logger::GetInstance().Info("Api::PostAsync => status_code = {0}, text = {1}", r.status_code, r.text);

                            Answer answ("Whoops!", -1);

                            if (r.status_code == 200)
                                try
                            {
                                json result = json::parse(r.text);
                                if (result != NULL && result.is_object())
                                    answ = Answer(result["errorMessage"].get<std::string>(), result["errorCode"].get<int16>(), result["data"].get<json>());
                            }
                            catch (...) {
                                Logger::GetInstance().Error("Could not parse json!");
                            }

                            callback(answ);

                        }, cpr::Url(BuildUrl()), cpr::Body{ value.dump() }, m_Header, cpr::Timeout{ m_TimeOut });
                }
                void PostAsync(const json& value) const;
                template<typename O, typename T>
                inline void GetAsync(O* obj, void (O::* function)(T&, Answer&), T& param) {
                    cpr::GetCallback(
                        [&](const cpr::Response& r) {
                            Logger::GetInstance().Info("Api::GetAsync => status_code = {0}, text = {1}", r.status_code, r.text);
                            Answer answer("Whoops!", -1);
                            if (r.status_code == 200) {
                                json result = json::parse(r.text);
                                result.get_to(answer);
                            }
                            (obj->*function)(param, answer);
                        }, cpr::Url(BuildUrl()), m_Header, cpr::Timeout{ m_TimeOut });
                }
                ~Api() {}
                void AddHeaders();
            private:
                cpr::Header                                     m_Header;
                std::vector<std::string>                        m_Url;
                uint16                                          m_TimeOut = 1500;
            };
        }
    }
}
#endif // !Api_GUARD