
#include "audio.h"
#include "udp_base.h"
#include <iostream>
namespace UDP
{
    class Server : public Base
    {
    private:
        std::shared_ptr<Player> _player_ptr;
    public:
        Server(std::string host, size_t port, const std::shared_ptr<Player>& player_ptr): Base(host,port),
        _player_ptr(player_ptr)
        {};

        Server(size_t port) : Server("127.0.0.1", port, std::make_shared<Player>(ma_format_u8, 1)){};

        [[noreturn]] void  start()
        {
            try
            {

                std::cout << "UDP server start listening" << std::endl;
                udp::endpoint remote_endpoint;
                for (;;)
                {
                    std::array<char, max_buffer_size> audio_buf;
                    auto audio_size = _socket_ptr->receive_from(boost::asio::buffer(audio_buf), remote_endpoint);
                    std::cout << "Client send audio "sv <<  audio_size << "bytes" << std::endl;

                    _player_ptr->PlayBuffer(audio_buf.data(), audio_buf.size(), 1.5s);
                }
            } catch (std::exception& e) {
                std::cerr << e.what() << std::endl;
            }
        }

    };

}
