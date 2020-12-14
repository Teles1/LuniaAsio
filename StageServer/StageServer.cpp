#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <Network/Api/Json.hpp>
#include <Core/GameConstants.h>
#pragma warning(disable : 4996)
#include <cpr/cpr.h>
#include <Core/Utils/ThreadPool.h>
#include <chrono>
#include <Info/Info.h>
using namespace Lunia;
std::set<int> players;



std::mutex mtx;

std::condition_variable cv;
std::mutex cv_lock;
uint32 timeout = 2000;
bool loop = true;
void Test(int threadId) {
    while (loop) {
        if ( players.size() > 0 ) {
            AutoLock _l(mtx);
            std::cout << "Mutex Locked" << std::endl;
            for (auto x : players)
                std::cout << "player [" << x << "] pinged" << std::endl;
            std::cout << "Mutex Destroyed" << std::endl;
        }
        std::unique_lock<std::mutex> _l(cv_lock);
        if (cv.wait_for(_l, std::chrono::milliseconds(timeout), [] {return !loop; })) {
            std::cout << "Loop has been set to false" << std::endl;
        }else
            std::cout << "timed out" << std::endl;
    }
}

void Populate(int size) {
    for (int i = 0; i < size; i++)
        players.emplace(Math::Random());
    
}

int main()
{
    /*
    Utils::thread_pool::GetInstance().push(Test);
    Sleep(5000);
    Populate(10);
    Sleep(5000);
    players.clear();
    Sleep(5000);
    Populate(10);
    Sleep(6000);
    loop = false;*/
    Lunia::XRated::Database::DatabaseInstance();
    return 0;
}