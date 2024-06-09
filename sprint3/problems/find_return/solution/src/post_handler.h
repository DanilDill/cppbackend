#pragma once

#include "staticfile_loader.h"
#include "default_handler.h"
#include "model/game.h"
#include "model/tokenizer.h"

namespace http_handler
{
    class PostHandler : public DefaultHandler
    {
    public:
        PostHandler(StringRequest&& request, model::Game& game, file::file_loader& root);

    private:
        std::pair<std::string,std::string> parse();

    protected:
        std::variant <StringResponse, FileResponse> HandleJoinGame()override;
        std::variant <StringResponse, FileResponse> HandlePlayerAction()override;
        std::variant <StringResponse, FileResponse> HandleGameTick()override;
    private:
        model::Game& game_;
        file::file_loader& wwwroot;
    };
}
