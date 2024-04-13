#pragma once

template<typename Handler>
class LoggingRequestHandler
{
public:
    LoggingRequestHandler (Handler& handler):
            decorated_handler_(handler){};
    void operator()(auto && req, auto&& send)
    {
        auto senderFuncttor = [sender = std::move(send)](auto&& response)
        {
            /// я могу модифицировать обработчик отправки тут
            sender(response);
        };
        decorated_handler_(std::forward<decltype(req)>(req), std::forward<decltype(senderFuncttor)>(senderFuncttor));
        // и тут
    }
private:
    Handler& decorated_handler_;
};