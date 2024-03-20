project(TCPServer)
message(STATUS "    building ${PROJECT_NAME} Library")

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME}  PRIVATE Threads::Threads dl )

