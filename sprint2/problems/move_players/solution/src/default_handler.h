
#pragma once
#include <variant>
#include "beast.h"
#include "content_type.h"
#include "json_response.h"
#include "json_response.h"
#include "requests.h"

namespace http_handler
{
    using namespace std::literals;

    using StringResponse = http::response<http::string_body>;
    using FileResponse = http::response<http::file_body>;
    using StringRequest = http::request<http::string_body>;

    class default_handler
    {
    protected:
    StringRequest _req;
        StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                          std::string_view content_type = ContentType::APPLICATION_JSON);

        virtual std::variant <StringResponse, FileResponse> HandleApiRequest();
        virtual std::variant <StringResponse, FileResponse> HandleMapRequest();
        virtual std::variant <StringResponse, FileResponse> HandleGameRequest();
        virtual std::variant <StringResponse, FileResponse> HandleFileRequest();
    public:
        explicit default_handler(StringRequest&& request);
        bool isApiReq();
        bool isMapReq();
        bool isMapListReq();
        bool isMapIdReq();
        bool isGameRequest();
        bool isGamePlayerListReq();
        bool isJoinGameReq();
        bool isGameStateReq();

        StringResponse NotAllowed(std::string_view  body, std::string_view methods )
        {
            const auto text_response = [this](http::status status, std::string_view text)
            {
                return MakeStringResponse(status, text, _req.version(),ContentType::APPLICATION_JSON);
            };
            auto  resp =  text_response(http::status::method_not_allowed, body);
            resp.set(http::field::cache_control,"no-cache"sv.data());
            resp.set(http::field::allow,methods);
            return resp;
        }
        StringResponse NotAllowed(std::string_view methods)
        {
           return NotAllowed("",methods);
        }
        StringResponse BadRequest(std::string_view  errorMessage="");
        StringResponse NotFound(std::string_view  body="");
        StringResponse Unauthorized(std::string_view  body= "");
        StringResponse Ok(std::string_view body="");

        std::variant <StringResponse, FileResponse> execute();

    };
}