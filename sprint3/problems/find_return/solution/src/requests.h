#pragma once
#include <string_view>
#include <unordered_map>

struct RequestTargets
{
    RequestTargets()=delete;
    static constexpr std::string_view API_REQ = "/api/";
    static constexpr std::string_view MAP_REQ = "/api/v1/maps";
    static constexpr std::string_view MAP_ID_REQ = "/api/v1/maps/";
    static constexpr std::string_view GAME_REQ = "/api/v1/game/";
    static constexpr std::string_view GAME_PLAYERS_REQ = "/api/v1/game/players";
    static constexpr std::string_view GAME_JOIN = "/api/v1/game/join";
    static constexpr std::string_view GAME_STATE = "/api/v1/game/state";
    static constexpr std::string_view GAME_ACTION = "/api/v1/game/player/action";
    static constexpr std::string_view GAME_TICK = "/api/v1/game/tick";

};

static std::unordered_map<std::string_view,std::string_view> request_right
{
        { RequestTargets::MAP_REQ,"GET, HEAD"sv },
        { RequestTargets::MAP_ID_REQ,"GET, HEAD"sv },
        { RequestTargets::GAME_PLAYERS_REQ,"GET, HEAD"sv },
        { RequestTargets::GAME_JOIN,"POST" },
        { RequestTargets::GAME_STATE, "GET, HEAD"sv},
        { RequestTargets::GAME_ACTION,"POST"},
        { RequestTargets::GAME_TICK,"POST"}
};
