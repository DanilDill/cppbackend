#pragma once
#include <variant>
#include "http_server.h"
#include "get_handler.h"
#include "head_handler.h"
#include "post_handler.h"
#include "model.h"
#include "content_type.h"
#include "staticfile_loader.h"



namespace http_handler {

class RequestHandler {
public:
    explicit RequestHandler(model::Game& game, file::file_loader& root)
        : game_{game}, wwwroot{root}
        {}
    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;
    std::variant<StringResponse,FileResponse> HandleRequest(StringRequest&& req)
    {

        switch (req.method())
        {
            case http::verb::get:
                return get_handler(std::move(req),game_,wwwroot).execute();
            case http::verb::head:
                return head_handler(std::move(req), game_, wwwroot).execute();
            case http::verb::post:
                return post_handler (std::move(req), game_, wwwroot).execute();
            default:
                return default_handler(std::move(req)).execute();

        }
    }



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
