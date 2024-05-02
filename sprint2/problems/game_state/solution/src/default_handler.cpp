
#include "default_handler.h"
#include "requests.h"
#include <exception>
#include <iostream>
namespace http_handler
{
    StringResponse default_handler::MakeStringResponse(http::status status, beast::string_view body, unsigned http_version,
                                  beast::string_view content_type)
    {

            StringResponse response(status,http_version);
            response.set(http::field::content_type, content_type);
            response.content_length(body.size());
            response.set(http::field::cache_control,"no-cache");
            response.body() = body;
            return response;


    }

    default_handler::default_handler(StringRequest&& request):
    _req(std::forward<decltype(request)>(request)){}

    bool default_handler::isApiReq()
    {
        return  _req.target().starts_with(RequestTargets::API_REQ);
    }
    bool default_handler::isMapReq()
    {
         return  _req.target().starts_with(RequestTargets::MAP_REQ);
    }

    bool default_handler::isMapListReq()
    {
        return _req.target() == RequestTargets::MAP_REQ;
    }

    bool default_handler::isMapIdReq()
    {
        return  _req.target().starts_with(RequestTargets::MAP_ID_REQ);
    }

    bool default_handler::isGameRequest()
    {
        return _req.target().starts_with(RequestTargets::GAME_REQ);
    }
    bool default_handler::isGamePlayerListReq()
    {
        return _req.target() == RequestTargets::GAME_PLAYERS_REQ;
    }

    bool default_handler::isJoinGameReq()
    {
        return _req.target() == RequestTargets::GAME_JOIN;
    }

    bool default_handler::isGameStateReq()
    {
        return _req.target() == RequestTargets::GAME_STATE;
    }

    StringResponse default_handler::Ok(std::string_view body)
    {
        const auto text_response = [this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, _req.version(),ContentType::APPLICATION_JSON);
        };
        auto resp =  text_response(http::status::ok,body);
        return resp;

    }

    std::variant <StringResponse, FileResponse> default_handler::HandleMapRequest()
    {
        return  NotAllowed(json_responce::ErrorJson("invalidMethod","Invalid method"), request_right[RequestTargets::MAP_REQ]);
    }

    std::variant <StringResponse, FileResponse> default_handler::HandleGameRequest()
    {
        if (isGamePlayerListReq())
        {
             return NotAllowed(json_responce::ErrorJson("invalidMethod","Invalid method"),request_right[RequestTargets::GAME_PLAYERS_REQ]);
        }
        if (isJoinGameReq())
        {
            return NotAllowed(json_responce::ErrorJson("invalidMethod","Invalid method"), request_right[RequestTargets::GAME_JOIN]);
        }
        if (isGameStateReq())
        {
            return NotAllowed(json_responce::ErrorJson("invalidMethod","Invalid method"), request_right[RequestTargets::GAME_STATE]);
        }
        return BadRequest();
    }

    std::variant <StringResponse, FileResponse> default_handler::HandleFileRequest()
    {
        return NotAllowed(json_responce::ErrorJson("invalidMethod","Invalid method"),"GET, HEAD");
    }


    std::variant <StringResponse, FileResponse> default_handler::HandleApiRequest()
    {
        if (isMapReq())
        {
            return HandleMapRequest();
        }
        if (isGameRequest())
        {
            return HandleGameRequest();
        }
        return BadRequest();
    }

    StringResponse default_handler::BadRequest(std::string_view errorMessage)
    {
        const auto text_response = [this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, _req.version(),ContentType::APPLICATION_JSON);
        };
        auto resp =  text_response(http::status::bad_request,errorMessage);
        resp.set(http::field::cache_control,"no-cache");
        return resp;
    }

    StringResponse default_handler::NotFound(std::string_view  body)
    {
        const auto text_response = [this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, _req.version(),ContentType::APPLICATION_JSON);
        };
        auto resp =  text_response(http::status::not_found,body);
        resp.set(http::field::cache_control,"no-cache");
        return resp;
    }

    StringResponse default_handler::Unauthorized(std::string_view body)
    {
        const auto text_response = [this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, _req.version(),ContentType::APPLICATION_JSON);
        };
        auto resp = text_response(http::status::unauthorized,body);
        resp.set(http::field::cache_control,"no-cache");
        return resp;
    }

    std::variant <StringResponse, FileResponse> default_handler::execute()
    {
        if (isApiReq())
        {
            return HandleApiRequest();
        }
        return HandleFileRequest();
    }
};
