#include <boost/asio.hpp>
#include <iostream>
#include <thread>

#include <syncstream>
namespace net = boost::asio;

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

/*
 * Функция post (англ. отправить) помещает функцию, переданную ей в качестве аргумента, в очередь для асинхронного исполнения.
 * Гарантируется, что переданная функция не будет выполнена на текущем потоке до возврата из post
 */
int main() {
    using osync = std::osyncstream;

    net::io_context io{2};
    auto strand = net::make_strand(io);

    net::post(strand, [] { osync(std::cout) << "Cat, "; });// Сat ВСЕГДА будет перед Fish

    net::post(strand, [] { osync(std::cout) << "Fish, "; });

    net::post(io, [] { osync(std::cout) << "Dog, "; }); // Если Dog Первый попадет в Очередь , выведется он, если Cat, то Cat. Fish перваым никогда не будет

    RunWorkers(2, [&io] {
        io.run();
    });

    std::cout << std::endl;
}