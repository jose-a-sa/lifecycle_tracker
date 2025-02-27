cmake_minimum_required(VERSION 3.24) # or higher if applicable
project(lifecycle_tracker)

# Main options
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Add compiler options (more pedantic in errors)
if(MSVC)
    add_compile_options(/W3 /WX)
else()
    add_compile_options(
        -Wall # Enable all warnings
        -Wextra # Enable extra warnings
        -Wpedantic # Enforce standard-compliant code
        -Werror # Treat warnings as errors
        -Wno-gnu-statement-expression
    )
endif()

option(BUILD_TESTING "" ON)
option(FETCH_FMTLIB "" ON)


add_library(vendor INTERFACE)


if(FETCH_FMTLIB)
    set(fmt_FETCH_VERSION "11.0.2")
    include(FetchContent)
    FetchContent_Declare(fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt
        GIT_TAG ${fmt_FETCH_VERSION}
    )
    FetchContent_MakeAvailable(fmt)
    target_link_libraries(vendor INTERFACE fmt::fmt)
endif()


include_directories(${CMAKE_SOURCE_DIR})


# add_library(lifecycle_tracker INTERFACE qs/config.h qs/demangler.h qs/lifecycle_tracker.h)
# add_library(qs::lifecycle_tracker ALIAS lifecycle_tracker)


if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
