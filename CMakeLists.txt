cmake_minimum_required(VERSION 3.8)

project(webli VERSION 0.3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(dep/json)

add_library(webli SHARED
	src/con.cpp
	src/dotenv.cpp
	src/http.cpp
	src/router.cpp
	src/server.cpp
	src/storage.cpp
	src/webclient.cpp
)

target_include_directories(webli PRIVATE ./include ./dep/json/include)

target_link_libraries(webli PUBLIC ssl crypto)

