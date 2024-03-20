#pragma once
#include "audio.h"
#include "udp_base.h"
#include <boost/asio.hpp>
#include <string>
#include <memory>

namespace UDP
{
    class Client : public Base
    {
    private:
        std::shared_ptr<Recorder> _recorder;
    public:
      Client(const std::string& host, size_t port, const std::shared_ptr<Recorder>& recorder ): Base(host,port),
      _recorder(recorder){};
      Client(size_t port) : Client("127.0.0.1", port, std::make_shared<Recorder>(ma_format_u8, 1)){};

      [[noreturn]] void start(const std::string& remote_host, size_t port)
      {
          try
          {
              boost::system::error_code ec;
              auto endpoint = udp::endpoint(net::ip::make_address(remote_host, ec), port);
              for(;;)
              {
                  std::cout << "Press Enter to record message..." << std::endl;
                  std::string  str;
                  std::getline(std::cin, str);

                  auto rec_result = _recorder->Record(65000, 1.5s);
                  std::cout << "Recording " << rec_result.data.size() << "bytes " << rec_result.frames << " frames" << std::endl;
                  _socket_ptr->send_to(net::buffer(rec_result.data), endpoint);

                 // auto ptr = reinterpret_cast<char*>(&rec_result.frames);
                  std::cout << "Send to server " << rec_result.frames<<  " frames" <<std::endl;


              }

          } catch (std::exception& e) {
              std::cerr << e.what() << std::endl;
          }
      }

    };
}