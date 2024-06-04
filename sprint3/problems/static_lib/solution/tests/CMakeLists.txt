project(tests)

add_executable(${PROJECT_NAME} loot_generator_tests.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE CONAN_PKG::catch2 model)



