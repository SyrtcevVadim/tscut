cmake_minimum_required(VERSION 3.20)

project(tscut
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Переменные структуры проекта
set(SOURCES_DIR "sources")
set(DEPENDENCIES_DIR "dependencies")

# Переменная для включения компиляции тестов
# Если надо отключить тесты, выставить значение False
set(TESTS_ENABLED FALSE CACHE BOOL "Allows a user to build a tests")

set(SOURCES 
${SOURCES_DIR}/main.cpp
${SOURCES_DIR}/connection_manager.cpp
${SOURCES_DIR}/connection.cpp
${SOURCES_DIR}/mime_types.cpp
${SOURCES_DIR}/reply.cpp
${SOURCES_DIR}/request_handler.cpp
${SOURCES_DIR}/request_parser.cpp
${SOURCES_DIR}/server.cpp

${SOURCES_DIR}/path_parser.cpp
${SOURCES_DIR}/video_processing.cpp
${SOURCES_DIR}/ffmpeg_guards.cpp
)


if (TESTS_ENABLED)
    message(STATUS "Tests are enabled..")
    add_subdirectory(tests)
endif()


# Собираем библиотеку Boost
add_subdirectory(${DEPENDENCIES_DIR}/boost EXCLUDE_FROM_ALL)
# Библиотека форматирования fmt 
add_subdirectory(${DEPENDENCIES_DIR}/fmt)

# Находим FFmpeg библиотеки
set(FFMPEG_PREFIX $ENV{HOME}/libraries/ffmpeg CACHE PATH "Set's a prefix to the FFmpeg library directory")
# Ищем библиотеки libavutil libavformat libavcodec
# NO_DEFAULT отключает поиск библиотек в путях по умолчанию.
# Например, у меня брались библиотеки из директории /usr/local/lib и брали статические библиотеки
# Данные же конструкции берут библиотеки по пути ${FFMPEG_PREFIX}/lib

# Мы указываем avutil вместо libavutil, т.к. на стадии линковки
# Название библиотеки будет использоваться с параметром -l, который и поразумевает
# часть lib
find_library(FFMPEG_AVUTIL avutil PATHS ${FFMPEG_PREFIX}/lib NO_DEFAULT_PATH)
find_library(FFMPEG_AVFORMAT avformat PATHS ${FFMPEG_PREFIX}/lib NO_DEFAULT_PATH)
find_library(FFMPEG_AVCODED avcodec PATHS ${FFMPEG_PREFIX}/lib NO_DEFAULT_PATH)
# Проверяем пути. Должно быть что-то подобное: $HOME/libraries/ffmpeg/lib/.so
message(STATUS ${FFMPEG_AVUTIL})
message(STATUS ${FFMPEG_AVFORMAT})
message(STATUS ${FFMPEG_AVCODED})
set(FFMPEG_INCLUDE_DIR ${FFMPEG_PREFIX}/include)
message(STATUS ${FFMPEG_PREFIX})
message(STATUS ${FFMPEG_INCLUDE_DIR})
include_directories(${FFMPEG_INCLUDE_DIR})

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC
Boost::asio 
Boost::logic 
Boost::regex 
${FFMPEG_AVUTIL}
${FFMPEG_AVFORMAT}
${FFMPEG_AVCODED}
fmt::fmt)