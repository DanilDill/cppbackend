#pragma once
#include <boost/asio.hpp>
#include <string>
#include <memory>

namespace UDP
{
    namespace net = boost::asio;
    using net::ip::udp;
    using namespace std::literals;
    const size_t max_buffer_size = 65000;
    class Base
    {
    protected:
        net::io_context _context;
        size_t _port;
        std::string _host;
        std::shared_ptr<udp::socket> _socket_ptr;

    public:
        Base(const std::string& host , size_t port): _host(host), _port(port),_context()
        {
            _socket_ptr = std::make_shared<udp::socket>(_context, udp::endpoint(net::ip::make_address(_host), _port));
        }
    };

}