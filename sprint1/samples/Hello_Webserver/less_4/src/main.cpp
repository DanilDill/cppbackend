#ifdef WIN32
#include <sdkddkver.h>
#endif

#include <iostream>
#include <thread>
#include <boost/asio.hpp>

#include "Robot.h"
namespace net = boost::asio;
namespace sys = boost::system;
using namespace std::chrono;
using namespace std::literals;


void RunRobots(net::io_context& io)
{
    Robot r1(io, 1);
    Robot r2(io, 2);

    r1.Rotate(60, [&r1] {
        r1.Walk(4, [] {});
    });
    r2.Walk(2, [&r2] {
        r2.Walk(3, [] {});
    });
}


int main()
{
    net::io_context io;
    RunRobots(io);
    for (;;) {
        try {
            io.run();
            break;
        } catch (const std::exception& e) {
            std::cout << e.what() << '\n';
        }
    }
    std::cout << "Done\n"sv;
    return 0;
}


