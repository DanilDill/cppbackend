#pragma once
#include "tcp_instance.h"
namespace TCP
{
    namespace SeaBattle
    {
        class Server : public TCPInstance
        {
        private:
            tcp::acceptor _acceptor;
        public:
            Server(const std::string& host, size_t port) : TCPInstance(host,port),_acceptor(_context)
            {};
            template <typename Callback>
            void start(Callback&& game_cb)
            {

                boost::asio::ip::tcp::endpoint endpoint(net::ip::make_address(_host), _port);
                _acceptor.open(endpoint.protocol());
                _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
                _acceptor.bind(endpoint);
                _acceptor.listen(2);
                std::cout << "Waiting for connection on "sv << _host << ":" << _port  << std::endl;
                _acceptor.accept(*_socket, _ec);
                if (_ec)
                {
                    throw std::runtime_error(_ec.what());
                }
            }
        };
    }
}