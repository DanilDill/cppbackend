#pragma once
#include <boost/log/trivial.hpp>
#include <boost/json.hpp>
#include <boost/log/core.hpp>
#include <boost/date_time.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <queue>
#include <ostream>
#include <iostream>
template<typename Handler>
class LoggingRequestHandler
{
public:
    LoggingRequestHandler (Handler& handler):
            decorated_handler_(handler)
    {
        InitLogger();
        BOOST_LOG_SEV(logger, boost::log::trivial::severity_level::info) << Log("port","8080","message","server started");
    };
    template<class... Ts>
    boost::json::value Log(const Ts&... args)
    {
        try
        {
          std::queue<std::string>queue;
          (queue.push(args),...);
          if (queue.size() % 2 != 0)
          {
              throw std::logic_error("wrong json format");
          }
          boost::json::object res;
          while (!queue.empty())
          {
                auto key = queue.front();
                queue.pop();
                auto value = queue.front();
                queue.pop();
                res.emplace(key,value);
          }
          return boost::json::value(std::move(res));
        }
        catch (const std::logic_error& er)
        {
            return "";
        }
    }

    void operator()(auto && req, auto&& send)
    {
        auto senderFuncttor = [this, sender = std::move(send)](auto&& response)
        {
            /// я могу модифицировать обработчик отправки тут
            boost::json::value logdata{{"response","response_data"}};
            BOOST_LOG_SEV(logger, boost::log::trivial::severity_level::info) << logdata;
            sender(response);
        };
        boost::json::value logdata{{"request","request_data"}};
        BOOST_LOG_SEV(logger, boost::log::trivial::severity_level::info) << logdata;
        decorated_handler_(std::forward<decltype(req)>(req), std::forward<decltype(senderFuncttor)>(senderFuncttor));
        // и тут
    }

private:
    void InitLogger()
    {
        // Конфигурация формата вывода в JSON
        boost::log::add_common_attributes();
        boost::log::core::get()->add_global_attribute("Scope", boost::log::attributes::named_scope());
        boost::log::add_common_attributes();

        // Установка формата вывода в JSON

       boost::log::add_console_log(
                std::cout,
                boost::log::keywords::format = boost::log::expressions::format("{\"timestamp\":\"%1%\",\"data\":%2%}")
                                               % boost::log::expressions::attr<boost::log::attributes::local_clock::value_type>("TimeStamp")
                                               % boost::log::expressions::smessage
        );

    }
private:
    boost::log::sources::severity_logger<boost::log::trivial::severity_level> logger;
    Handler& decorated_handler_;
};