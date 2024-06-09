#pragma once
#include <variant>
//#include "http_server.h"
#include "model/model.h"
#include "content_type.h"
//#include "GetHandler.h"
#include "beast.h"

namespace file
{
    class file_loader;
}
namespace http_handler {

    using namespace std::literals;

    using StringResponse = http::response<http::string_body>;
    using FileResponse = http::response<http::file_body>;
    using StringRequest = http::request<http::string_body>;




    class RequestHandler {
public:
    explicit RequestHandler(model::Game& game, file::file_loader& root)
        : game_{game}, wwwroot{root}
        {}
    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;
    std::variant<StringResponse,FileResponse> HandleRequest(StringRequest&& req);




    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send)
    {
        auto response_variant = HandleRequest(std::forward<decltype(req)>(req));
        std::visit([&send](auto && response){send(response);},response_variant);
    }


    StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                      bool keep_alive,
                                      std::string_view content_type = ContentType::TEXT_HTML);

    StringResponse HandlePost(StringRequest&& req);

    StringResponse HandleHead(StringRequest&& req);
    StringResponse HandleUnexpected(StringRequest&& req);



private:
    model::Game& game_;
    file::file_loader& wwwroot;
};

}  // namespace http_handler
