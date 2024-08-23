cmake_minimum_required(VERSION 3.22)

project(use_usocket LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include(FetchContent)

FetchContent_Declare(
  uSockets_content
  GIT_REPOSITORY https://github.com/uNetworking/uSockets
  GIT_TAG v0.8.5
  GIT_SHALLOW ON
  GIT_SUBMODULES ""
)
FetchContent_MakeAvailable(uSockets_content)
file(GLOB_RECURSE SOURCES ${usockets_content_SOURCE_DIR}/src/*.c)
add_library(uSockets ${SOURCES})
target_include_directories(uSockets PUBLIC ${usockets_content_SOURCE_DIR}/src)
target_compile_definitions(uSockets PRIVATE LIBUS_NO_SSL)

FetchContent_Declare(
  uWebSockets_content
  GIT_REPOSITORY https://github.com/uNetworking/uWebSockets
  GIT_TAG v20.37.0
  GIT_SHALLOW ON
  GIT_SUBMODULES ""
)
FetchContent_MakeAvailable(uWebSockets_content)
find_package(ZLIB REQUIRED)
add_library(uWebSockets INTERFACE)
target_include_directories(uWebSockets INTERFACE ${uwebsockets_content_SOURCE_DIR}/src/)
target_link_libraries(uWebSockets INTERFACE uSockets ${ZLIB_LIBRARIES})
target_compile_options(uWebSockets INTERFACE -Wno-deprecated-declarations)

# Client exec
# add_executable(client src/client.cpp)
# target_link_libraries(client uWebSockets)

# Server exec
find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/protobuf/generated
)
add_executable(server 
    src/main.cpp
    src/Game.cpp
    src/MonteCarlo.cpp
    src/ThreadPool.cpp
    protobuf/generated/game_state.pb.cc
)
target_link_libraries(server uWebSockets ${PROTOBUF_LIBRARIES})
