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
        virtual std::variant <StringResponse, FileResponse> HandleJoinGame()override;
        virtual std::variant <StringResponse, FileResponse> HandlePlayerAction()override;
        virtual std::variant <StringResponse, FileResponse> HandleGameTick()override;
    private:
        model::Game& game_;
        file::file_loader& wwwroot;
    };
}
