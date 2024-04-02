#pragma once
#include "sdk.h"
// boost.beast будет использовать std::string_view вместо boost::string_view
#define BOOST_BEAST_USE_STD_STRING_VIEW

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <iostream>
namespace http_server {

namespace net = boost::asio;
using tcp = net::ip::tcp;
namespace beast = boost::beast;
namespace http = beast::http;
using namespace std::literals;
namespace sys = boost::system;

void ReportError(beast::error_code ec, std::string_view what);

    class SessionBase
    {
    public:
        SessionBase(const SessionBase&) = delete;
        SessionBase& operator=(const SessionBase&) = delete;
        void Run(); // 13 запускаем
    protected:
        using HttpRequest = http::request<http::string_body>;
        explicit SessionBase(tcp::socket&& socket);
        ~SessionBase() = default;

        template <typename Body, typename Fields>
        void Write(http::response<Body, Fields>&& response)
        {
            // Запись выполняется асинхронно, поэтому response перемещаем в область кучи
            auto safe_response = std::make_shared<http::response<Body, Fields>>(std::move(response));

            auto self = GetSharedThis();
            http::async_write(stream_, *safe_response,
                              [safe_response, self](beast::error_code ec, std::size_t bytes_written) {
                                  self->OnWrite(safe_response->need_eof(), ec, bytes_written);
                              });
        }

    private:
        void OnWrite(bool close, beast::error_code ec, [[maybe_unused]] std::size_t bytes_written);
        void Read();

        void OnRead(beast::error_code ec, [[maybe_unused]] std::size_t bytes_read);
        void Close();

        // Обработку запроса делегируем подклассу
        virtual void HandleRequest(HttpRequest&& request) = 0;
        virtual std::shared_ptr<SessionBase> GetSharedThis() = 0;
        // tcp_stream содержит внутри себя сокет и добавляет поддержку таймаутов
        beast::tcp_stream stream_;
        beast::flat_buffer buffer_;
        HttpRequest request_;
    };

template <typename RequestHandler>
class Session : public SessionBase, public std::enable_shared_from_this<Session<RequestHandler>>
{
public:
    template <typename Handler>
    Session(tcp::socket&& socket, Handler&& request_handler)//12 создаем сессию
            : SessionBase(std::move(socket))
            , request_handler_(std::forward<Handler>(request_handler)) //получаем обработчик
            {}

private:
    std::shared_ptr<SessionBase> GetSharedThis() override
    {
        return this->shared_from_this();
    };

    RequestHandler request_handler_;

    void HandleRequest(HttpRequest&& request) override //17 брабатываем запрос
    {
        /*
         * вызывваем передаенные из  main функционлальный объект
           auto handler  = [](auto&& req, auto&& sender)
             {
                   sender(HandleRequest(std::forward<decltype(req)>(req)));
              };
         */
        request_handler_(std::move(request), [self = this->shared_from_this()](auto&& response)
        {
            self->Write(std::move(response)); // 18 по итогу send оказывается лямбдой, который принимает  http::response<http::string_body>;
        });
    }

};

template <typename RequestHandler>
class Listener : public std::enable_shared_from_this<Listener<RequestHandler>>
{
public:
    template <typename Handler>
    Listener(net::io_context& ioc, const tcp::endpoint& endpoint, Handler&& request_handler) // 4 создаем Listener ,Слушаем входящие соединеия
            : ioc_(ioc)
            , acceptor_(net::make_strand(ioc))
            , request_handler_(std::forward<Handler>(request_handler)) //6 получаем функц. объект
    {

        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(net::socket_base::reuse_address(true));
        acceptor_.bind(endpoint);
        acceptor_.listen(net::socket_base::max_listen_connections);
    };

    void Run()
    {
        DoAccept();// 7 реагируем на содеинение
    };

    void DoAccept()
    {
        acceptor_.async_accept(
                net::make_strand(ioc_),
                beast::bind_front_handler(&Listener::OnAccept, this->shared_from_this()));//8 разрешаем подключение
    }
    void OnAccept(sys::error_code ec, tcp::socket socket) //9 обрабатываем клиента
    {
        using namespace std::literals;
        if (ec)
        {
            return ReportError(ec, "accept"sv);
        }
        AsyncRunSession(std::move(socket));  //10 асинхронно запускаем сессию с клиентом
        DoAccept();//cлушаем следующих
    }
    void AsyncRunSession(tcp::socket&& socket)
    {
        std::make_shared<Session<RequestHandler>>(std::move(socket), request_handler_)->Run();// 11 создаем сессию, запускаем передаем обработчик
    }

private:
    net::io_context& ioc_;
    tcp::acceptor acceptor_;
    RequestHandler request_handler_;
};

template <typename RequestHandler>
void ServeHttp(net::io_context& ioc, const tcp::endpoint& endpoint, RequestHandler&& handler)//2
{
    using MyListener = Listener<std::decay_t<RequestHandler>>;
    /*std::decay_t Выполняет преобразования типов, эквивалентные тем, которые выполняются при передаче аргументов по значению
     Если T — это «массив U» или ссылка на него, тип typedef члена — U*
     в общем случае он отбрасывает ссылки и квалификаторы
     Лямбда функцию он оставит без изменений

     */
    std::make_shared<MyListener>(ioc, endpoint, std::forward<RequestHandler>(handler))->Run(); //3 процесс запуска Listener который принимаеет входящие соединения
}

}  // namespace http_server
