#pragma once
#include "default_handler.h"
#include "staticfile_loader.h"

#include "model.h"

namespace http_handler
{

    class get_handler: public default_handler
    {
    public:
        get_handler(StringRequest&&request, model::Game& game, file::file_loader& root);

    protected:
        virtual std::variant <StringResponse, FileResponse> HandleMapsList()override;
        virtual std::variant <StringResponse, FileResponse> HandleMapId()override;
        virtual std::variant <StringResponse, FileResponse> HandlePlayerList()override;
        virtual std::variant <StringResponse, FileResponse> HandleGameState()override;
        virtual std::variant <StringResponse, FileResponse> HandleFileRequest()override;

        virtual StringResponse PlayerList();
        virtual StringResponse PlayerState();
        virtual StringResponse Maps();
        virtual StringResponse Map(const std::string& map_id);
        virtual StringResponse HandleNotFound();

    protected:
        StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                          bool keep_alive,
                                          std::string_view content_type = ContentType::TEXT_HTML);


    protected:
       model::Game& game_;
       file::file_loader& wwwroot;
    };



}
