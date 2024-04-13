#pragma once
#include <string_view>
using namespace std::literals;
struct ContentType {

    ContentType() = delete;
    constexpr static std::string_view TEXT_HTML = "text/html"sv;
    constexpr static std::string_view APPLICATION_JSON = "application/json"sv;
    constexpr static std::string_view TEXT_CSS = "text/css";
    constexpr static std::string_view TEXT_JAVASCRIPT= "text/javascript";
    constexpr static std::string_view IMAGE_PNG = "image/png";
    constexpr static std::string_view IMAGE_SVG_XML = "image/svg+xml";
    constexpr static std::string_view TEXT_PLAIN = "text/plain";
    constexpr static std::string_view APPLICATION_OCTETSTREAM = "application/octet-stream";
};