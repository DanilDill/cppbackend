#pragma once
#include <string_view>
struct RequestTargets
{
    RequestTargets()=delete;
    static constexpr std::string_view API_REQ = "/api/";
    static constexpr std::string_view MAP_REQ = "/api/v1/maps";
    static constexpr std::string_view MAP_ID_REQ = "/api/v1/maps/";
    static constexpr std::string_view GAME_REQ = "/api/v1/game/";
    static constexpr std::string_view GAME_PLAYERS_REQ = "/api/v1/game/players";
    static constexpr std::string_view GAME_JOIN = "/api/v1/game/join";

};