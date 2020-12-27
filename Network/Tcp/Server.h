#ifndef ServerTcp_GUARD
#define ServerTcp_GUARD
#pragma once
#include "Shared.h"
#include "Client.h"
namespace Lunia {
    namespace Net {
        template<class T>
        struct ServerTcp
        {
            using TemplTypeSharedPtr = std::shared_ptr<T>;

            ServerTcp(const std::string ip, unsigned port) :
                m_acceptor(m_ioservice, tcp::endpoint(asio::ip::address_v4::any(), port)) // this ip is what asio will be listening for :')
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
}

#endif //Server_GUARD