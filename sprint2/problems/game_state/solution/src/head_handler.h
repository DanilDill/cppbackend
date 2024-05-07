#pragma once
#include "staticfile_loader.h"
#include "default_handler.h"
#include "model.h"
#include "authorization_checker.h"

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
                auto auth_check = AuthorizationChecker(_req,game_).check();
                if (auth_check)
                {
                    return Unauthorized();
                }
                else
                {
                    return Ok();
                }
            }
            if (isJoinGameReq())
            {
                return NotAllowed(request_right[RequestTargets::GAME_JOIN]);
            }
            if (isGameStateReq())
            {
                auto auth_check = AuthorizationChecker(_req,game_).check();
                if (auth_check)
                {
                    return Unauthorized();
                }
                else
                {
                    return Ok();
                }
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
