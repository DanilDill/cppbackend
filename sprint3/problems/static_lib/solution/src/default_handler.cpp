
#include "default_handler.h"
#include "requests.h"
#include <exception>
#include <iostream>
namespace http_handler
{
    StringResponse DefaultHandler::MakeStringResponse(http::status status, beast::string_view body, unsigned http_version,
                                                      beast::string_view content_type)
    {

            StringResponse response(status,http_version);
            response.set(http::field::content_type, content_type);
            response.content_length(body.size());
            response.set(http::field::cache_control,"no-cache");
            response.body() = body;
            return response;


    }

    DefaultHandler::DefaultHandler(StringRequest&& request):
    _req(std::forward<decltype(request)>(request)){}

    bool DefaultHandler::isApiReq()
    {
        return  _req.target().starts_with(RequestTargets::API_REQ);
    }
    bool DefaultHandler::isMapReq()
    {
         return  _req.target().starts_with(RequestTargets::MAP_REQ);
    }

    bool DefaultHandler::isMapListReq()
    {
        return _req.target() == RequestTargets::MAP_REQ;
    }

    bool DefaultHandler::isMapIdReq()
    {
        return  _req.target().starts_with(RequestTargets::MAP_ID_REQ);
    }

    bool DefaultHandler::isGameRequest()
    {
        return _req.target().starts_with(RequestTargets::GAME_REQ);
    }
    bool DefaultHandler::isGamePlayerListReq()
    {
        return _req.target() == RequestTargets::GAME_PLAYERS_REQ;
    }

    bool DefaultHandler::isJoinGameReq()
    {
        return _req.target() == RequestTargets::GAME_JOIN;
    }

    bool DefaultHandler::isGameStateReq()
    {
        return _req.target() == RequestTargets::GAME_STATE;
    }

    bool DefaultHandler::isGameTick()
    {
        return _req.target() == RequestTargets::GAME_TICK;
    }

    StringResponse DefaultHandler::Ok(std::string_view body)
    {
        const auto text_response = [this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, _req.version(),ContentType::APPLICATION_JSON);
        };
        auto resp =  text_response(http::status::ok,body);
        return resp;

    }

    bool DefaultHandler::isGameAction()
    {
         return _req.target() == RequestTargets::GAME_ACTION;
    }

    std::variant <StringResponse, FileResponse> DefaultHandler::HandleMapRequest()
    {
        if (isMapListReq())
        {
            return HandleMapsList();
        }
        if (isMapIdReq())
        {
            return HandleMapId();
        }
        return BadRequest();

    }

    std::variant <StringResponse, FileResponse> DefaultHandler::HandleGameRequest()
    {
        if (isGamePlayerListReq())
        {
           return HandlePlayerList();
        }
        if (isJoinGameReq())
        {
            return HandleJoinGame();
        }
        if (isGameStateReq())
        {
            return HandleGameState();
        }
        if (isGameAction())
        {
            return HandlePlayerAction();
        }
        if (isGameTick())
        {
            return HandleGameTick();
        }

        return BadRequest();
    }

     std::variant <StringResponse, FileResponse> DefaultHandler::HandleGameTick()
    {
        return  NotAllowed(json_responce::ErrorJson("invalidMethod","Invalid method"), request_right[RequestTargets::GAME_TICK]);
    }


    std::variant <StringResponse, FileResponse> DefaultHandler::HandleMapsList()
     {
         return  NotAllowed(json_responce::ErrorJson("invalidMethod","Invalid method"), request_right[RequestTargets::MAP_REQ]);
     }

     std::variant <StringResponse, FileResponse> DefaultHandler::HandleMapId()
     {
         return  NotAllowed(json_responce::ErrorJson("invalidMethod","Invalid method"), request_right[RequestTargets::MAP_ID_REQ]);
     }

     std::variant <StringResponse, FileResponse> DefaultHandler::HandlePlayerList()
     {
         return NotAllowed(json_responce::ErrorJson("invalidMethod","Invalid method"),request_right[RequestTargets::GAME_PLAYERS_REQ]);
     }

     std::variant <StringResponse, FileResponse> DefaultHandler::HandleJoinGame()
     {
         return NotAllowed(json_responce::ErrorJson("invalidMethod","Invalid method"), request_right[RequestTargets::GAME_JOIN]);
     }

     std::variant <StringResponse, FileResponse> DefaultHandler::HandleGameState()
     {
         return NotAllowed(json_responce::ErrorJson("invalidMethod","Invalid method"), request_right[RequestTargets::GAME_STATE]);
     }

     std::variant <StringResponse, FileResponse> DefaultHandler::HandlePlayerAction()
     {
         return NotAllowed(json_responce::ErrorJson("invalidMethod","Invalid method"), request_right[RequestTargets::GAME_ACTION]);
     }

    std::variant <StringResponse, FileResponse> DefaultHandler::HandleFileRequest()
    {
        return NotAllowed(json_responce::ErrorJson("invalidMethod","Invalid method"),"GET, HEAD");
    }


    std::variant <StringResponse, FileResponse> DefaultHandler::HandleApiRequest()
    {
        if (isMapReq())
        {
            return HandleMapRequest();
        }
        if (isGameRequest())
        {
            return HandleGameRequest();
        }
        return BadRequest(json_responce::ErrorJson("badRequest","Bad Request"));
    }

    StringResponse DefaultHandler::BadRequest(std::string_view errorMessage)
    {
        const auto text_response = [this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, _req.version(),ContentType::APPLICATION_JSON);
        };
        auto resp =  text_response(http::status::bad_request,errorMessage);
        resp.set(http::field::cache_control,"no-cache");
        return resp;
    }

    StringResponse DefaultHandler::BadRequest()
    {
        return BadRequest("");
    }

    StringResponse DefaultHandler::NotFound(std::string_view  body)
    {
        const auto text_response = [this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, _req.version(),ContentType::APPLICATION_JSON);
        };
        auto resp =  text_response(http::status::not_found,body);
        resp.set(http::field::cache_control,"no-cache");
        return resp;
    }

    StringResponse DefaultHandler::Unauthorized(std::string_view body)
    {
        const auto text_response = [this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, _req.version(),ContentType::APPLICATION_JSON);
        };
        auto resp = text_response(http::status::unauthorized,body);
        resp.set(http::field::cache_control,"no-cache");
        return resp;
    }


    std::variant <StringResponse, FileResponse> DefaultHandler::execute()
    {
        if (isApiReq())
        {
            return HandleApiRequest();
        }
        return HandleFileRequest();
    }


    StringResponse DefaultHandler::NotAllowed(std::string_view  body, std::string_view methods )
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
    StringResponse DefaultHandler::NotAllowed(std::string_view methods)
    {
        return NotAllowed("",methods);
    }

};
