#pragma once
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/core.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/logger.hpp>

#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>

#include <boost/date_time.hpp>


#include <boost/json.hpp>

#include <queue>
#include <ostream>
#include <iostream>

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
namespace expr = boost::log::expressions;
namespace attrs = boost::log::attributes;




BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)

BOOST_LOG_ATTRIBUTE_KEYWORD(data, "data", boost::json::value)
BOOST_LOG_ATTRIBUTE_KEYWORD(message, "message", std::string)
void Formatter(logging::record_view const& rec, logging::formatting_ostream& strm)
{
    strm << "{"<<"\"timestamp\":";
    auto ts = rec[timestamp];
    strm <<"\""<< to_iso_extended_string(*ts) << "\"" << ",";
    strm << "\"data\":"<< rec[data] <<"," << "\"message\":"<<"\""<<rec[message] <<"\"";
    strm << "}";
}

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

    void Log(boost::log::trivial::severity_level lvl,const boost::json::value& json_data, const std::string& message_str="")
    {
      //  BOOST_LOG_SEV(logger, lvl) << data << "," << LogMessage(message);
        BOOST_LOG_TRIVIAL(info)  << logging::add_value(data, json_data) << logging::add_value(message,message_str);
    };



    void operator()(auto && req, auto&& send)
    {
        auto  enpoint_str = req.second;
        std::chrono::system_clock::time_point start_ts = std::chrono::system_clock::now();
        auto senderFuncttor = [this, &start_ts, &enpoint_str, sender = std::move(send)](auto&& response)
        {
            auto content_type = response[http::field::content_type];
            content_type = content_type == "" ? "null" : content_type;
            std::chrono::system_clock::time_point end_ts = std::chrono::system_clock::now();
            Log(boost::log::trivial::severity_level::info,{{"ip",enpoint_str},
                                                           {"response_time", (end_ts - start_ts)/1ms},

                                                           {"code",response.result_int()},{"content_type",content_type}},"response sent");
            sender(response);
        };
        Log(boost::log::trivial::severity_level::info,{{"ip",enpoint_str},{"URI",req.first.target()},{"method",req.first.method_string()}},"request received");
        decorated_handler_(std::forward<decltype(req.first)>(req.first), std::forward<decltype(senderFuncttor)>(senderFuncttor));

    }

private:

    void InitLogger()
    {
        boost::log::add_common_attributes();
        boost::log::add_console_log(std::clog, keywords::format = &Formatter);
    }
private:
    Handler& decorated_handler_;
};