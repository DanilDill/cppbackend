#pragma once
#include <cmath>
namespace model
{
    using Dimension = int;
    using Coord = Dimension;
    using Coordf = double;

    struct Point {
        Coord x, y;
    };
    struct Pointf
    {
        Coordf x, y;
        bool operator<=(const Pointf& other) const{ return x <= other.x and y<= other.y;};
        static double  measure_distance(const Pointf& first, const Pointf second)
        {
            double squared_distance = pow(second.x - first.x, 2) + pow(second.y - first.y, 2);
            return sqrt(squared_distance);
        }
    };

}
