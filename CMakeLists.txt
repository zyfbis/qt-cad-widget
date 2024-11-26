cmake_minimum_required(VERSION 3.27)
project(QtCadWidget)

# from https://www.boost.org/users/download/
set(BOOST_DIR "${CMAKE_SOURCE_DIR}/boost_1_86_0")
set(Boost_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/boost_1_86_0")
if (MSVC)
    set(QT_MSVC_PATH "C:/Qt/6.8.0/msvc2022_64")
    list(APPEND CMAKE_PREFIX_PATH ${QT_MSVC_PATH})
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC_OPTIONS --connections string)

add_subdirectory(LibreCAD)
add_subdirectory(example)
