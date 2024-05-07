#pragma once
#include <string_view>
#define BOOST_JSON_STANDALONE

struct JsonAttribute
 {
   JsonAttribute()=delete;
   struct BuildArrayAttributes
    {
       BuildArrayAttributes()=delete;
       constexpr static const std::string_view NAME = "buildings";
       constexpr static const std::string_view ATTR_X = "x";
       constexpr static const std::string_view ATTR_Y = "y";
       constexpr static const std::string_view ATTR_WIDTH = "w";
       constexpr static const std::string_view ATTR_HEIGHT = "h";
    };
   struct RoadArrayAttributes
    {
       RoadArrayAttributes()=delete;
       constexpr static const std::string_view NAME = "roads";
       constexpr static const std::string_view ATTR_X0 = "x0";
       constexpr static const std::string_view ATTR_Y0 = "y0";
       constexpr static const std::string_view ATTR_X1 = "x1";
       constexpr static const std::string_view ATTR_Y1 = "y1";
    };
    struct OfficeArrayAttributes
    {
       OfficeArrayAttributes()=delete;
       constexpr static const std::string_view NAME = "offices";
       constexpr static const std::string_view ATTR_ID = "id";
       constexpr static const std::string_view ATTR_X = "x";
       constexpr static const std::string_view ATTR_Y = "y";
       constexpr static const std::string_view ATTR_OFFSET_X = "offsetX";
       constexpr static const std::string_view ATTR_OFFSET_Y= "offsetY";
    };
    struct MapArrayAttributes
    {
        MapArrayAttributes()=delete;
        constexpr static const std::string_view NAME = "maps";
        struct MapAttributes
        {
            MapAttributes()=delete;
            constexpr static const std::string_view ATTR_ID = "id";
            constexpr static const std::string_view ATTR_NAME = "name";
            constexpr static const std::string_view DOG_SPEED = "dogSpeed";
            constexpr static const std::string_view ATTR_ROADS = JsonAttribute::RoadArrayAttributes::NAME;
            constexpr static const std::string_view ATTR_BUILDINGS = JsonAttribute::BuildArrayAttributes::NAME;
            constexpr static const std::string_view ATTR_OFFICES = JsonAttribute::OfficeArrayAttributes::NAME;
        };
    };
    constexpr static const std::string_view DOG_SPEED_DEFAULT = "defaultDogSpeed";

    struct ResponseAtrrubutes
    {
        ResponseAtrrubutes()=delete;
        constexpr static const std::string_view CODE = "code";
        constexpr static const std::string_view MESSAGE = "message";
    };


};

