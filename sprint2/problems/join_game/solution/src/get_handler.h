#include <variant>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include "content_type.h"
#include "staticfile_loader.h"
#include "model.h"
#include "default_handler.h"
namespace http_handler
{
    class get_handler: public default_handler
    {
    public:
        get_handler(StringRequest&&request, model::Game& game, file::file_loader& root);

    protected:
        virtual std::variant <StringResponse, FileResponse> HandleMapRequest()override;
        virtual std::variant <StringResponse, FileResponse> HandleGameRequest()override;
        virtual std::variant <StringResponse, FileResponse> HandleFileRequest()override;
    private:
        StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                          bool keep_alive,
                                          std::string_view content_type = ContentType::TEXT_HTML);

        StringResponse PlayerList();
        StringResponse Maps();
        StringResponse Unathorized();
        StringResponse Map(const std::string& map_id);
        StringResponse HandleNotFound();
        StringResponse BadRequest();
        StringResponse PLayerNotFound();
        StringResponse HandleNotAllowed();
    private:
       model::Game& game_;
       file::file_loader& wwwroot;
    };



}
