#pragma once
#include <string>
namespace model
{
    enum  Direction
    {
        NORTH ,
        SOUTH,
        WEST ,
        EAST,
        STOP,
        UNKNOWN
    };
    std::string to_string(Direction direction);
    Direction to_direction(const std::string& string);


}
