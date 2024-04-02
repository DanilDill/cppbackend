
#include "request_handler.h"

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

    StringResponse RequestHandler::HandleGetMaps()
    {
       auto maps =  game_.GetMaps();
    }


    StringResponse RequestHandler::HandleGet(StringRequest&& req)
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
    StringResponse RequestHandler::HandleUnexpected(StringRequest&& req)
    {
        const auto text_response = [&req, this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, req.version(), req.keep_alive());
        };
        return text_response(http::status::method_not_allowed,"Invalid method");
    }
    StringResponse RequestHandler::HandleRequest(StringRequest&& req)
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


}  // namespace http_handler
