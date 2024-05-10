#include "urldecode.h"

#include <charconv>
#include <stdexcept>
#include <curl/curl.h>
std::string UrlDecode(std::string_view str) {

    int output_length;
    const auto decoded_value = curl_easy_unescape(nullptr, str.data(), static_cast<int>(str.length()), &output_length);
    std::string result(decoded_value, output_length);
    curl_free(decoded_value);
    for (auto& str_char : result)
    {
        if (str_char =='+')
        {
            str_char=' ';
        }
    }
    return result;
}


