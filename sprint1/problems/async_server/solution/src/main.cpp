#include "sdk.h"
//
#include <boost/asio/signal_set.hpp>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "http_server.h"

namespace {
namespace net = boost::asio;
using namespace std::literals;
namespace sys = boost::system;
namespace http = boost::beast::http;

// Запрос, тело которого представлено в виде строки
using StringRequest = http::request<http::string_body>;
// Ответ, тело которого представлено в виде строки
using StringResponse = http::response<http::string_body>;

struct ContentType {
    ContentType() = delete;
    constexpr static std::string_view TEXT_HTML = "text/html"sv;
    // При необходимости внутрь ContentType можно добавить и другие типы контента
};

// Создаёт StringResponse с заданными параметрами
StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                  bool keep_alive,
                                  std::string_view content_type = ContentType::TEXT_HTML) {
    StringResponse response(status, http_version);
    response.set(http::field::content_type, content_type);
    response.body() = body;
    response.content_length(body.size());
    response.keep_alive(keep_alive);
    return response;
}


    StringResponse HandleHead(StringRequest&& req)
    {
        const auto text_response = [&req](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, req.version(), req.keep_alive());
        };
        return text_response(http::status::ok, ""sv);
    }
    StringResponse HandleGet(StringRequest&& req)
    {
        const auto text_response = [&req](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, req.version(), req.keep_alive());
        };
        auto name = req.target().substr(1);
        std::stringstream ss;
        ss << "Hello, "<<name ;
        return text_response(http::status::ok, ss.view());
    }

    StringResponse HandleUnexpected(StringRequest&& req)
    {
        const auto text_response = [&req](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, req.version(), req.keep_alive());
        };
        return text_response(http::status::method_not_allowed,"Invalid method");
    }


StringResponse HandleRequest(StringRequest&& req) {
    // Подставьте сюда код из синхронной версии HTTP-сервера
    switch (req.method())
    {
        case http::verb::get:
            return HandleGet(std::move(req));
        case http::verb::head:
            return HandleHead(std::move(req));
        default:
            return HandleUnexpected(std::move(req));

    }

    //return MakeStringResponse(http::status::ok, "OK"sv, req.version(), req.keep_alive());
}

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

int main() {
    const unsigned num_threads = std::thread::hardware_concurrency();

    net::io_context ioc(num_threads);

    // Подписываемся на сигналы и при их получении завершаем работу сервера
    net::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait([&ioc](const sys::error_code& ec, [[maybe_unused]] int signal_number) {
        if (!ec) {
            ioc.stop();
        }
    });

    const auto address = net::ip::make_address("0.0.0.0");
    constexpr net::ip::port_type port = 8080;
    auto handler  = [](auto&& req, auto&& sender)
    {
        sender(HandleRequest(std::forward<decltype(req)>(req))); // 19 вызывем функц объект sender, в аргумент которого передаем функцию, которая вернет http::response<http::string_body>
    };

    using handler_type = decltype(handler);


    http_server::ServeHttp(ioc, {address, port}, std::move(handler));//1 начинаем слушать входящие соединения

    // Эта надпись сообщает тестам о том, что сервер запущен и готов обрабатывать запросы
    std::cout << "Server has started..."sv << std::endl;

    RunWorkers(num_threads, [&ioc] {
        ioc.run();
    });
}
