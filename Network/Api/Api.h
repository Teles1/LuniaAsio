#ifndef Api_GUARD
#define Api_GUARD

#pragma once
#include "./Core/Core.h"
#include <cpr/cpr.h>
#include "Json.hpp"
#include <string>
// for convenience
using json = nlohmann::json;
struct Answer {
    Answer(const std::string& errorMessage, const uint16& errorCode)
    :   errorMessage(errorMessage),errorCode(errorCode) {}
    std::string errorMessage;
    uint16 errorCode;
    
    //json object? who knows
};
struct Api {
    static std::string ApiUrl;
public:
    Api(const std::string& reqPage = "") {
        std::string aux = reqPage; // I need it to be a non const lol
        if (aux == "") {
            Logger::GetInstance()->Error("The requested Procedure shouldn't be empty!");
            throw "The requested Procedure shouldn't be empty!";
        }
        if (ApiUrl.size() == 0) {
            Logger::GetInstance()->Error("The api url shouldn't be empty!");
            throw "The api url shouldn't be empty!";
        }
        if (aux[aux.size() - 1] != '/')
            aux.push_back('/');
        m_Url.push_back(std::move(aux)); //getting rid of aux and pushing it inside m_url
        AddHeaders();
    }
    template<typename T>
    Api& operator<< (T& a) {
        Append(a);
        return *this;
    }

#pragma region BasicConversion
    void Append(const std::string& in) {
        m_Url.push_back(in);
    }
    void Append(std::string& in) {
        m_Url.push_back(in);
    }
    void Append(const double& in) {
        m_Url.push_back(std::move(std::to_string(in)));
    }
    void Append(const float& in) {
        m_Url.push_back(std::move(std::to_string(in)));
    }
    void Append(const int64& in) {
        m_Url.push_back(std::move(std::to_string(in)));
    }
    void Append(const int32& in) {
        m_Url.push_back(std::move(std::to_string(in)));
    }
    void Append(const int16& in) {
        m_Url.push_back(std::move(std::to_string(in)));
    }
    void Append(const int8& in) {
        m_Url.push_back(std::move(std::to_string(in)));
    }
    void Append(const uint64& in) {
        m_Url.push_back(std::move(std::to_string(in)));
    }
    void Append(const uint32& in) {
        m_Url.push_back(std::move(std::to_string(in)));
    }
    void Append(const uint16& in) {
        m_Url.push_back(std::move(std::to_string(in)));
    }
    void Append(const uint8& in) {
        m_Url.push_back(std::move(std::to_string(in)));
    }
    void Append(const std::wstring& in) {
        using convert_type = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_type, wchar_t> converter;
        Append(std::move(converter.to_bytes(in)));
    }
#pragma endregion

    const Answer RequestApi() {
        cpr::Response r = cpr::Get(cpr::Url(BuildUrl()), m_Header, cpr::Timeout{ 1000 });
        Logger::GetInstance()->Info("[{0}]{1}", r.status_code, r.text);
        if (r.status_code == 200) {
            try {
                json result = json::parse(r.text);
                if (result != NULL && result.is_object()) {
                    uint16 errorCode = result["errorCode"].get<uint16>();
                    std::string errorMessage = result["errorMessage"].get<std::string>();
                    //get objct
                    return Answer(errorMessage, errorCode);
                }
            }
            catch (...) {
                Logger::GetInstance()->Error("Could not parse json!");
            }
        }
        return Answer("Whoops!", -1);
    }
    const std::string BuildUrl() {
        std::string ret;
        for (size_t i = 0; i < m_Url.size(); i++) {
            if (i > 1)
                ret.append("|");
            ret.append(m_Url[i]);
        }
        return ApiUrl + "/" + ret;
    }
    ~Api() {}
    void AddHeaders() {
        m_Header.emplace("ServerName", "Lobby");
        m_Header.emplace("ServerIp", "127.0.0.1");
        m_Header.emplace("ContentType", "application/json");
    }
private:
    cpr::Header                                     m_Header;
    std::vector<std::string>                        m_Url;
};

#endif // !Api_GUARD