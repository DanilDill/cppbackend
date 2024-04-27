#pragma once
#include <random>
#include "tagged.h"
#include <cassert>
#include <sstream>
#include <iomanip>
namespace detail
{
    struct TokenTag {};
}  // namespace detail

using Token = util::Tagged<std::string, detail::TokenTag>;

class PlayerTokens
{
private:
    std::random_device random_device_;
    std::mt19937_64 generator1_{[this] {
        std::uniform_int_distribution<std::mt19937_64::result_type> dist;
        return dist(random_device_);
    }()};

    std::mt19937_64 generator2_{[this] {
        std::uniform_int_distribution<std::mt19937_64::result_type> dist;
        return dist(random_device_);
    }()};
public:
    Token make_token()
    {
        std::stringstream ss;
        auto word1 = generator1_();
        auto word2 = generator2_();
        auto word3 = generator1_();
        ss << std::hex << word1 << word3 << word2;


        auto string_token = ss.str();
        string_token.resize(32);
        return  Token(string_token);
    }

};