#define BOOST_TEST_MODULE urlencode tests
#include <boost/test/unit_test.hpp>

#include "../src/urldecode.h"

BOOST_AUTO_TEST_CASE(UrlDecode_tests) {
    using namespace std::literals;
    BOOST_TEST(UrlDecode(""sv) == ""s);
    BOOST_TEST(UrlDecode("%23"sv) =="#"s);
    BOOST_TEST(UrlDecode("%7D"sv) =="}"s);
    BOOST_TEST(UrlDecode("%7d"sv) =="}"s);
    BOOST_TEST(UrlDecode("%xj"sv) =="%xj"s);
    BOOST_TEST(UrlDecode("Hello+World!"sv) == "Hello World!"s);
    BOOST_TEST(UrlDecode("Hello+World%20%21"sv) == "Hello World !"s);
    BOOST_TEST(UrlDecode("%"sv) =="%"s);


    // Напишите остальные тесты для функции UrlDecode самостоятельно
}