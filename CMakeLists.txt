# (C) Copyright 2014, MongoDB, Inc.

cmake_minimum_required(VERSION 2.8.12)

project(mongotestcluster)

find_package(Qt5Widgets REQUIRED)

qt5_wrap_ui(UI_HEADERS
    add_host_dialog.ui
    console_output_dialog.ui
    main_window.ui)

qt5_add_resources(RESOURCES mongotestcluster.qrc)
if(WIN32)
set(RESOURCES ${RESOURCES} mongotestcluster.rc)
endif(WIN32)

add_executable(mongotestcluster WIN32
    mongotestcluster.cpp
    add_host_dialog.cpp
    add_host_dialog.h
    console_output_dialog.cpp
    console_output_dialog.h
    host_types.cpp
    host_types.h
    main_window.cpp
    main_window.h
    test_cluster_model.cpp
    test_cluster_model.h
    ${UI_HEADERS}
    ${RESOURCES})

target_link_libraries(mongotestcluster Qt5::Widgets)

set_target_properties(mongotestcluster PROPERTIES AUTOMOC ON)
