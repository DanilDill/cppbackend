#include "http_server.h"
#include <iostream>
namespace http_server {

// Разместите здесь реализацию http-сервера, взяв её из задания по разработке асинхронного сервера
    void ReportError(beast::error_code ec, std::string_view what)
    {
        std::cerr << what << ": "sv << ec.message() << std::endl;
    }

    SessionBase::SessionBase(tcp::socket&& socket)
            :stream_(std::move(socket)){}

    void SessionBase::Run()
    {
        net::dispatch(stream_.get_executor(),
                      beast::bind_front_handler(&SessionBase::Read, GetSharedThis()));
    }

    void SessionBase::OnWrite(bool close, beast::error_code ec, [[maybe_unused]] std::size_t bytes_written)
    {
        if (ec)
        {
            return ReportError(ec, "write"sv);
        }

        if (close)
        {
            return Close();
        }
        Read();
    }
    void SessionBase::Read()
    {
        using namespace std::literals;
        // Очищаем запрос от прежнего значения (метод Read может быть вызван несколько раз)
        request_ = {};
        stream_.expires_after(30s);
        // Считываем request_ из stream_, используя buffer_ для хранения считанных данных
        http::async_read(stream_, buffer_, request_,
                // По окончании операции будет вызван метод OnRead
                         beast::bind_front_handler(&SessionBase::OnRead, GetSharedThis()));
    }

    void SessionBase::OnRead(beast::error_code ec, [[maybe_unused]] std::size_t bytes_read)
    {
        using namespace std::literals;
        if (ec == http::error::end_of_stream) {
            // Нормальная ситуация - клиент закрыл соединение
            return Close();
        }
        if (ec) {
            return ReportError(ec, "read"sv);
        }
        HandleRequest(std::move(request_));
    }

    void SessionBase::Close()
    {
        try
        {
            stream_.socket().shutdown(tcp::socket::shutdown_send);
        }
        catch (std::exception& ex)
        {
          std::cerr << ex.what() << std::endl;
        }

    }


}  // namespace http_server
