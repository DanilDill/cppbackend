#pragma once
#include "default_handler.h"
#include "staticfile_loader.h"

#include "model.h"

namespace http_handler
{

    class GetHandler: public DefaultHandler
    {
    public:
        GetHandler(StringRequest&&request, model::Game& game, file::file_loader& root);

    protected:
        std::variant <StringResponse, FileResponse> HandleMapsList()override;
        std::variant <StringResponse, FileResponse> HandleMapId()override;
        std::variant <StringResponse, FileResponse> HandlePlayerList()override;
        std::variant <StringResponse, FileResponse> HandleGameState()override;
        std::variant <StringResponse, FileResponse> HandleFileRequest()override;

        virtual StringResponse PlayerList();
        virtual StringResponse GameState();
        virtual StringResponse Maps();
        virtual StringResponse Map(const std::string& map_id);
        virtual StringResponse HandleNotFound();

    protected:
        static StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                          bool keep_alive,
                                          std::string_view content_type = ContentType::TEXT_HTML);


    protected:
       model::Game& game_;
       file::file_loader& wwwroot;
    };



}
