#pragma once
#include "./Shared.h"
#include "./Client.h"
namespace net {
    struct ServerTcp {
        ServerTcp( const char* ip, unsigned port) :
            _a(_svc, tcp::endpoint( tcp::v4(), port) )
            , _s(_svc)
        {
            do_accept();
        }
        void Run() {
            _svc.run();
        }
        void NewConnection(const error_code& ec) {
            std::cout << "Async_accept!" << std::endl;
            if (!ec) {
                //User pointer owned by Server. This pointer will be active untill the server gets destroyed or this pointer is deleted.
                std::make_shared<ClientTcp>(std::move(_s))->Start();
                do_accept();
                return;
            }
            assert(ec, "The connection was not succefull");
        }
        auto GetInstance() {
            return this;
        }
    private:
        void do_accept() {
            std::cout << "Server Listening Initialized" << std::endl;
            _a.async_accept(_s, std::bind(&ServerTcp::NewConnection, this, std::placeholders::_1));
        }

        ba::io_service                      _svc;
        tcp::acceptor                       _a;
        tcp::socket                         _s;
    };
}