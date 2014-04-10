# (C) Copyright 2014, MongoDB, Inc.

cmake_minimum_required(VERSION 2.8.12)

project(mongotestcluster)

find_package(Qt5Widgets REQUIRED)

qt5_wrap_ui(UI_HEADERS
    main_window.ui)

add_executable(mongotestcluster WIN32
    mongotestcluster.cpp
    main_window.cpp
    main_window.h
    ${UI_HEADERS})

target_link_libraries(mongotestcluster Qt5::Widgets)
