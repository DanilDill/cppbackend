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
namespace http = boost::beast::http;
template<typename Handler>
class LoggingRequestHandler
{
public:

    LoggingRequestHandler (Handler& handler):
            decorated_handler_(handler)
    {
        InitLogger();
    };

    void Log(boost::log::trivial::severity_level lvl,const boost::json::value& data, const std::string& message="")
    {
        BOOST_LOG_SEV(logger, lvl) << data << "," << LogMessage(message);
    };

    std::string LogMessage(const std::string& msg)
    {
       std::stringstream ss;
       ss << "\"" << "message" << "\"" << ":"<< "\"" << msg << "\"";
        return ss.str();
    }


    void operator()(auto && req, auto&& send)
    {
        auto  enpoint_str = req.second;
        std::chrono::system_clock::time_point start_ts = std::chrono::system_clock::now();
        auto senderFuncttor = [this, &start_ts, &enpoint_str, sender = std::move(send)](auto&& response)
        {
            /// я могу модифицировать обработчик отправки тут
            auto content_type = response[http::field::content_type];
            content_type = content_type == "" ? "null" : content_type;

            std::chrono::system_clock::time_point end_ts = std::chrono::system_clock::now();


            Log(boost::log::trivial::severity_level::info,{{"ip",enpoint_str},
                                                           {"response_time", (end_ts - start_ts)/1ms},

                                                           {"code",response.result_int()},{"content_type",content_type}},"responce sent");
            sender(response);
        };
        Log(boost::log::trivial::severity_level::info,{{"ip",enpoint_str},{"URI",req.first.target()},{"method",req.first.method_string()}},"request received");
        decorated_handler_(std::forward<decltype(req.first)>(req.first), std::forward<decltype(senderFuncttor)>(senderFuncttor));

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