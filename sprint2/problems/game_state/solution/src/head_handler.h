#pragma once
#include "staticfile_loader.h"
#include "default_handler.h"
#include "model.h"


namespace http_handler
{

    class head_handler : public default_handler
    {
    public:
        head_handler(StringRequest&&request, model::Game& game, file::file_loader& root);

    protected:
        virtual std::variant <StringResponse, FileResponse> HandleMapRequest()override
        {
            if (isMapListReq())
            {
                return Ok();
            }
            if (isMapIdReq())
            {
                std::string map_id(_req.target().substr("/api/v1/maps/"sv.size()));
                return Map(map_id);
            }
            return BadRequest();
        }
        virtual std::variant <StringResponse, FileResponse> HandleGameRequest()override
        {
            if (isGamePlayerListReq())
            {
                auto auth_str = _req[http::field::authorization];
                if (auth_str == "" or auth_str.substr("Bearer"sv.size()) == "")
                {
                    return Unauthorized();
                }
                auto token = auth_str.substr("Bearer "sv.size());

                if(game_.FindPlayer(Token(std::string(token))))
                {
                    return Ok();
                }
                return Unauthorized();
            }
            if (isJoinGameReq())
            {
                return NotAllowed(request_right[RequestTargets::GAME_JOIN]);
            }
            if (isGameStateReq())
            {

            }
            return BadRequest();
        }
        virtual std::variant <StringResponse, FileResponse> HandleFileRequest()override
        {
            auto target = _req.target() == "/" ? "index.html" : _req.target().substr(1);
            auto response_file = wwwroot.try_get(target.data());
            if (response_file)
            {
                return Ok();
            }
            return NotFound();
        }
    private:

        StringResponse Map(const std::string& map_id)
        {
            auto map  = game_.FindMap(model::Map::Id(map_id));
            if (map)
            {
                return Ok();
            }
            return NotFound();
        }

    private:
        model::Game& game_;
        file::file_loader& wwwroot;
    };
}
