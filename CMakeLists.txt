#ive been feeling down lately
cmake_minimum_required(VERSION 3.20)

project(MatrixTask)

#set(CMAKE_CXX_CLANG_TIDY "clang-tidy")

# файлы перечисляются либо через пустую строку, либо через пробел
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MatrixTask.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MatrixPrint.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Matrix.cpp
)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/MatrixTask.h
    ${CMAKE_CURRENT_SOURCE_DIR}/MatrixPrint.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Matrix.h
)

# новая цель с названием аналогичным названию проекта
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS})
set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY CXX_STANDARD 20)
#target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -Wpedantic)

if(ENABLE_DEBUG)
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -g)
endif()

add_custom_target(
    clang-format
    COMMAND clang-format
    --dry-run
    -Werror
    ${SOURCES}
    ${HEADERS}
)