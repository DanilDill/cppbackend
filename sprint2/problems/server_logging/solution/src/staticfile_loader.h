#pragma once
#include <filesystem>
#include <optional>
#include <boost/beast.hpp>
namespace file
{
    using namespace std::literals;
    namespace fs = std::filesystem;
    namespace beast = boost::beast;
    namespace http = beast::http;

    enum class MIMETYPE
    {
        HTML,
        CSS,
        TXT,
        JS,
        PNG,
        SVG,
        OCTET_STREAM
    };
    class file_loader
    {
    public:
        file_loader(const std::filesystem::path& root);
        std::optional<http::response<http::file_body>> try_get(const std::filesystem::path& filepath);
        static MIMETYPE get(const std::string& extension);
        static std::string_view to_ContentType (MIMETYPE mimetype);
    private:
        bool isSubpath(const std::filesystem::path&);
    private:
        fs::path _root;
    };

}


