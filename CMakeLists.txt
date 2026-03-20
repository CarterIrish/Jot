cmake_minimum_required(VERSION 3.14)
project(jot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
	json
	GIT_REPOSITORY https://github.com/nlohmann/json.git
	GIT_TAG v3.11.3
)

FetchContent_MakeAvailable(json)

add_executable(jot
	src/main.cpp
	src/Config.cpp
	src/NoteManager.cpp
	src/TUI.cpp
)

target_include_directories(jot PRIVATE src)
target_link_libraries(jot PRIVATE nlohmann_json::nlohmann_json)