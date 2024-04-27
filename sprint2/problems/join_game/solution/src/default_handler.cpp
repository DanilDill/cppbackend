#include "default_handler.h"
namespace http_handler
{
    StringResponse default_handler::MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                  std::string_view content_type)
    {
        StringResponse response(status, http_version);
        response.set(http::field::content_type, content_type);
        response.content_length(body.size());
        response.set(http::field::cache_control,"no-cache");
        response.body() = body;
        return response;
    }


    default_handler::default_handler(StringRequest&& request):
    _req(request){}

    bool default_handler::isApiReq()
    {
      _req.target().starts_with(RequestTargets::API_REQ);
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
    StringResponse default_handler::NotAllowed(std::convertible_to<std::string_view>auto ... methods )
    {
         const auto text_response = [this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, _req.version(),ContentType::APPLICATION_JSON);
        };
        auto  resp =  text_response(http::status::method_not_allowed,json_responce::ErrorJson("invalidMethod","Invalid method"));
        resp.set(http::field::cache_control,"no-cache");
        for (const auto& method : std::initializer_list<std::string_view>{ methods... })
        {
             resp.set(http::field::allow,method);
        }
        return resp;
    }

    std::variant <StringResponse, FileResponse> default_handler::HandleMapRequest()
    {
        return  NotAllowed("GET","HEAD");
    }

    std::variant <StringResponse, FileResponse> default_handler::HandleGameRequest()
    {
        if (isGamePlayerListReq())
        {
             return NotAllowed("GET","HEAD");
        }
        if (isJoinGameReq())
        {
            return NotAllowed("POST");
        }
    }

    std::variant <StringResponse, FileResponse> default_handler::HandleFileRequest()
    {
        return NotAllowed("GET","HEAD");
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
