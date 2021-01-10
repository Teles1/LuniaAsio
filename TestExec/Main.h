#pragma once
#include <iostream>
#pragma warning(disable : 4996)
#include <cpr/cpr.h>
#include <optional>
#include <Core/Utils/ThreadPool.h>
#include <Network/Api/Api.h>
using namespace Lunia::XRated;
//struct Answer {
//	std::string content;
//public:
//	Answer(const std::string& in) : content(in) {
//
//	}
//};
struct User {
	int myPrecious = 0;
public:
	User(int in) {
		myPrecious = in;
	}
};
struct Server {
	std::mutex mtx;
public:
	void Authed(const User& user, const Net::Answer& answer);
	void CallDb();
};
struct Guild {
public:
	void Authed(const User& user, const Net::Answer& answer){}
};
//struct Api {
//	cpr::Header m_Header;
//	std::string url = "http://localhost:51542/Square/Family/JoinedInfo/1";
//public:
//	Api() {
//
//		m_Header.emplace("ServerName", "LobbyDev");
//		m_Header.emplace("ServerIp", "");
//		m_Header.emplace("Content-Type", "application/json");
//	}
//	void Request(const cpr::Url& url, const cpr::Header& header) {
//	};
//	template<typename T>
//	void Test(T& obj, void(T::* function)(User&, const Answer&), User& user) {
//		for (int i = 0; i < 100; i++)
//			thread_pool::GetInstance().push(
//				[&](const int& id) {
//					auto r = cpr::Get(cpr::Url(url), m_Header, cpr::Timeout(1000));
//					if(&obj != nullptr)
//						((&obj)->*function)(user, Answer(r.text));
//				});
//	}
//};
