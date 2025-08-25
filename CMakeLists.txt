cmake_minimum_required(VERSION 4.0)
project(liblog VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

add_library(liblog src/logger.cpp)
target_include_directories(liblog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# --- Управление типом сборки (статическая/динамическая) ---

# В CLion или через командную строку можно будет указать:
# -DBUILD_SHARED_LIBS=ON  для динамической (.dll/.so)
# -DBUILD_SHARED_LIBS=OFF для статической (.lib/.a)
# По умолчанию будет статическая.
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(LIBLOG_BUILD_EXAMPLES "Build examples" ON)

if(LIBLOG_BUILD_EXAMPLES)
    add_executable(basic_usage examples/basic_usage.cpp)
    target_link_libraries(basic_usage PRIVATE liblog)
endif()