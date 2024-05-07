
#include "staticfile_loader.h"
#include "content_type.h"
#include <boost/algorithm/string.hpp>
#include <iostream>
namespace file
{
    file_loader::file_loader(const std::filesystem::path& path):
    _root(fs::weakly_canonical(path)){}

    std::optional<http::response<http::file_body>> file_loader::try_get(const std::filesystem::path& filepath)
    {
        auto  canonical_path =  _root / fs::weakly_canonical(filepath);
        if (isSubpath(canonical_path))
        {
            http::file_body::value_type file;
            if (boost::system::error_code ec; file.open(canonical_path.c_str(),beast::file_mode::read, ec),ec)
            {
                std::cout << "cannot load file " <<filepath << ":\n "<< ec.what();
                return std::nullopt;
            }
            else
            {
                http::response<http::file_body> response;
                response.version(11);
                response.result(http::status::ok);
                auto content_type = to_ContentType(get(boost::algorithm::to_lower_copy(canonical_path.extension().string())));
                response.insert(http::field::content_type, content_type);
                response.body() = std::move(file);
                response.prepare_payload();
                return  response;
            }
        }
        return std::nullopt;
    }

    MIMETYPE file_loader::get(const std::string& extension)
    {

        if (extension == ".html" or extension == ".htm")
        {
            return MIMETYPE::HTML;
        }
        if (extension == ".css" )
        {
            return MIMETYPE::CSS;
        }
        if (extension == ".txt" )
        {
            return MIMETYPE::TXT;
        }
        if (extension == ".js")
        {
            return MIMETYPE::JS;
        }
        if(extension == ".png")
        {
            return  MIMETYPE::PNG;
        }
        if (extension == ".svg" or extension == "svgz")
        {
            return MIMETYPE::SVG;
        }

        return MIMETYPE::OCTET_STREAM;
    }
    std::string_view file_loader::to_ContentType (MIMETYPE mimetype)
    {
        switch (mimetype)
        {
            case MIMETYPE::HTML:
                return ContentType::TEXT_HTML;
            case MIMETYPE::CSS:
                return ContentType::TEXT_CSS;
            case MIMETYPE::JS:
                return ContentType::TEXT_JAVASCRIPT;
            case MIMETYPE::PNG:
                return ContentType::IMAGE_PNG;
            case MIMETYPE::SVG:
                return ContentType::IMAGE_SVG_XML;
            case MIMETYPE::TXT:
                return ContentType::TEXT_PLAIN;
            case MIMETYPE::OCTET_STREAM:
                return ContentType::APPLICATION_OCTETSTREAM;
        }
    }
    bool  file_loader::isSubpath(const std::filesystem::path& path)
    {

        for (auto r = _root.begin(), p = path.begin(); r != _root.end(); ++r, ++p)
        {
            if (p == path.end() || *p != *r)
            {
                return false;
            }
        }
        return true;
    }
}
