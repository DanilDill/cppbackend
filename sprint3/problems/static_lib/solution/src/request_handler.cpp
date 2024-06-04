
#include "request_handler.h"
#include "json_response.h"
#include "get_handler.h"
#include "head_handler.h"
#include "post_handler.h"

namespace http_handler
{
    StringResponse RequestHandler::MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                      bool keep_alive,
                                      std::string_view content_type)
    {
        StringResponse response(status, http_version);
        response.set(http::field::content_type, content_type);
        response.body() = body;
        response.content_length(body.size());
        response.keep_alive(keep_alive);
        return response;
    }

    std::variant<StringResponse,FileResponse> RequestHandler::HandleRequest(StringRequest&& req)
    {

        switch (req.method())
        {
            case http::verb::get:
                return GetHandler(std::move(req), game_, wwwroot).execute();
            case http::verb::head:
                return HeadHandler(std::move(req), game_, wwwroot).execute();
            case http::verb::post:
                return PostHandler (std::move(req), game_, wwwroot).execute();
            default:
                return DefaultHandler(std::move(req)).execute();

        }
    }



    StringResponse RequestHandler::HandleHead(StringRequest&& req)
    {
        const auto text_response = [&req, this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, req.version(), req.keep_alive(),ContentType::APPLICATION_JSON);
        };
        return text_response(http::status::ok, ""sv);
    }

    StringResponse RequestHandler::HandleUnexpected(StringRequest&& req)
    {

        const auto text_response = [&req, this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, req.version(), req.keep_alive(),ContentType::APPLICATION_JSON);
        };
        auto resp =  text_response(http::status::method_not_allowed,json_responce::ErrorJson("invalidMethod","Invalid method"));
        resp.set(http::field::cache_control,"no-cache");
        resp.set(http::field::allow,"POST");
        return resp;
    }



}  // namespace http_handler
