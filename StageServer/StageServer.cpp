#include "../Core/Core.h"
#include <iostream>
#include <chrono>
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
struct Tester {
public:
    Tester(std::vector<uint8>& in) {
        buffer = std::make_shared < std::vector<uint8> >(std::move(in));
        (*buffer)[0] = 30;
    }
    std::shared_ptr<std::vector<uint8>>& Test(std::shared_ptr<std::vector<uint8>>& bufferin) {
        return bufferin;
    }
    std::vector<uint8>& GetData() {
        return *buffer;
    }
    void Print() {
        auto x = (*buffer)[0];
        std::cout << std::to_string(x).c_str() << std::endl;
    }
private:
    std::shared_ptr<std::vector<uint8>> buffer;
};
struct Server;
struct User : std::enable_shared_from_this<User> {
public:
    User(std::shared_ptr<Server> server) {
        id = 0;
        std::cout << "User Created!" << std::endl;
    }
    User() {
        std::cout << "User Created!" << std::endl;
    }
    int& GetId() { return id; }
    void DoWork(int i) {
    }
public:
    void print(int i) {
        id = i;
    }
    ~User(){
        std::cout << "User destroyed" << std::endl;
    }
private:
    int id;
};
typedef std::shared_ptr<User> UserPtr;

struct Server {
public:
    Server (){}
    void AddUsr(User* newUsr)
    {
        _connections.push_back(std::shared_ptr<User>(newUsr));
    }
    auto GetUser(uint16 i) {
        //return _connections.at(i)->weak_from_this();
    }
    ~Server() {
        UserPtr x = _connections[0];
        std::cout << "server destroyed" << std::endl;
    }
    void Changedata(int& i) { user = i; }
private:
    std::vector < UserPtr> _connections;
    uint16 user = 11;
};

typedef std::shared_ptr<Server> ServerPtr;
int main()
{	
    ServerPtr x = std::shared_ptr<Server>( new Server);






    x->AddUsr( new User );




    system("pause");
    return 0;
}