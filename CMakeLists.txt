cmake_minimum_required(VERSION 3.10)

project(tinySQL VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG         v1.14.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(tinySQL 
    "src/main.cpp" 
    "src/engine.h" "src/engine.cpp"
    "src/parser.h" "src/parser.cpp"
    "src/tokenizer.h" "src/tokenizer.cpp"
    "src/printer.h" "src/printer.cpp"
    "src/table.h" "src/table.cpp"
)

target_include_directories(tinySQL PRIVATE src)
if(MSVC)
    target_compile_options(tinySQL PRIVATE /fsanitize=address /Zi)
    target_link_options(tinySQL PRIVATE /fsanitize=address)
else()
    target_compile_options(tinySQL PRIVATE -fsanitize=address -g)
    target_link_options(tinySQL PRIVATE -fsanitize=address)
endif()

enable_testing()

add_executable(tinySQLTests
    "tests/test_tokenizer.cpp"
    "tests/test_engine.cpp"
    "tests/test_parser.cpp"
    "src/engine.cpp"
    "src/parser.cpp"
    "src/tokenizer.cpp"
    "src/printer.cpp"
    "src/table.cpp"
)
target_include_directories(tinySQLTests PRIVATE src)
target_link_libraries(tinySQLTests PRIVATE GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(tinySQLTests)