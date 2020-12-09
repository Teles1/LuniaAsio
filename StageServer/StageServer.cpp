#include "../Core/Core.h"
#include <iostream>
#include <strstream>
#include <chrono>
#include "./Network/Api/Json.hpp"
#include <cpr/cpr.h>

// for convenience
using json = nlohmann::json;
class later
{
public:
    template <class callable, class... arguments>
    later(int after, bool async, callable&& f, arguments&&... args)
    {
        std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));

        if (async)
        {
            std::thread([after, task]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(after));
                task();
                }).detach();
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(after));
            task();
        }
    }

};
void test1(std::string &a)
{
    a = "alpha";
    return;
}

void test2(int a)
{
    printf("%i\n", a);
    return;
}

json Send() {
    cpr::Header m_Header;

    m_Header.emplace("ServerName", "Lobby");
    m_Header.emplace("ServerIp", "127.0.0.1");
    m_Header.emplace("ContentType", "application/json");

    cpr::Response r = cpr::Get(cpr::Url("http://127.0.0.1:51542/Lobby/CheckAccount/Teles|DA6EFBC18629FFC57A4273A40D5092C063A40F78|184.146.62.104"), m_Header, cpr::Timeout{ 1000 });
    Logger::GetInstance()->Info("[{0}]{1}", r.status_code, r.text);
    if (r.status_code == 200) {
        try {
            return json::parse(r.text);
        }
        catch (...) {
            Logger::GetInstance()->Error("Could not parse json!");
        }
    }
    return NULL;
}
int main()
{
    auto x = Send();
    try {
        if (x != NULL && x.is_object()) {
            uint16 aux = x["errorCode"].get<uint16>();
            std::string auxString = x["errorMessage"].get<std::string>();
            Logger::GetInstance()->Info("fella {0} da {1} ", auxString, aux);
        }
    }
    catch (...) {
        Logger::GetInstance()->Error("Error parsing jason");
    }
    return 0;
}