#ifndef Server_GUARD
#define Server_GUARD
#pragma once
#include "./Shared.h"
#include "./Client.h"

namespace net {
    template<typename T>
    struct ServerTcp 
    {
        using TemplTypeSharedPtr = std::shared_ptr<T>;

        ServerTcp(const char* ip, unsigned port) :
            m_acceptor(m_ioservice, tcp::endpoint(tcp::v4(), port))
            , m_socket(m_ioservice)
            , m_userId(1000)
        {
            HandleAcceptNextSocketConnection();
        }

        void Run() {
            m_ioservice.run();
        }

        void NewConnection(const error_code& err_code) {
            std::cout << "Server Listening Initialized" << std::endl;
            m_acceptor.async_accept(m_socket, std::bind(&ServerTcp::NewConnection, this, std::placeholders::_1));
        }
    private:
        void HandleAcceptNextSocketConnection() {
            std::cout << "Server Listening Initialized" << std::endl;
            m_acceptor.async_accept(m_socket, std::bind(&ServerTcp::NewConnection, this, std::placeholders::_1));
        }

        ba::io_service                          m_ioservice;
        tcp::acceptor                           m_acceptor;
        tcp::socket                             m_socket;
        uint32                                  m_userId;
    protected:
        std::unordered_map<uint32, TemplTypeSharedPtr>    _connections;
    };
}
#endif //Server_GUARD