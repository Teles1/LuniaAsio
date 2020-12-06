#pragma once
#include "Shared.h"

namespace net {
    const int READ_DATA_BUFFER_LENGTH = 1024;
    struct ClientTcp : std::enable_shared_from_this<ClientTcp> {
        ClientTcp(tcp::socket&& s)
            : _s(std::move(s)) //ClientTcp now Owns the socket.
            //, _onDataReceived(receivedCB)
            , bytes_read(0)
        {}
        ~ClientTcp() {
            std::cout << "User Destroyed" << std::endl;
        }
        void Disconect(){}
        void ReceivedData(const error_code& ec, size_t size) {
            printf("%zd %zd %zd\n", size, bytes_read, sizeof(_request) );
            //_onDataReceived(_request);
            if(ec != ba::error::eof)
                DoRead();
        }
        void Start() {
            std::cout << "New Client Initialized" << std::endl;
            DoRead();
            //tell the service to be ready to read data.
        }
    private:
        void DoRead() {
            /*
                The placeholders here are making sure that asio can call the function binded with (error_code, bytesread)
            */
            _s.async_read_some(ba::buffer(_request, READ_DATA_BUFFER_LENGTH),
                std::bind(&ClientTcp::ReceivedData, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
        }
    private:
        tcp::socket   _s;
        size_t bytes_read;
        uint8 _request[READ_DATA_BUFFER_LENGTH] = {};
    };
}
