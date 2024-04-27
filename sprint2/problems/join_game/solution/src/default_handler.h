#pragma once
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <string_view>
#include <variant>
#include "content_type.h"
#include "json_response.h"
namespace http_handler
{
    namespace beast = boost::beast;
    namespace http = beast::http;
    using StringResponse = http::response<http::string_body>;
    using FileResponse = http::response<http::file_body>;
    using StringRequest = http::request<http::string_body>;

    struct RequestTargets
    {
        RequestTargets()=delete;
        static constexpr std::string_view API_REQ = "/api/";
        static constexpr std::string_view MAP_REQ = "/api/v1/maps";
        static constexpr std::string_view MAP_ID_REQ = "/api/v1/maps/";
        static constexpr std::string_view GAME_REQ = "/api/v1/game/";
        static constexpr std::string_view GAME_PLAYERS_REQ = "/api/v1/game/players";
        static constexpr std::string_view GAME_JOIN = "/api/v1/game/join";

    };



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
        default_handler(StringRequest&& request);
        bool isApiReq();
        bool isMapReq();
        bool isMapListReq();
        bool isMapIdReq();
        bool isGameRequest();
        bool isGamePlayerListReq();
        bool isJoinGameReq();
        StringResponse NotAllowed(std::convertible_to<std::string_view>auto ... methods );
        std::variant <StringResponse, FileResponse> execute();

    };
}