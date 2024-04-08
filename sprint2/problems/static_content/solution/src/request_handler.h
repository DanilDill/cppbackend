#pragma once
#include <variant>
#include "http_server.h"
#include "model.h"
#include "content_type.h"
#include "staticfile_loader.h"
namespace http_handler {
namespace beast = boost::beast;
namespace http = beast::http;
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


    std::variant<StringResponse,FileResponse>  HandleGet(StringRequest&& req)
    {
        auto target = req.target();
        if (target.starts_with("/api/"))
        {
            if (target == "/api/v1/maps")
            {
                return  HandleGetMaps(std::move(req));
            }
            if (target.starts_with("/api/v1/maps/"))
            {
                std::string map_id ( target.substr("/api/v1/maps/"sv.size()));
                return HandleGetMap(map_id,std::move(req));
            }
            return HandleBadRequest(std::move(req));
        }

          target = target == "/" ? "index.html" : target.substr(1);
           auto response_file =  wwwroot.try_get(target);
           if (response_file)
           {
               auto res  = std::move((*response_file));
               return res;
           }
        return HandleNotFound(std::move(req));
    }

    std::variant<StringResponse,FileResponse> HandleRequest(StringRequest&& req)
    {

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



    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send)
    {
        auto response_variant = HandleRequest(std::forward<decltype(req)>(req));
        std::visit([&send](auto && response){send(response);},response_variant);
    }


    StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                      bool keep_alive,
                                      std::string_view content_type = ContentType::TEXT_HTML);






    StringResponse HandleGetMaps(StringRequest&& req);
    StringResponse HandleGetMap(const std::string& map_id, StringRequest&& req);
    StringResponse HandleBadRequest(StringRequest&& req);
    StringResponse HandleNotFound(StringRequest&& req);
    StringResponse HandleHead(StringRequest&& req);
    StringResponse HandleUnexpected(StringRequest&& req);



private:
    model::Game& game_;
    file::file_loader& wwwroot;
};

}  // namespace http_handler
