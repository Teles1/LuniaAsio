#include <ctime>
#include <iostream>
#include <string>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include <json/reader.h>
#include <json/writer.h>

namespace {
    Json::Value to_json(std::string json)
    {
        Json::Value root;
        Json::Reader reader;
        /*bool success =*/ reader.parse(json, root);
        return root;
    }

    std::string to_string(Json::Value root) // unused TODO FIXME
    {
        Json::FastWriter writer;
        std::string json = writer.write(root);
        return json;
    }
}

using boost::asio::ip::tcp;

class tcp_connection : public boost::enable_shared_from_this<tcp_connection>
{
public:
    typedef boost::shared_ptr<tcp_connection> pointer;

    static pointer create(boost::asio::io_service& io_service)
    {
        return pointer(new tcp_connection(io_service));
    }

    tcp::socket& socket()
    {
        return socket_;
    }

    void start()
    {
        //Read from client, make json and send appropriate response
        boost::asio::async_read(socket_, message_,
            boost::bind(&tcp_connection::handle_read, shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

private:
    tcp_connection(boost::asio::io_service& io_service)
        : socket_(io_service)
    {
    }

    void handle_write(const boost::system::error_code& /*error*/,
        size_t /*bytes_transferred*/)
    {
    }

    void handle_read(const boost::system::error_code& error, size_t bytes_transferred)
    {
        std::cout << "Handle Read of connection\n";

        if (error && error != boost::asio::error::eof) {
            std::cout << "Error: " << error.message() << "\n";
            return;
        }

        std::string messageP;
        {
            std::stringstream ss;
            ss << &message_;
            ss.flush();
            messageP = ss.str();
        }

        std::cout << messageP << std::endl;

        Json::Value root = to_json(messageP);
        std::string isHello = root["hello"].asString();
        std::string isMessage = root["message"].asString();
        if (!isHello.empty())
        {
            std::string messageTemp = "{\"Hello\":\"Server\"}";
            boost::asio::async_write(socket_, boost::asio::buffer(messageTemp),
                boost::bind(&tcp_connection::handle_write, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }

        if (!isMessage.empty())
        {
            std::string messageTemp = "{\"response\":\"Fine\"}";
            boost::asio::async_write(socket_, boost::asio::buffer(messageTemp),
                boost::bind(&tcp_connection::handle_write, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
    }

    tcp::socket socket_;
    boost::asio::streambuf message_;
};

class tcp_server
{
public:
    tcp_server(boost::asio::io_service& io_service)
        : acceptor_(io_service, tcp::endpoint(tcp::v4(), 1936))
    {
        start_accept();
    }

private:
    void start_accept()
    {
        tcp_connection::pointer new_connection =
            tcp_connection::create(acceptor_.get_io_service());

        acceptor_.async_accept(new_connection->socket(),
            boost::bind(&tcp_server::handle_accept, this, new_connection,
                boost::asio::placeholders::error));
    }

    void handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& error)
    {
        if (!error)
        {
            std::cout << "A client connected" << std::endl;
            new_connection->start();
        }

        start_accept();
    }

    tcp::acceptor acceptor_;
};

int main()
{
    try
    {
        boost::asio::io_service io_service;
        tcp_server server(io_service);
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}