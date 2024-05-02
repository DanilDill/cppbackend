#pragma once
#include "staticfile_loader.h"
#include "get_handler.h"
#include "model.h"
#include "authorization_checker.h"

namespace http_handler
{

    class head_handler : public get_handler
    {
    public:
        head_handler(StringRequest&&request, model::Game& game, file::file_loader& root);

    protected:
        virtual std::variant <StringResponse, FileResponse> HandlePlayerAction()override;
        virtual std::variant <StringResponse, FileResponse> HandleJoinGame()override;
        virtual std::variant <StringResponse, FileResponse> HandleFileRequest()override;
        virtual StringResponse Unauthorized(std::string_view body)override;
        virtual StringResponse PlayerList()override;
        virtual StringResponse PlayerState()override;
        virtual StringResponse Maps()override;
        virtual StringResponse Map(const std::string& map_id)override;
        virtual StringResponse HandleNotFound()override;
    };
}
