#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/json.hpp>
#include "model.h"
#include "staticfile_loader.h"
namespace http_handler
{
    namespace beast = boost::beast;
    namespace http = beast::http;
    using StringResponse = http::response<http::string_body>;
    using FileResponse = http::response<http::file_body>;
    using StringRequest = http::request<http::string_body>;

    class post_handler
    {
    public:
        post_handler(StringRequest&& request, model::Game& game, file::file_loader& root);
        StringResponse execute();

    private:
        std::pair<std::string,std::string> parse();
    private:
        StringRequest _req;
        model::Game& game_;
        file::file_loader& wwwroot;
    };
}
