cmake_minimum_required(VERSION 3.27)

find_package(cmake-bare REQUIRED PATHS node_modules/cmake-bare)
# find_package(cmake-fetch REQUIRED PATHS node_modules/cmake-fetch)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_MACOSX_BUNDLE OFF)

project(bare_llama C CXX)

# option(LLAMA_BUILD_EXAMPLES "Build llama.cpp examples" OFF)
# option(DEBUG "Enable debugging" OFF)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
set(LLAMA_STATIC ON CACHE BOOL "Build static libraries" FORCE)

if(APPLE)
    set(CMAKE_C_COMPILER_FORCED ON)
    set(CMAKE_CXX_COMPILER_FORCED ON)
endif()

include(FetchContent)

FetchContent_Declare(
    llama
    GIT_REPOSITORY "https://github.com/ggerganov/llama.cpp.git"
    GIT_TAG "master"
    GIT_REMOTE_UPDATE_STRATEGY REBASE_CHECKOUT
)
FetchContent_MakeAvailable(llama)
FetchContent_GetProperties(
   llama
   SOURCE_DIR llama_SOURCE_DIR
   BINARY_DIR llama_BINARY_DIR
)

# couldn't authenticate with fetch_package!
# tried to use a personal access token but it didn't work
# fetch_package("github:/ggerganov/llama.cpp #master")

add_bare_module(bare_llama)

target_sources(
  ${bare_llama}
  PRIVATE
    binding.c
)

target_compile_options(
    ${bare_llama}
    PRIVATE
    -fno-rtti
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Debug>:-O0 -g>
)

# Link against static libraries
target_link_libraries(
  ${bare_llama}
  PRIVATE
    llama
    ggml
)

target_include_directories(
  ${bare_llama}
  PRIVATE
  ${CMAKE_SOURCE_DIR}/src
  ${llama_SOURCE_DIR}
  ${llama_SOURCE_DIR}/common
  ${llama_SOURCE_DIR}/ggml/include
)
