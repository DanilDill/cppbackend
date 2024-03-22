#include <boost/asio.hpp>
#include <memory>
#include <exception>
namespace TCP
{
    namespace net = boost::asio;
    using net::ip::tcp;
    using namespace std::literals;
    namespace SeaBattle
    {
        class TCPInstance
        {
        protected:
            net::io_context _context;
            std::string _host;
            size_t _port;
            std::unique_ptr<tcp::socket> _socket;
            boost::system::error_code _ec;
        public:
            TCPInstance(const std::string& host, const size_t port):
                    _host(host), _port(port)
            {
                _socket = std::make_unique<tcp::socket>(_context);
            }
            TCPInstance() : TCPInstance("127.0.0.1",3333){};
            void write(const std::string& message)
            {
                _socket->write_some(net::buffer(message), _ec);
                if (_ec)
                {
                    throw std::runtime_error(_ec.what());
                }
            };
            std::string read()
            {
                net::streambuf stream_buf;
                net::read_until(_socket, stream_buf, '\n', _ec);
                std::string data{std::istreambuf_iterator<char>(&stream_buf),
                                        std::istreambuf_iterator<char>()};
                if (_ec)
                {
                    throw std::runtime_error(_ec.what());
                }
                else
                {
                    return data;
                }
            };
        };
    }
}