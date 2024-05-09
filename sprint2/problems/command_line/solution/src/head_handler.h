#pragma once
#include "staticfile_loader.h"
#include "get_handler.h"
#include "model.h"
#include "authorization_checker.h"

namespace http_handler
{

    class HeadHandler : public GetHandler
    {
    public:
        HeadHandler(StringRequest&&request, model::Game& game, file::file_loader& root);

    protected:
        std::variant <StringResponse, FileResponse> HandlePlayerAction()override;
        std::variant <StringResponse, FileResponse> HandleJoinGame()override;
        std::variant <StringResponse, FileResponse> HandleFileRequest()override;
        StringResponse Unauthorized(std::string_view body)override;
        StringResponse PlayerList()override;
        StringResponse PlayerState()override;
        StringResponse Maps()override;
        StringResponse Map(const std::string& map_id)override;
        StringResponse HandleNotFound()override;
    };
}
