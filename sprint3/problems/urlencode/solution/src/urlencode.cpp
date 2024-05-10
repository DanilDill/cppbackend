#include "urlencode.h"
#include <curl/curl.h>

std::string UrlEncode(std::string_view str)
{
    const auto encoded_value = curl_easy_escape(nullptr, str.data(), static_cast<int>(str.length()));
    std::string result(encoded_value);
    curl_free(encoded_value);
    return result;
    for(auto& str_char: result)
    {
        if(str_char == ' ')
        {
            str_char = '+';
        }
    }
    return {str.begin(), str.end()};
}
