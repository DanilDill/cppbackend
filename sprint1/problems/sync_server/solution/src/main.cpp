#ifdef WIN32
#include <sdkddkver.h>
#endif
// boost.beast будет использовать std::string_view вместо boost::string_view
#define BOOST_BEAST_USE_STD_STRING_VIEW

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <iostream>
#include <thread>
#include <sstream>
#include <optional>

namespace net = boost::asio;
using tcp = net::ip::tcp;
using namespace std::literals;
namespace beast = boost::beast;
namespace http = beast::http;

using StringRequest = http::request<http::string_body>;
using StringResponse = http::response<http::string_body>;


struct ContentType {
    ContentType() = delete;
    constexpr static std::string_view TEXT_HTML = "text/html"sv;
    // При необходимости внутрь ContentType можно добавить и другие типы контента
};

std::optional<StringRequest> ReadRequest(tcp::socket& socket, beast::flat_buffer& buffer)
{
    beast::error_code ec;
    StringRequest req;
    // Считываем из socket запрос req, используя buffer для хранения данных.
    // В ec функция запишет код ошибки.
    http::read(socket, buffer, req, ec);

    if (ec == http::error::end_of_stream)
    {
        return std::nullopt;
    }
    if (ec)
    {
        throw std::runtime_error("Failed to read request: "s.append(ec.message()));
    }
    return req;
}


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
    ss<< "<strong>" << "Hello, "<< name<< "</strong>";
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

StringResponse HandleRequest(StringRequest&& req)
{

    switch (req.method()) 
    {
        case http::verb::get:
            return HandleGet(std::move(req));
            break;
        case http::verb::head:
            return HandleHead(std::move(req));
            break;
        default:
            return HandleUnexpected(std::move(req));

    }

}



template <typename RequestHandler>
void HandleConnection(tcp::socket& socket,RequestHandler&& handle_request)
{
    try {
        // Буфер для чтения данных в рамках текущей сессии.
        beast::flat_buffer buffer;

        // Продолжаем обработку запросов, пока клиент их отправляет
        while (auto request = ReadRequest(socket, buffer)) {
            StringResponse response = handle_request(*std::move(request));
            http::write(socket, response);
            if (response.need_eof()) {
                break;
            }
        }
    } catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    beast::error_code ec;
    // Запрещаем дальнейшую отправку данных через сокет
    socket.shutdown(tcp::socket::shutdown_send, ec);
}


int main()
{
    // Выведите строчку "Server has started...", когда сервер будет готов принимать подключения
    net::io_context ioc;
    const auto address = net::ip::make_address("127.0.0.1");
    constexpr unsigned short port = 8080;
    tcp::acceptor acceptor(ioc, {address, port});
    std::cout << "Server has started... "sv << std::endl;
    while (true)
    {
        tcp::socket socket(ioc);
        acceptor.accept(socket);
        HandleConnection(socket,HandleRequest);
    }
}
