project(TCPClient)
message(STATUS "    building ${PROJECT_NAME} Library")

file(GLOB headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)


add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME}  PRIVATE Threads::Threads  )



