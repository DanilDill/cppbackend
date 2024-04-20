#include <variant>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include "content_type.h"
#include "staticfile_loader.h"
#include "model.h"
namespace http_handler
{
    namespace beast = boost::beast;
    namespace http = beast::http;
    using StringResponse = http::response<http::string_body>;
    using FileResponse = http::response<http::file_body>;
    using StringRequest = http::request<http::string_body>;

    class get_handler
    {
    public:
        get_handler(StringRequest&&request, model::Game& game, file::file_loader& root);
        std::variant <StringResponse, FileResponse> execute();

    private:
        StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                          bool keep_alive,
                                          std::string_view content_type = ContentType::TEXT_HTML);

        StringResponse HandleGetMaps();
        StringResponse HandleGetMap(const std::string& map_id);
        StringResponse HandleNotFound();
        StringResponse HandleBadRequest();
    private:
       StringRequest _req;
       model::Game& game_;
       file::file_loader& wwwroot;
    };



}
