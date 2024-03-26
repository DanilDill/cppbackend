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
int main() {
    using osync = std::osyncstream;

    net::io_context io;
    auto strand = net::make_strand(io);

    net::post(strand, [strand] {
        net::post(strand, [] {
            osync(std::cout) << 'A';
        });
        net::dispatch(strand, [] {
            osync(std::cout) << 'B';
        });
        osync(std::cout) << 'C';
    });

    net::post(io, [&io] {
        net::post(io, [] {
            osync(std::cout) << 'D';
        });
        net::dispatch(io, [] {
            osync(std::cout) << 'E';
        });
        osync(std::cout) << 'F';
    });

    RunWorkers(2, [&io] {
        io.run();
    });
}