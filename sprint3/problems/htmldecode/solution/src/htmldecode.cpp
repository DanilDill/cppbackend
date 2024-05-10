#include "htmldecode.h"
#include "html_coder.hpp"
#include <sstream>
std::string HtmlDecode(std::string_view str) {

    HtmlCoder html_decoder;

    std::string decode = (std::stringstream ()<<str).str();

    html_decoder.decode(decode);
    return decode;
}
