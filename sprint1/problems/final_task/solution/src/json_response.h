#pragma once
#include <string>
#include "model.h"

namespace json_responce
{
    std::string to_json(const std::vector<model::Map>& maps);
    std::string to_json(const model::Map& map);
    std::string NotFoundJson();
    std::string BadRequestJson();
}