#pragma once
#include "point.h"
namespace model
{
    class Road {
        struct HorizontalTag {
            explicit HorizontalTag() = default;
        };

        struct VerticalTag {
            explicit VerticalTag() = default;
        };
    public:
        constexpr static HorizontalTag HORIZONTAL{};
        constexpr static VerticalTag VERTICAL{};
        Road(HorizontalTag, Point start, Coord end_x) noexcept;
        Road(VerticalTag, Point start, Coord end_y) noexcept;
        bool IsOnTheRoad(Pointf point) const;
        bool IsHorizontal() const noexcept;
        bool IsVertical() const noexcept;
        Point GetStart() const noexcept;
        Point GetEnd() const noexcept;
        Pointf bound_to_the_road(Pointf point);
        static double bound(double bound_1, double bound_2,double value);
    private:
        void makeCorners();
        Pointf left_bottom_corner;
        Pointf right_top_corner;
        const double width = 0.4;
        Point start_;
        Point end_;
    };

}