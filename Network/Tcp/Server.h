#ifndef ServerTcp_GUARD
#define ServerTcp_GUARD
#pragma once
#include "Shared.h"
#include "Client.h"

namespace net {
    template<class T>
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

        /*
            We should add some sort of filtering here. I'm not really sure what but for sure something.
        */
        void NewConnection(const error_code& err_code) {
            HandleNewConnection(err_code, m_socket);
            m_acceptor.async_accept(m_socket, std::bind(&ServerTcp::NewConnection, this, std::placeholders::_1));
        }
    protected:
        virtual void HandleNewConnection(const error_code& err_code, tcp::socket& socket) = 0;
    private:
        void HandleAcceptNextSocketConnection() {
            m_acceptor.async_accept(m_socket, std::bind(&ServerTcp::NewConnection, this, std::placeholders::_1));
        }

        asio::io_service                        m_ioservice;
        tcp::acceptor                           m_acceptor;
        tcp::socket                             m_socket;
        uint32                                  m_userId;
    protected:
        std::unordered_map<uint32, TemplTypeSharedPtr>    _connections;
    };
}
#endif //Server_GUARD