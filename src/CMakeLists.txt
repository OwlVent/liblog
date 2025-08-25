set(LIBLOG_SOURCES logger.cpp)
set(LIBLOG_TARGETS "") # Начинаем с пустого списка

# Создаем статическую библиотеку, только если опция включена
if(BUILD_STATIC_LIB)
    add_library(liblog-static STATIC ${LIBLOG_SOURCES})
    list(APPEND LIBLOG_TARGETS liblog-static) # Добавляем в список
endif()

# Создаем динамическую библиотеку, только если опция включена
if(BUILD_SHARED_LIB)
    add_library(liblog-shared SHARED ${LIBLOG_SOURCES})
    list(APPEND LIBLOG_TARGETS liblog-shared) # Добавляем в список
    if(WIN32)
        set_target_properties(liblog-shared PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
    endif()
endif()

if(LIBLOG_TARGETS)
    # Проходимся по каждой цели, которая была добавлена в список LIBLOG_TARGETS
    foreach(TARGET ${LIBLOG_TARGETS})
        # Применяем свойства к каждой цели ИНДИВИДУАЛЬНО
        target_include_directories(
                ${TARGET} # <-- Передаем только одну цель за раз
                PUBLIC
                # $<INSTALL_INTERFACE:include> - Путь для установленной библиотеки
                # $<BUILD_INTERFACE:...> - Путь для использования внутри проекта
                $<INSTALL_INTERFACE:include>
                $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        )
    endforeach()
endif()
