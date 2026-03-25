cmake_minimum_required(VERSION 3.10)

project(tinySQL VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tinySQL "src/main.cpp" "src/engine.h" "src/engine.cpp" "src/parser.h" "src/parser.cpp" "src/tokenizer.h" "src/tokenizer.cpp" "src/printer.h" "src/printer.cpp" "tests/test_engine.cpp" "src/table.h" "src/table.cpp")

target_include_directories(tinySQL PRIVATE src)
target_compile_options(tinySQL PRIVATE /fsanitize=address /Zi)
target_link_options(tinySQL PRIVATE /fsanitize=address)