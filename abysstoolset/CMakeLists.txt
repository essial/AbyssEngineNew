cmake_minimum_required(VERSION 3.20)

project(abysstoolset CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(spdlog CONFIG REQUIRED)
find_package(argh CONFIG REQUIRED)

add_executable(${PROJECT_NAME})

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE -DSPDLOG_DEBUG_ON -DSPDLOG_TRACE_ON)
endif(CMAKE_BUILD_TYPE MATCHES Debug)

configure_file(
        ${CMAKE_SOURCE_DIR}/config.h.in
        ${PROJECT_BINARY_DIR}/config.h
)

target_sources(${PROJECT_NAME}
        PRIVATE
        src/abt.cpp
        )

set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "abt"
        FOLDER "Applications/"
        )

target_include_directories(${PROJECT_NAME} PRIVATE
        ${PROJECT_BINARY_DIR}
        )

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        libabyss
        spdlog::spdlog spdlog::spdlog_header_only
        argh
        )

if (WIN32)
    install(TARGETS ${PROJECT_NAME}
            RUNTIME_DEPENDENCIES
            PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-"
            POST_EXCLUDE_REGEXES ".*system32/.*\\.dll"
            DIRECTORIES C:/DevLibs/bin
            LIBRARY DESTINATION bin
            ARCHIVE DESTINATION bin
            )
else (WIN32)
    install(TARGETS ${PROJECT_NAME})
endif (WIN32)
