#pragma once

#include <filesystem>

#include "model.h"

namespace json_loader
{
model::Game LoadGame(const std::filesystem::path& json_path,boost::asio::io_context&);
}  // namespace json_loader

