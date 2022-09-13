cmake_minimum_required(VERSION 3.17)

set(TESTS_PROJECT_NAME tscut_tests)

project(${TESTS_PROJECT_NAME}
LANGUAGES CXX C
)

set(SOURCES_DIR "../sources")
set(DEPENDENCIES_DIR "../dependencies")

# Делаем основные файлы проекта видимыми для файлов тестов (только заголовки)
include_directories(${SOURCES_DIR})

# Собираем библиотеку Catch2
add_subdirectory(${DEPENDENCIES_DIR}/catch2 Catch2Build)

message(STATUS "Compiling tests..")

# Файлы основного проекта, необходимые для проведения тестов
SET(TEST_PATH_PARSER_REQUIRED_FILES
${SOURCES_DIR}/path_parser.cpp)

# Файлы тестов
set(TEST_FILES
main.cpp
${TEST_PATH_PARSER_REQUIRED_FILES}
test_path_parser.cpp)

add_executable(${TESTS_PROJECT_NAME} ${TEST_FILES})

target_link_libraries(${TESTS_PROJECT_NAME} Catch2 Boost::regex)