#include <thread>
#include <vector>
#include <boost/asio.hpp>
#include "Restaurant.h"
namespace net = boost::asio;





// Запускает функцию fn на n потоках, включая текущий
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
    const unsigned num_workers = 4;
    // Сообщаем io_context о количестве потоков, которые будут одновременно вызывать метод run
    net::io_context io(num_workers);

    Restaurant restaurant{io};

    Logger logger{"main"s};
    auto print_result = [&logger](sys::error_code ec, int order_id, Hamburger* hamburger) {
        std::ostringstream os;
        if (ec) {
            os << "Order "sv << order_id << "failed: "sv << ec.what();
            return;
        }
        os << "Order "sv << order_id << " is ready. "sv << *hamburger;
        logger.LogMessage(os.str());
    };




    // Запускаем io.run() на num_workers потоках
    RunWorkers(num_workers, [&io,&restaurant,print_result]
    {
        restaurant.MakeHamburger(true, print_result);
        io.run();
    });
}