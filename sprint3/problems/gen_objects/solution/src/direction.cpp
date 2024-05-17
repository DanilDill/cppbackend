#include "direction.h"
namespace model
{
    std::string to_string(Direction direction)
    {
        switch (direction)
        {
            case Direction::NORTH:
                return "U";
            case Direction::SOUTH:
                return "D";
            case Direction::WEST:
                return "L";
            case Direction::EAST:
                return "R";
            case Direction::STOP:
                return "";
            default:
                return "unknown";
        }
    }

    Direction to_direction(const std::string& string)
    {
        if (string =="U")
        {
            return Direction::NORTH;
        }
        if (string =="D")
        {
            return Direction::SOUTH;
        }
        if (string =="L")
        {
            return Direction::WEST;
        }
        if (string =="R")
        {
            return Direction::EAST;
        }
        if (string =="")
        {
            return Direction::STOP;
        }
        return Direction::UNKNOWN;
    }

}