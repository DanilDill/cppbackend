#include "road.h"
#include <algorithm>
namespace model
{
Road::Road(HorizontalTag, Point start, Coord end_x) noexcept:
start_{start},end_{end_x, start.y}
{
    makeCorners();
}

Road::Road(VerticalTag, Point start, Coord end_y) noexcept:
start_{start}, end_{start.x, end_y}
{
    makeCorners();
}
bool Road::IsOnTheRoad(Pointf point) const
{
    return left_bottom_corner <= point && point <= right_top_corner;
}

bool  Road::IsHorizontal() const noexcept {
    return start_.y == end_.y;
}

bool  Road::IsVertical() const noexcept {
    return start_.x == end_.x;
}

Point  Road::GetStart() const noexcept {
    return start_;
}

Point  Road::GetEnd() const noexcept {
    return end_;
}
Pointf  Road::bound_to_the_road(Pointf point)
{
    double new_x = bound(left_bottom_corner.x, right_top_corner.x, point.x);
    double new_y = bound(left_bottom_corner.y, right_top_corner.y, point.y);
    return Pointf{new_x, new_y};
}
double  Road::bound(double bound_1, double bound_2,double value)
{
    double lower = std::min(bound_1, bound_2);
    double upper = std::max(bound_1, bound_2);

    double result = std::min(upper, value);
    result = std::max(lower, result);
    return result;
}

void  Road::makeCorners()
{
    double left_bottom_x = std::min(start_.x,end_.x) - width;
    double left_bottom_y = std::min(start_.y, end_.y) - width;
    double right_top_x = std::max(start_.x, end_.x) + width;
    double right_top_y = std::max(start_.y, end_.y) + width;
    left_bottom_corner = {left_bottom_x, left_bottom_y};
    right_top_corner = {right_top_x, right_top_y};
}


}