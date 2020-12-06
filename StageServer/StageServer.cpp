#include "../Core/Core.h"
#include <iostream>

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


int main()
{	
    std::vector<uint8> buffer;
    buffer.push_back(10);
    buffer.push_back(3);
    buffer.push_back(4);
    {
        Tester obj(buffer);
        obj.Print();
        obj.GetData()[0] = 45;
        obj.Print();
    } 
    std::cout << buffer[0] << std::endl;
    return 0;
}