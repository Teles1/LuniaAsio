#include "stdafx.h"
#include "./Tcp/Server.h"
struct ClientTcp : std::enable_shared_from_this<ClientTcp> {
public:
    ClientTcp(int& inData) :
        data( std::move(inData) )
    {}
    void Send() {
        //Tcpshit
        std::cout << "Using tcp to send data" << std::endl;
    }
private:
    int data;
};
int main() {
    auto handler = [&](std::vector<uint8>& buffer) -> void{
        std::cout << "received!" << std::endl;
    };
    net::ServerTcp server("127.0.0.1", 15550);
    std::cout << "Initialization over" << std::endl;
    server.Run();
}