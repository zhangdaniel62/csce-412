cmake_minimum_required(VERSION 3.16)
project(load_balancer_sim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Your executable name
add_executable(lb
    src/main.cpp
    src/LoadBalancer.cpp
    src/WebServer.cpp
    src/Request.cpp
    src/IPBlocker.cpp
    src/Config.cpp
)

target_include_directories(lb PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ================= yaml-cpp dependency =================
# Prefer an installed yaml-cpp, but automatically fetch it if missing.
include(FetchContent)

# Try config-package first (what Homebrew/vcpkg typically provide)
find_package(yaml-cpp QUIET)

if (NOT yaml-cpp_FOUND)
  message(STATUS "yaml-cpp not found; fetching via FetchContent...")

  # Use a known stable release tag. You can bump this later if desired.
  FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG        yaml-cpp-0.9.0
  )

  # Avoid installing yaml-cpp when building this project
  set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
  set(YAML_CPP_INSTALL     OFF CACHE BOOL "" FORCE)

  FetchContent_MakeAvailable(yaml-cpp)

  # The fetched project typically provides the target `yaml-cpp::yaml-cpp`.
  # Some builds provide `yaml-cpp` only; handle both.
  if (TARGET yaml-cpp::yaml-cpp)
    target_link_libraries(lb PRIVATE yaml-cpp::yaml-cpp)
  else()
    target_link_libraries(lb PRIVATE yaml-cpp)
  endif()
else()
  target_link_libraries(lb PRIVATE yaml-cpp::yaml-cpp)
endif()

# ================= spdlog dependency =================
# Prefer installed spdlog, fetch if missing.

find_package(spdlog QUIET)

if (NOT spdlog_FOUND)
  message(STATUS "spdlog not found; fetching via FetchContent...")

  FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.17.0
  )

  # Disable extras
  set(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
  set(SPDLOG_BUILD_TESTS   OFF CACHE BOOL "" FORCE)
  set(SPDLOG_BUILD_BENCH   OFF CACHE BOOL "" FORCE)
  set(SPDLOG_FMT_EXTERNAL  OFF CACHE BOOL "" FORCE)
  set(SPDLOG_INSTALL       OFF CACHE BOOL "" FORCE)

  FetchContent_MakeAvailable(spdlog)
endif()

# Link spdlog
if (TARGET spdlog::spdlog)
  target_link_libraries(lb PRIVATE spdlog::spdlog)
elseif (TARGET spdlog::spdlog_header_only)
  target_link_libraries(lb PRIVATE spdlog::spdlog_header_only)
elseif (TARGET spdlog)
  target_link_libraries(lb PRIVATE spdlog)
endif()