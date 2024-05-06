#pragma once
#define BOOST_BEAST_USE_STD_STRING_VIEW
#include <string_view>
#include <boost/beast.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
using namespace std::literals;

