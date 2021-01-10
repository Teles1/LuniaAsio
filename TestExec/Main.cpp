#include <iostream>
#include "Main.h"
#include <Core/Utils/ConfigReader.h>
User user(10);
void Server::Authed(const User& user, const Net::Answer& answer)
{
	mtx.lock();
	printf("Ohai %s", answer.errorMessage.c_str());
	mtx.unlock();
}

void Server::CallDb()
{
	mtx.lock();
	Net::Api request("Family/JoinedInfo");
	request << 1;
	request.GetAsync(this, &Server::Authed, user);
	mtx.unlock();
}

int main(int argc, char* argv[]) {
	Lunia::Logger::GetInstance("StageServer");
	Lunia::Config::GetInstance("Config_Stage.json");
	Net::Api::ApiUrl = Lunia::Config::GetInstance().Settings.ApiUrl;
	Server server;
	server.CallDb();
	std::thread t2(
		[]() {
			Sleep(80000);
		});
	int _Hello = 0;
	t2.join();
	return 0;
}