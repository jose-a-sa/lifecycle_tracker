# Specify the minimum CMAKE version required
cmake_minimum_required(VERSION 3.24) # or higher if applicable
project(lifecycle_tracker_tests)


# Enable CTest testing
enable_testing()

set(googletest_FETCH_VERSION "v1.12.0")

include(FetchContent)
FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG ${googletest_FETCH_VERSION}
)
if(WIN32)
    set(gtest_force_shared_crt ON CACHE BOOL "For Windows: Prevent overriding the parent project's compiler/linker settings" FORCE)
endif()
FetchContent_MakeAvailable(googletest)

include(GoogleTest)


# Macro that take binary name and folder and creates a test with all .cpp inside
function(add_test_binary NAME LOCATION_SRC)
    add_executable(test_${NAME} ${LOCATION_SRC})
    target_compile_definitions(test_${NAME} PRIVATE QS_TESTING)
    target_link_libraries(test_${NAME} PRIVATE vendor GTest::gmock_main)
    gtest_discover_tests(test_${NAME})
    install(TARGETS test_${NAME} DESTINATION ${CMAKE_SOURCE_DIR}/bin)
endfunction()


add_test_binary(lifecycle_tracker test_lifecycle_tracker.cpp)