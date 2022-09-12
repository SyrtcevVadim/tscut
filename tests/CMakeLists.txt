cmake_minimum_required(VERSION 3.17)

set(TESTS_PROJECT_NAME tscut_tests)

project(${TESTS_PROJECT_NAME}
LANGUAGES CXX C
)

set(SOURCES_DIR "sources")
set(DEPENDENCIES_DIR "dependencies")

# Делаем основные файлы проекта видимыми для файлов тестов (только заголовки)
include_directories(../${SOURCES_DIR})

# Подключаем библиотеку Catch2
add_subdirectory(../${DEPENDENCIES_DIR}/catch2 Catch2Build)
# Подключаем библиотеку Boost
# add_subdirectory(../${DEPENDENCIES_DIR}/boost EXCLUDE_FROM_ALL)

message(STATUS "Compiling tests..")

set(TEST_FILES
test.cpp)

add_executable(${TESTS_PROJECT_NAME} ${TEST_FILES})

target_link_libraries(${TESTS_PROJECT_NAME} Catch2)