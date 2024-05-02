#pragma once

#include "staticfile_loader.h"
#include "default_handler.h"
#include "model.h"
#include "Tokenizer.h"

namespace http_handler
{
    class post_handler : public default_handler
    {
    public:
        post_handler(StringRequest&& request, model::Game& game, file::file_loader& root);

    private:
        std::pair<std::string,std::string> parse();


    protected:
      //  virtual std::variant <StringResponse, FileResponse> HandleApiRequest();

        virtual std::variant <StringResponse, FileResponse> HandleGameRequest()
        {
            if (isGamePlayerListReq())
            {
                return NotAllowed(json_responce::ErrorJson("invalidMethod","Invalid method"),request_right[RequestTargets::GAME_PLAYERS_REQ]);
            }
            if (isJoinGameReq())
            {
                auto data = _req.body().data();
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
            if (isGameStateReq())
            {
                return NotAllowed(json_responce::ErrorJson("invalidMethod","Invalid method"), request_right[RequestTargets::GAME_STATE]);
            }
            return BadRequest(json_responce::ErrorJson("invalidArgument","invalidRequest"));
        }
    private:
        model::Game& game_;
        file::file_loader& wwwroot;
    };
}
