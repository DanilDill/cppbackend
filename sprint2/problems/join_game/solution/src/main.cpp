#include "sdk.h"
//
#include <boost/asio/signal_set.hpp>
#include <boost/asio/io_context.hpp>
#include <iostream>
#include <thread>

#include "json_loader.h"
#include "request_handler.h"
#include "logging_request_handler.h"
#include "staticfile_loader.h"
using namespace std::literals;
namespace net = boost::asio;

namespace {

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

}  // namespace

int main(int argc, const char* argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: game_server <game-config-json> <wwwroot_dir>"sv << std::endl;
        return EXIT_FAILURE;

    }
    try {
        // 1. Загружаем карту из файла и построить модель игры
        model::Game game = json_loader::LoadGame(argv[1]);
        file::file_loader wwwroot(argv[2]);
     // Создаём обработчик HTTP-запросов и связываем его с моделью игры
        http_handler::RequestHandler handler{game,wwwroot};
        LoggingRequestHandler<http_handler::RequestHandler> log_handler{handler};
        // 2. Инициализируем io_context
        const unsigned num_threads = std::thread::hardware_concurrency();
        net::io_context ioc(num_threads);
        net::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc,&log_handler](const boost::system::error_code& ec, [[maybe_unused]] int signal_number) {
            if (!ec) {
                log_handler.Log(boost::log::trivial::info,{{"code",0}},"server exited");
                ioc.stop();
            }
        });
        // 3. Добавляем асинхронный обработчик сигналов SIGINT и SIGTERM


        // 5. Запустить обработчик HTTP-запросов, делегируя их обработчику запросов
        const auto address = net::ip::make_address("0.0.0.0");
        constexpr net::ip::port_type port = 8080;
        http_server::ServeHttp(ioc, {address, port}, [&log_handler](auto&& req,auto&& send)
        {
            log_handler(std::forward<decltype(req)>(req), std::forward<decltype(send)>(send));
        });



        // Эта надпись сообщает тестам о том, что сервер запущен и готов обрабатывать запросы
       // std::cout << "Server has started..."sv << std::endl;
       log_handler.Log(boost::log::trivial::info,{{"port",port},{"address",address.to_string()},},"server started");

        // 6. Запускаем обработку асинхронных операций
        RunWorkers(std::max(1u, num_threads), [&ioc] {
            ioc.run();
        });
    } catch (const std::exception& ex) {

        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
}
