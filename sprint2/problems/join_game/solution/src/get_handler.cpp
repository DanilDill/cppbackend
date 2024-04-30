
#include "get_handler.h"
#include "staticfile_loader.h"

namespace http_handler {
    get_handler::get_handler(StringRequest &&request, model::Game &game, file::file_loader &root):
            default_handler(std::forward<decltype(request)>(request)), wwwroot(root), game_(game) {}

    std::variant <StringResponse, FileResponse>  get_handler::HandleMapRequest()
    {
        if (isMapListReq())
        {
            return Maps();
        }
        if (isMapIdReq())
        {
            std::string map_id(_req.target().substr("/api/v1/maps/"sv.size()));
            return Map(map_id);
        }
        return BadRequest();
    }

    std::variant <StringResponse, FileResponse> get_handler::HandleGameRequest()
    {
        if (isGamePlayerListReq())
        {
            auto auth_str = _req[http::field::authorization];
            if (auth_str == "" or auth_str.substr("Bearer"sv.size()) == "")
            {
                return Unauthorized(json_responce::ErrorJson("invalidToken","Authorization header is missing"));
            }
            auto token = auth_str.substr("Bearer "sv.size());

            if(game_.FindPlayer(Token(std::string(token))))
            {
                return PlayerList();
            }
            return Unauthorized(json_responce::ErrorJson("unknownToken","Player token has not been found"));
        }
        if (isJoinGameReq())
        {
            return NotAllowed(json_responce::ErrorJson("invalidMethod","Only POST method is expected"), http::verb::post,http::verb::get);
        }
        return BadRequest();
    }

    std::variant <StringResponse, FileResponse> get_handler::HandleFileRequest()
    {
        auto target = _req.target() == "/" ? "index.html" : _req.target().substr(1);
        auto response_file = wwwroot.try_get(target.data());
        if (response_file)
        {
            auto res = std::move((*response_file));
            return res;
        }
        return HandleNotFound();
    }


    StringResponse get_handler::PlayerList()
    {
        std::string  body = json_responce::to_json(game_.GetPLayers());
        return Ok(body);
    }


    StringResponse get_handler::Maps()
    {
        auto maps =  game_.GetMaps();
        auto maps_json_str = json_responce::to_json(maps);
        auto resp  = Ok(maps_json_str);

        return resp;
    }
    StringResponse get_handler::Map(const std::string& map_id)
    {
        auto map  = game_.FindMap(model::Map::Id(map_id));
        if (map)
        {
            std::string map_json = json_responce::to_json(*map);
            auto  resp =  Ok(map_json);
            resp.keep_alive(_req.keep_alive());
            return resp;
        }
        auto resp =  NotFound(json_responce::ErrorJson("mapNotFound","Map not found"));
        resp.keep_alive(_req.keep_alive());
        return resp;
    }

    StringResponse get_handler::HandleNotFound()
    {
        const auto text_response = [this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, _req.version(), _req.keep_alive(),ContentType::APPLICATION_JSON);
        };
        return text_response(http::status::not_found,"ERROR 404 Not found");
    }


    StringResponse get_handler::MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                                   bool keep_alive,
                                                   std::string_view content_type)
    {
        StringResponse response(status, http_version);
        response.set(http::field::content_type, content_type.data());
        response.body() = body;
        response.content_length(body.size());
        response.keep_alive(keep_alive);
        return response;
    }
}

