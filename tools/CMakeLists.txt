set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(log_server log_server.cpp)


if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(log_server PRIVATE -Wall -Wextra -pedantic)
endif()

# --- Линковка с системными библиотеками ---

if(WIN32)
    target_link_libraries(log_server PRIVATE ws2_32)
else()
    # find_package(Threads REQUIRED)
    # target_link_libraries(log_server PRIVATE Threads::Threads)
endif()