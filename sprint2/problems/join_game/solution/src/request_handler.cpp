
#include "request_handler.h"
#include "json_response.h"
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

    StringResponse RequestHandler::HandleHead(StringRequest&& req)
    {
        const auto text_response = [&req, this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, req.version(), req.keep_alive());
        };
        return text_response(http::status::ok, ""sv);
    }

    StringResponse RequestHandler::HandleUnexpected(StringRequest&& req)
    {
        const auto text_response = [&req, this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, req.version(), req.keep_alive());
        };
        return text_response(http::status::method_not_allowed,"Invalid method");
    }

    StringResponse RequestHandler::HandlePost(StringRequest&& req)
    {

    }


}  // namespace http_handler
