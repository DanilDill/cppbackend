
#include "get_handler.h"
#include "staticfile_loader.h"
#include "authorization_checker.h"
namespace http_handler {
    GetHandler::GetHandler(StringRequest &&request, model::Game &game, file::file_loader &root):
            DefaultHandler(std::forward<decltype(request)>(request)), game_(game), wwwroot(root) {}

    std::variant <StringResponse, FileResponse> GetHandler::HandleMapsList()
    {
        return Maps();
    }
    std::variant <StringResponse, FileResponse> GetHandler::HandleMapId()
    {
        std::string map_id(_req.target().substr("/api/v1/maps/"sv.size()));
        return Map(map_id);
    }

    std::variant <StringResponse, FileResponse> GetHandler::HandlePlayerList()
    {
        auto auth_check = AuthorizationChecker(_req,game_).check();
        if (auth_check)
        {
            return Unauthorized(*auth_check);
        }
        else
        {
            return PlayerList();
        }
    }

    std::variant <StringResponse, FileResponse> GetHandler::HandleGameState()
    {
        auto auth_check = AuthorizationChecker(_req,game_).check();
        if (auth_check)
        {
            return Unauthorized(*auth_check);
        }
        else
        {
            return GameState();
        }
    }


    std::variant <StringResponse, FileResponse> GetHandler::HandleFileRequest()
    {
        std::string_view target = _req.target() == "/" ? "index.html" : _req.target().substr(1);
        auto response_file = wwwroot.try_get(target);
        if (response_file)
        {
            auto res = std::move((*response_file));
            return res;
        }
        return HandleNotFound();
    }


    StringResponse GetHandler::PlayerList()
    {
        std::string  body = json_responce::to_json(game_.GetPLayers());
        return Ok(body);
    }

    StringResponse GetHandler::GameState()
    {
        boost::json::object obj;
        auto players = json_responce::to_json_obj(game_.GetPLayers(), true);
        obj["_players"] = players;
        auto lost_objects = json_responce::to_json_obj(game_.getLostObjcets());
        obj["lostObjects"] = lost_objects;
        return Ok(boost::json::serialize(obj));
    }

    StringResponse GetHandler::Maps()
    {
        auto maps =  game_.GetMaps();
        auto maps_json_str = json_responce::to_json(maps);
        auto resp  = Ok(maps_json_str);

        return resp;
    }
    StringResponse GetHandler::Map(const std::string& map_id)
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

    StringResponse GetHandler::HandleNotFound()
    {
        const auto text_response = [this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, _req.version(), _req.keep_alive(),ContentType::TEXT_PLAIN);
        };
        return text_response(http::status::not_found,"ERROR 404 Not found");
    }


    StringResponse GetHandler::MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
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

