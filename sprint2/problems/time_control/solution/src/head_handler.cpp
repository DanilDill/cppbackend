#include "head_handler.h"

namespace http_handler
{

    head_handler::head_handler(StringRequest&&request, model::Game& game, file::file_loader& root):
            get_handler(std::move(request),game,root)
    {}

    std::variant <StringResponse, FileResponse> head_handler::HandlePlayerAction()
{
    return NotAllowed(request_right[RequestTargets::GAME_ACTION]);
}
    std::variant <StringResponse, FileResponse> head_handler::HandleJoinGame()
    {
        return NotAllowed(request_right[RequestTargets::GAME_JOIN]);
    }
    std::variant <StringResponse, FileResponse> head_handler::HandleFileRequest()
    {
        auto target = _req.target() == "/" ? "index.html" : _req.target().substr(1);
        auto response_file = wwwroot.try_get(target.data());
        if (response_file)
        {
            return Ok();
        }
        return HandleNotFound();
    }

     StringResponse head_handler::HandleNotFound()
    {
        const auto text_response = [this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, _req.version(), _req.keep_alive(),ContentType::TEXT_PLAIN);
        };
        return text_response(http::status::not_found,"");
    }
    StringResponse head_handler::Unauthorized(std::string_view body)
    {
        return default_handler::Unauthorized("");
    }

    StringResponse head_handler::PlayerList()
    {
        return Ok();
    }
    StringResponse head_handler::PlayerState()
    {
        return Ok();
    }
    StringResponse head_handler::Maps()
    {
        return Ok();
    }
    StringResponse head_handler::Map(const std::string& map_id)
    {
        auto map  = game_.FindMap(model::Map::Id(map_id));
        if (map)
        {
        return Ok();
        }
        return NotFound();
    }
}