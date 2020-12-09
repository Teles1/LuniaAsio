#ifndef Api_GUARD
#define Api_GUARD

#pragma once
#include "./Core/Core.h"
#include <cpr/cpr.h>
#include "Json.hpp"
// for convenience
using json = nlohmann::json;
struct Api {
    const std::string ApiUrl = "http://127.0.0.1:51542/Lobby";
public:
    Api(const char* reqPage = {}) {
        if (ApiUrl.size() == 0) {
            Logger::GetInstance()->Error("The api url shouldn't be empty!");
            throw "The api url shouldn't be empty!";
        }
        if (reqPage == NULL)
            return;
        std::string aux(reqPage);
        if (aux[aux.size() - 1] != '/')
            aux.append("/");
        m_Url.push_back(std::move(aux));
        AddHeaders();
    }
    template<typename T>
    Api& operator<< (T& a) {
        Append(a);
        return *this;
    }
    void Append(const std::string& in) {
        m_Url.push_back(in);
    }
    void Append(std::string& in) {
        m_Url.push_back(in);
    }
    void Append(const std::wstring& in) {
        using convert_type = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_type, wchar_t> converter;
        Append(std::move(converter.to_bytes(in)));
    }
    json Send() {
        cpr::Response r = cpr::Get(cpr::Url(BuildUrl()), m_Header, cpr::Timeout{ 1000 });
        Logger::GetInstance()->Info("[{0}]{1}", r.status_code, r.text);
        if (r.status_code == 200) {
            try {
                return json::parse(r.text);
            }
            catch (json::parse_error& e) {
                Logger::GetInstance()->Error("Could not parse json! {0}", e.what());
            }
        }
    }
    std::string BuildUrl() {
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
    }
private:
    cpr::Header                                     m_Header;
    std::vector<std::string>                        m_Url;
};

#endif // !Api_GUARD