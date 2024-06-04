#pragma once

#include <filesystem>

#include "model/model.h"

namespace  model
{
model::Game LoadGame(const std::filesystem::path& json_path,boost::asio::io_context&);
}  // namespace json_loader

