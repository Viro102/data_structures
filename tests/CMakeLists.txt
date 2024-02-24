add_library(
        tests STATIC
        _details/console_output.cpp
        _details/console.cpp
        _details/test.cpp
)

target_include_directories(tests PUBLIC ${PROJECT_SOURCE_DIR})
