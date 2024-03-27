#include <boost/asio.hpp>
#include <iostream>
#include <iomanip>
#include <thread>

#include <syncstream>
namespace net = boost::asio;

/*
 Третий способ выполнить функцию асинхронно — функция boost::asio::dispatch. post и defer всегда вызывают функцию асинхронно.
 dispatch работает иначе: он выполняет функцию синхронно, если вы вызываете dispatch внутри этого же исполнителя.
 */


template <typename Fn>
void RunWorkers(unsigned n, const Fn& fn) {
    n = std::max(1u, n);
    std::vector<std::jthread> workers;
    workers.reserve(n - 1);
    // Запускаем n-1 рабочих потоков, выполняющих функцию fn
    while (--n) {
        workers.emplace_back(fn);
    }
    fn();
}

int main()
{
    using osync = std::osyncstream;

    net::io_context io;
    auto strand1 = net::make_strand(io);

    net::post(strand1, [strand1] {  // (1)
        net::post(strand1, [] {     // (2)
            osync(std::cout) << 'A';
        });
        net::dispatch(strand1, [] {  // (3)
            osync(std::cout) << 'B';
        });
        osync(std::cout) << 'C';
    });

    auto strand2 = net::make_strand(io);
    // Эти функции выполняются в strand2
    net::post(strand2, [strand2] {  // (4)
        net::post(strand2, [] {     // (5)
            osync(std::cout) << 'D';
        });
        net::dispatch(strand2, [] {  // (6)
            osync(std::cout) << 'E';
        });
        osync(std::cout) << 'F';
    });

    RunWorkers(2, [&io] {
        io.run();
    });
}