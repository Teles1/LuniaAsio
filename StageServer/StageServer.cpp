#include "../Core/Core.h"
#include <iostream>
#include <strstream>
#include <chrono>
#include <cpr/cpr.h>
#include <spdlog/spdlog.h>
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
/*
struct Api {
public:
    Api(const char* reqPage) {
        std::string buf = "http://192.168.86.50:51542/Lobby/AddServer/10";
        if (buf[buf.size() - 1] != '/')
            buf.append("/");
        buf.append(reqPage);
        if (buf[buf.size() - 1] != '/')
            buf.append("/");
        m_request = new http::Request(std::move(buf));
        //AddHeader();
    }
    template<typename T>
    Api& operator<< (T& a) {
        Append(a);
        return *this;
    }
    void Append(const std::string& in) {
        m_body.push_back(in);
    }
    void Append(const std::wstring& in) {
        using convert_type = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_type, wchar_t> converter;
        Append(std::move(converter.to_bytes(in)));
    }
    void Send() {
        //try{
        std::string aux("aopa");
        auto response = m_request->send();
            INFO_LOG("[{0}] {1}", response.status, std::string( response.body.begin(), response.body.end() ));
        //}
        //catch (const std::exception& e)
        //{
            //WARN_LOG("Request failed => {0}", e.what());
        //}
    }
    const std::vector<uint8> BuildBody() {
        std::string ret;
        for (size_t i = 0; i < m_body.size(); i++) {
            if (i != 0)
                ret.append("|");
            ret.append(m_body[i]);
        }
        m_body.clear();
        return std::move(std::vector<uint8>(ret.begin(), ret.end()));
    }
    ~Api() {
        delete m_request;
    }
private:
    void AddHeader() {
        m_headers.push_back("ServerName: Lobby_DEV");
        m_headers.push_back("ServerIp: 127.0.0.1");
    }
private:
    std::vector<std::string>     m_body;
    std::vector<std::string>     m_headers;
    http::Request*               m_request;
};
*/
int main()
{
    //StringUtil::StringBuilder sb;
    //Api api("AddServer");
    //api << "1010";
    //api.Send();
    cpr::Response r = cpr::Get(cpr::Url{ "http://192.168.86.50:51542/Lobby/AddServer/10" });
    r.status_code;                  // 200
    r.header["content-type"];       // application/json; charset=utf-8
    r.text;                         // JSON text string
    return 0;
}