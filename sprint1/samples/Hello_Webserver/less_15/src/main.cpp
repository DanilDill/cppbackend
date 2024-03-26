#include <boost/asio.hpp>
#include <iostream>
#include <iomanip>
#include <thread>

#include <syncstream>
namespace net = boost::asio;

/*
 * Функция boost::asio::defer, как и post, помещает функцию в очередь для асинхронного выполнения.
 * post информирует другие потоки о появлении функции в очереди, чтобы первый освободившийся поток начал её выполнять.
 * defer этого не делает, если вызывающий код уже выполняется контекстом — например, вызван из метода io_context::run.
 *
 * Если асинхронная функция продолжает текущую, например, работает с теми же данными, отложенный вызов может сделать использование кэша эффективнее.
 * Если асинхронная функция не продолжает вызывающую,
 * использование defer вместо post может привести к тому, что функция будет вызвана позже, чем могла бы.
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
    using namespace std::chrono;

    // Создаём пул, содержащий два потока
    net::io_context io{2};
    const auto start = steady_clock::now();

    auto print = [start](char ch) {
        const auto t = duration_cast<duration<double>>(steady_clock::now() - start).count();
        osync(std::cout) << std::fixed << std::setprecision(6)
                         << t << "> " << ch << ':' << std::this_thread::get_id() << std::endl;
    };

    net::post(io, [&io, print] {
        print('A');

        net::defer(io, [print] { print('B'); });
        net::defer(io, [print] { print('C'); });
        net::post(io, [print] { print('D'); });

        // Засыпаем, чтобы дать шанс другим потокам сделать свою работу
        std::this_thread::sleep_for(std::chrono::seconds(1));
    });

    // Дожидаемся окончания работы потоков
    RunWorkers(2, [&io]{ io.run(); });
}