cmake_minimum_required(VERSION 3.17)

project(tscut
    VERSION 1.0
)

set(CMAKE_CXX_STANDARD 17)

# Переменные структуры проекта
set(SOURCES_DIR "sources")
set(DEPENDENCIES_DIR "dependencies")

set(SOURCES 
# ${SOURCES_DIR}/test.cpp
${SOURCES_DIR}/main.cpp
${SOURCES_DIR}/connection_manager.cpp
${SOURCES_DIR}/connection.cpp
${SOURCES_DIR}/mime_types.cpp
${SOURCES_DIR}/reply.cpp
${SOURCES_DIR}/request_handler.cpp
${SOURCES_DIR}/request_parser.cpp
${SOURCES_DIR}/server.cpp
)

# Подключаем библиотеки
add_subdirectory(${DEPENDENCIES_DIR}/boost EXCLUDE_FROM_ALL)


add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

target_link_libraries(${CMAKE_PROJECT_NAME}
Boost::asio Boost::logic)