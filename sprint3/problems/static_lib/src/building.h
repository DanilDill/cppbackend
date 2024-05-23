#pragma once
#include "point.h"
namespace model
{
    struct Size {
        Dimension width, height;
    };

    struct Rectangle {
        Point position;
        Size size;
    };
    class Building {
    public:
        explicit Building(Rectangle bounds) noexcept
                : bounds_{bounds} {
        }

        const Rectangle& GetBounds() const noexcept {
            return bounds_;
        }

    private:
        Rectangle bounds_;
    };
}