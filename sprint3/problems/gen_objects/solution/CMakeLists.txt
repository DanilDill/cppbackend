cmake_minimum_required(VERSION 3.11)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)
get_property(importTargets DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY IMPORTED_TARGETS)
message(STATUS "${importTargets}")


set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
add_executable(game_server
	src/json_atribute.h
	src/main.cpp
	src/http_server.cpp
	src/http_server.h
	src/sdk.h
	src/model.h
	src/model.cpp
	src/tagged.h
	src/boost_json.cpp
	src/json_loader.h
	src/json_loader.cpp
	src/request_handler.cpp
	src/request_handler.h
	src/json_response.h
	src/json_response.cpp
	src/staticfile_loader.cpp
	src/staticfile_loader.h
	src/content_type.h
	src/logging_request_handler.h
	src/get_handler.cpp
	src/get_handler.h
	src/post_handler.cpp
	src/post_handler.h
    src/tokenizer.h
	src/head_handler.cpp
	src/head_handler.h
	src/default_handler.h
	src/default_handler.cpp
	src/ticker.h
	src/loot_generator.h
	src/loot_generator.cpp
		src/direction.h
		src/direction.cpp
		src/player.h
		src/player.cpp
		src/game_map.h
		src/game_map.cpp
		src/building.h
		src/road.h
		src/road.cpp
		src/game_session.h
		src/game_session.cpp
		src/loot_type.h)

target_include_directories(game_server PRIVATE CONAN_PKG::boost)
target_link_libraries(game_server PRIVATE Threads::Threads CONAN_PKG::boost)
