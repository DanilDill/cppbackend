
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


        virtual std::variant <StringResponse, FileResponse> HandleMapsList();
        virtual std::variant <StringResponse, FileResponse> HandleMapId();
        virtual std::variant <StringResponse, FileResponse> HandlePlayerList();
        virtual std::variant <StringResponse, FileResponse> HandleJoinGame();
        virtual std::variant <StringResponse, FileResponse> HandleGameState();
        virtual std::variant <StringResponse, FileResponse> HandlePlayerAction();
        virtual std::variant <StringResponse, FileResponse> HandleGameTick();


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
        bool isGameAction();
        bool isGameTick();

        StringResponse NotAllowed(std::string_view  body, std::string_view methods );
        StringResponse NotAllowed(std::string_view methods);
        virtual StringResponse BadRequest(std::string_view  errorMessage="");
        virtual StringResponse NotFound(std::string_view  body="");
        virtual StringResponse Unauthorized(std::string_view  body= "");
        virtual StringResponse Ok(std::string_view body="");

        std::variant <StringResponse, FileResponse> execute();

    };
}