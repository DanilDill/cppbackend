#include <gtest/gtest.h>

#include "../src/urlencode.h"

using namespace std::literals;

TEST(UrlEncodeTestSuite, OrdinaryCharsAreNotEncoded) {
    EXPECT_EQ(UrlEncode("hello"sv), "hello"s);
    EXPECT_EQ(UrlEncode(""sv) , ""s);
    EXPECT_EQ(UrlEncode("#"sv) ,"%23"s);
    EXPECT_EQ(UrlEncode("}"sv) ,"%7D"s);

    EXPECT_EQ(UrlEncode("%xj"sv) ,"%25xj"s);
    EXPECT_EQ(UrlEncode("Hello World!"sv) , "Hello%20World%21"s);
    EXPECT_EQ(UrlEncode("Hello World !"sv) , "Hello%20World%20%21"s);
    EXPECT_EQ(UrlEncode("%"sv) ,"%25"s);
}

/* Напишите остальные тесты самостоятельно */
