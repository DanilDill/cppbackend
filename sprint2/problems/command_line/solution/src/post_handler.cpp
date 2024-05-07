
#include <string_view>
#include "post_handler.h"
#include <boost/json.hpp>

#include "json_response.h"
#include "authorization_checker.h"
namespace http_handler
{
    post_handler::post_handler(StringRequest&& request, model::Game& game, file::file_loader& root):
    default_handler(std::move(request)),game_(game),wwwroot(root)
    {
    }

    std::pair<std::string,std::string> post_handler::parse()
    {
        auto json_data =  boost::json::parse(_req.body().data());

        std::string map_id = "";
        std::string user_name = "";
        if (auto user_name_iter = json_data.as_object().find("userName"); user_name_iter != json_data.as_object().end())
        {
            user_name = user_name_iter->value().as_string().data();
        }
        if ( auto map_id_iter =  json_data.as_object().find("mapId"); map_id_iter != json_data.as_object().end())
        {
            map_id = map_id_iter->value().as_string().data();
        }
        return std::make_pair(std::move(user_name),std::move(map_id));
    }

   std::variant <StringResponse, FileResponse> post_handler::HandlePlayerAction()
{
    AuthorizationChecker checker(_req,game_);
    auto auth_check = checker.check();
    if (auth_check)
    {
        return Unauthorized(*auth_check);
    }
    else
    {
        if (_req.body().empty())
        {
            return BadRequest(json_responce::ErrorJson("invalidArgument","Failed to parse action"));
        }
        auto json_data =  boost::json::parse(_req.body().data());
        if(auto moveDirectionIter =  json_data.as_object().find("move"); moveDirectionIter != json_data.as_object().end())
        {
            std::string action = moveDirectionIter->value().as_string().data();
            model::Direction direction = model::to_direction(action);
            if (direction == model::Direction::UNKNOWN)
            {
                return BadRequest(json_responce::ErrorJson("invalidArgument","Failed to parse action"));
            }
            auto  token = checker.getLastToken();
            game_.SetDirection(token, direction);
            return Ok("{}");
        }
        return BadRequest(json_responce::ErrorJson("invalidArgument","Failed to parse action"));
    }
}


    std::variant <StringResponse, FileResponse> post_handler::HandleJoinGame()
    {
        if (_req.body().empty())
        {
            return BadRequest(json_responce::ErrorJson("invalidArgument","Join game request parse error"));
        }
        auto gamer  = parse();
        if (gamer.first.empty())
        {
            return BadRequest(json_responce::ErrorJson("invalidArgument","Invalid name"));
        }
        if (gamer.second.empty())
        {
            return BadRequest( json_responce::ErrorJson("invalidArgument","Invalid map"));
        }
        auto map = game_.FindMap(model::Map::Id(gamer.second));
        if (!map)
        {
            return NotFound(json_responce::ErrorJson("mapNotFound","Map not found"));
        }
        auto token =  PlayerTokens().make_token();
        auto  id =  game_.AddPlayer(token,gamer.first,model::Map::Id(gamer.second));
        return  Ok(json_responce::AuthTokenJson(*token,id));
    }

    std::variant <StringResponse, FileResponse> post_handler::HandleGameTick()
    {
        if (!game_.hasTicker())
        {
            if (_req.body().empty())
            {
                return BadRequest(json_responce::ErrorJson("invalidArgument","Failed to parse tick request JSON"));
            }
            auto json_data =  boost::json::parse(_req.body().data());
            size_t tick_ms;
            if (auto tick_iter = json_data.as_object().find("timeDelta"); tick_iter != json_data.as_object().end() && tick_iter->value().is_int64())
            {
                tick_ms = tick_iter->value().as_int64();
                game_.Tick(std::chrono::milliseconds(tick_ms));
                return Ok("{}");
            }
            else
            {
                return BadRequest(json_responce::ErrorJson("invalidArgument","Failed to parse tick request JSON"));
            }
        }
        return BadRequest(json_responce::ErrorJson("badRequest","Invalid endpoint"));
    }


}