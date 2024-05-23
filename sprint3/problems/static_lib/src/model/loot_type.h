#pragma once

#include <variant>
#include <map>
namespace model
{
    using LootType = std::map<std::string_view,std::variant<std::string,double,std::int64_t>>;
}
