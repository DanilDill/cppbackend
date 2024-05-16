#include "head_handler.h"

namespace http_handler
{

    HeadHandler::HeadHandler(StringRequest&&request, model::Game& game, file::file_loader& root):
            GetHandler(std::move(request), game, root)
    {}

    std::variant <StringResponse, FileResponse> HeadHandler::HandlePlayerAction()
{
    return NotAllowed(request_right[RequestTargets::GAME_ACTION]);
}
    std::variant <StringResponse, FileResponse> HeadHandler::HandleJoinGame()
    {
        return NotAllowed(request_right[RequestTargets::GAME_JOIN]);
    }
    std::variant <StringResponse, FileResponse> HeadHandler::HandleFileRequest()
    {
        auto target = _req.target() == "/" ? "index.html" : _req.target().substr(1);
        auto response_file = wwwroot.try_get(target.data());
        if (response_file)
        {
            return Ok();
        }
        return HandleNotFound();
    }

     StringResponse HeadHandler::HandleNotFound()
    {
        const auto text_response = [this](http::status status, std::string_view text)
        {
            return MakeStringResponse(status, text, _req.version(), _req.keep_alive(),ContentType::TEXT_PLAIN);
        };
        return text_response(http::status::not_found,"");
    }
    StringResponse HeadHandler::Unauthorized(std::string_view body)
    {
        return DefaultHandler::Unauthorized("");
    }

    StringResponse HeadHandler::PlayerList()
    {
        return Ok();
    }
    StringResponse HeadHandler::PlayerState()
    {
        return Ok();
    }
    StringResponse HeadHandler::Maps()
    {
        return Ok();
    }
    StringResponse HeadHandler::Map(const std::string& map_id)
    {
        auto map  = game_.FindMap(model::Map::Id(map_id));
        if (map)
        {
        return Ok();
        }
        return NotFound();
    }
}