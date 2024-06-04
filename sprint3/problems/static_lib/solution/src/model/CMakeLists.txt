project(model)
file(GLOB headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_library(${PROJECT_NAME} ${sources} ${headers})
target_link_libraries(${PROJECT_NAME} Threads::Threads CONAN_PKG::boost)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)


