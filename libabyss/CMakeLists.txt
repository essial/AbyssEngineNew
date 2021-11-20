cmake_minimum_required(VERSION 3.20)

project(libabyss CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

if(MSVC)
    add_definitions(-D_HAS_DEPRECATED_RESULT_OF=1)
endif(MSVC)

find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)
find_package(stormlib CONFIG)
if(NOT stormlib_FOUND)
    find_library(STORMLIB storm REQUIRED)
    add_library(stormlib::stormlib UNKNOWN IMPORTED)
    set_target_properties(stormlib::stormlib PROPERTIES
        IMPORTED_LOCATION ${STORMLIB})
endif()

add_library(${PROJECT_NAME}
        src/mpq.cpp
        src/mpqstream.cpp
        )

set_target_properties(${PROJECT_NAME} PROPERTIES
        FOLDER "Libraries/"
        )


target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        ZLIB::ZLIB
        BZip2::BZip2
        stormlib::stormlib
        ${CMAKE_THREAD_LIBS_INIT}
        )

install(TARGETS libabyss
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )
