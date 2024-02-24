add_library(complexities STATIC complexity_analyzer.cpp)

target_include_directories(complexities PUBLIC ${PROJECT_SOURCE_DIR})
