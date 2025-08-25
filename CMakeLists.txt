cmake_minimum_required(VERSION 4.0)
project(liblog VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

# --- Управление типом сборки (статическая/динамическая) ---
option(BUILD_SHARED_LIB "Build the shared library" ON)
option(BUILD_STATIC_LIB "Build the static library" ON)

add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(tools)

add_library(liblog src/logger.cpp)
target_include_directories(liblog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# --- УСТАНОВКА БИБЛИОТЕКИ ---
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Устанавливаем цели (файлы .a, .lib, .so, .dll)
# и создаем экспортный набор "liblogTargets"
install(TARGETS liblog-static liblog-shared
        EXPORT liblogTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Устанавливаем публичные заголовочные файлы
install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# --- ГЕНЕРАЦИЯ CMake-файлов для find_package() ---
# Это позволит другим проектам находить вашу библиотеку
# командой find_package(liblog)

install(EXPORT liblogTargets
        FILE liblogTargets.cmake
        NAMESPACE liblog::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/liblog
)

write_basic_package_version_file(
        liblogConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
        "cmake/liblogConfig.cmake.in"
        "liblogConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/liblog
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/liblogConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/liblogConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/liblog
)