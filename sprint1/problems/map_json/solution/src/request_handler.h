#pragma once
#include "http_server.h"
#include "model.h"


namespace http_handler {
namespace beast = boost::beast;
namespace http = beast::http;
using namespace std::literals;
using StringResponse = http::response<http::string_body>;
using StringRequest = http::request<http::string_body>;

    struct ContentType {
        ContentType() = delete;
        constexpr static std::string_view TEXT_HTML = "text/html"sv;

    };
class RequestHandler {
public:
    explicit RequestHandler(model::Game& game)
        : game_{game} {
    }

    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;

    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send)
    {
      send(HandleRequest(std::forward<decltype(req)>(req)));
    }


    StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                      bool keep_alive,
                                      std::string_view content_type = ContentType::TEXT_HTML)
    {
        StringResponse response(status, http_version);
        response.set(http::field::content_type, content_type);
        response.body() = body;
        response.content_length(body.size());
        response.keep_alive(keep_alive);
        return response;
    }

    StringResponse HandleHead(StringRequest&& req)
    {
        const auto text_response = [&req, this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, req.version(), req.keep_alive());
        };
        return text_response(http::status::ok, ""sv);
    }
    StringResponse HandleGet(StringRequest&& req)
    {
        const auto text_response = [&req, this](http::status status, std::string_view text)
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
        const auto text_response = [&req, this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, req.version(), req.keep_alive());
        };
        return text_response(http::status::method_not_allowed,"Invalid method");
    }
    StringResponse HandleRequest(StringRequest&& req) {
        switch (req.method())
        {
            case http::verb::get:
                return HandleGet(std::move(req));
            case http::verb::head:
                return HandleHead(std::move(req));
            default:
                return HandleUnexpected(std::move(req));

        }

    }



private:
    model::Game& game_;
};

}  // namespace http_handler
