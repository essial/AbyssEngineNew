cmake_minimum_required(VERSION 3.20)

project(libabyss CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

find_package(ZLIB REQUIRED)


add_library(${PROJECT_NAME}
        src/bitstream.cpp
        src/readstream.cpp
        src/mpq.cpp
        src/crypto.cpp
        )


set_target_properties(${PROJECT_NAME} PROPERTIES
        FOLDER "Libraries/"
        )


target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )

target_link_libraries(${PROJECT_NAME}
        ZLIB::ZLIB
        ${CMAKE_THREAD_LIBS_INIT}
        )

install(TARGETS libabyss
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )
