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
                                      std::string_view content_type = ContentType::TEXT_HTML);

    StringResponse HandleHead(StringRequest&& req);
    StringResponse HandleGetMaps();
    StringResponse HandleGet(StringRequest&& req);
    StringResponse HandleUnexpected(StringRequest&& req);
    StringResponse HandleRequest(StringRequest&& req);


private:
    model::Game& game_;
};

}  // namespace http_handler
