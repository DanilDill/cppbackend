
#include "post_handler.h"
#include <boost/json.hpp>
#include "Tokenizer.h"
#include "json_response.h"
namespace http_handler
{
    post_handler::post_handler(StringRequest&& request, model::Game& game, file::file_loader& root):
    _req(request),game_(game),wwwroot(root)
    {
    }

    StringResponse  post_handler::MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                      std::string_view content_type)
    {
        StringResponse response(status, http_version);
        response.set(http::field::content_type, content_type);
        response.content_length(body.size());
        response.set(http::field::cache_control,"no-cache");
        response.body() = body;
        return response;
    }
    std::pair<std::string,std::string> post_handler::parse()
    {
        auto json_data =  boost::json::parse(_req.body().data());

        auto map_id_iter =  json_data.as_object().find("mapId");
        std::string map_id = "";
        std::string user_name = "";
        if (auto user_name_iter = json_data.as_object().find("userName"); user_name_iter != json_data.as_object().end())
        {
            user_name = user_name_iter->value().as_string().data();
        }
        if ( map_id_iter =  json_data.as_object().find("mapId"); map_id_iter != json_data.as_object().end())
        {
            map_id = map_id_iter->value().as_string().data();
        }
        return std::make_pair(std::move(user_name),std::move(map_id));

    }

    StringResponse post_handler::execute()
    {
        std::string_view target = _req.target();
        auto response = [this](http::status status, std::string_view body)
        {
            return MakeStringResponse(status,body,_req.version());
        };
        if (target == "/api/v1/game/join")
        {
            if (_req[http::field::content_type] == "application/json")
            {
                if (_req.body().data() == "")
                {
                   return response(http::status::bad_request,json_responce::ErrorJson("invalidArgument","Join game request parse error"));
                }
                 auto gamer  = parse();
                if (gamer.first.empty())
                {
                   return response(http::status::bad_request, json_responce::ErrorJson("invalidArgument","Invalid name"));
                }
                if (gamer.second.empty())
                {
                   return response(http::status::bad_request, json_responce::ErrorJson("invalidArgument","Invalid map"));
                }
                auto map = game_.FindMap(model::Map::Id(gamer.second));
                if (!map)
                {
                    // {"code": "mapNotFound", "message": "Map not found"}
                   return response(http::status::not_found, json_responce::ErrorJson("mapNotFound","Map not found"));
                }
                auto token =  PlayerTokens().make_token();
                auto  id =  game_.AddPlayer(token,gamer.first,model::Map::Id(gamer.second));
                return  response(http::status::ok, json_responce::AuthTokenJson(*token,id));
            }
           return response(http::status::bad_request, json_responce::ErrorJson("WrongContentType","Head Content-Type must be \"application/json\""));
        }
        return response(http::status::bad_request, json_responce::ErrorJson("invalid","unsupported target"));
    }
}