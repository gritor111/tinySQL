# 1. Set the minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# 2. Name your project and specify the language
project(tinySQL VERSION 1.0 LANGUAGES CXX)

# 3. Specify the C++ standard (e.g., C++17 or C++20)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 4. Define the executable and the source files it needs
add_executable(tinySQL "src/main.cpp" "src/engine.h" "src/engine.cpp" "src/parser.h" "src/parser.cpp" "src/tokenizer.h" "src/tokenizer.cpp" "src/printer.h" "src/printer.cpp" "tests/test_engine.cpp" "src/table.h" "src/table.cpp")

target_include_directories(tinySQL PRIVATE src)