#ifndef Api_GUARD
#define Api_GUARD
#pragma warning(disable : 4996)
#pragma once
#include <cpr/cpr.h>
#include "Json.hpp"
#include <Core/Utils/ThreadPool.h>
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
                void get_to(const char name[], T& value) const {
                    resultObject.at(name).get_to(value);
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
                enum class Methods {
                    GET,
                    POST
                };
                Api(const std::string& reqPage = "", const Methods& method = Methods::GET);

                template <typename T> inline void Append(const T& param);
                template <typename T> inline void Append(const T* param);

                template<typename T>
                inline Api& operator<< (const T& param)
                {
                    Append(param);
                    return *this;
                }

                std::string BuildUrl() const;
                Answer RequestApi() const;
                Answer RequestPost(const json& value) const;
                void GetAsync(const json& payLoad = json()) const;
                template<typename O, typename T>
                inline void GetAsync(O* inObj, void (O::* inFunction)(const T&, const Answer&), const T& inParam, const json& inPayload = json()) {
                    Utils::thread_pool::GetInstance().push(
                        [function = inFunction, &param = inParam, obj = inObj]
                        (const int& id, const Methods& method, const cpr::Url& url, const cpr::Header& header, const cpr::Timeout& timeout, const json& payload) {
                            cpr::Response r;
                            if (method == Methods::GET)  r = cpr::Get(url, header, timeout);
                            else r = cpr::Post(url, header, cpr::Body{ payload.dump() }, timeout);
                            if (obj != nullptr) {
                                Logger::GetInstance().Info("Api::GetAsync({0}) => status_code = {1}, text = {2}",id , r.status_code, r.text);
                                Answer answer("Whoops!", -1);
                                if (r.status_code == 200) {
                                    json::parse(r.text).get_to(answer);
                                }
                                (obj->*function)(param, answer);
                            }
                        }, m_Method, cpr::Url{ BuildUrl() }, m_Header, cpr::Timeout(m_TimeOut), inPayload);
                }
                ~Api() {}
                void AddHeaders();
            private:
                cpr::Header                                     m_Header;
                std::vector<std::string>                        m_Url;
                uint16                                          m_TimeOut = 5000;
                Methods                                         m_Method = Methods::GET;
            };
        }
    }
}
#endif // !Api_GUARD