#include "sdk.h"
//
#include <boost/program_options.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/io_context.hpp>
#include <iostream>
#include <thread>

#include "http_server.h"
#include "json_loader.h"
#include "request_handler.h"
#include "logging_request_handler.h"
#include "staticfile_loader.h"
using namespace std::literals;
namespace net = boost::asio;
struct Args {
    std::string config_file;
    std::string wwwroot;
    size_t tick_period;
    bool randomize_spawn_points;
};

[[nodiscard]] std::optional<Args> ParseCommandLine(int argc, const char* const argv[])
{
   namespace po = boost::program_options;
    po::options_description desc{"All options"s};
    Args args;
    desc.add_options()
    ("help,h", "Show help")
    ("config-file,c", po::value(&args.config_file)->value_name("file"s), "set config file path")
    ("www-root,w",po::value(&args.wwwroot)->value_name("dir"),"set static files root")
    ("tick-period,t",po::value(&args.tick_period)->value_name("milliseconds"s),"set tick period")
    ("randomize-spawn-points", po::value(&args.randomize_spawn_points),"spawn dogs at random positions ");
     po::variables_map vm;
     po::store(po::parse_command_line(argc, argv, desc), vm);
     po::notify(vm);

     if (vm.contains("help"s)) {
         // Если был указан параметр --help, то выводим справку и возвращаем nullopt
         std::cout << desc;
         return std::nullopt;
     }
     if (!vm.contains("config-file"))
     {
         std::cout << desc << std::endl;
         throw std::runtime_error("config file was not found");
     }
     if (!vm.contains("www-root"))
     {
         throw std::runtime_error("wwwroot not specified");
     }
     args.randomize_spawn_points = vm.contains("randomize-spawn-points");
     args.tick_period = vm.contains("tick-period") ? args.tick_period  : 0;
     return args;
  //  return std::nullopt;
}


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
    try {
        if(auto args = ParseCommandLine(argc,argv))
        {
            // 1. Загружаем карту из файла и построить модель игры
            const unsigned num_threads = std::thread::hardware_concurrency();
            net::io_context ioc(num_threads);
            model::Game game = json_loader::LoadGame(args->config_file,ioc);
            file::file_loader wwwroot(args->wwwroot);

            if (args->tick_period != 0)
            {
               game.SetTicker(std::chrono::milliseconds(args->tick_period));
            }

            if (args->randomize_spawn_points)
            {
                game.SetRandomizedCoord();
            }
            // Создаём обработчик HTTP-запросов и связываем его с моделью игры
            http_handler::RequestHandler handler{game,wwwroot};
            LoggingRequestHandler<http_handler::RequestHandler> log_handler{handler};

            net::signal_set signals(ioc, SIGINT, SIGTERM);
            signals.async_wait([&ioc,&log_handler](const boost::system::error_code& ec, [[maybe_unused]] int signal_number) {
                if (!ec) {
                    log_handler.Log(boost::log::trivial::info,{{"code",0}},"server exited");
                    ioc.stop();
                }
            });
            const auto address = net::ip::make_address("0.0.0.0");
            constexpr net::ip::port_type port = 8080;
            http_server::ServeHttp(ioc, {address, port}, [&log_handler](auto&& req,auto&& send)
            {
                log_handler(std::forward<decltype(req)>(req), std::forward<decltype(send)>(send));
            });

            log_handler.Log(boost::log::trivial::info,{{"port",port},{"address",address.to_string()},},"server started");
            RunWorkers(std::max(1u, num_threads), [&ioc] {
                ioc.run();
            });
        }
        else
        {
            return 0;
        }
    } catch (const std::exception& ex) {

        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
}
