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

get_target_property(QT5WIDGETS_PATH Qt5::Widgets IMPORTED_LOCATION_RELEASE)
get_filename_component(QT_BIN_PATH ${QT5WIDGETS_PATH} DIRECTORY)
set(QT_DLLS
	icudt52.dll
	icuin52.dll
	icuuc52.dll
	Qt5Core.dll
	Qt5Gui.dll
	Qt5Widgets.dll
	libEGL.dll
	libGLESv2.dll)
foreach(DLL ${QT_DLLS})
	list(APPEND QT_DLL_PATHS ${QT_BIN_PATH}/${DLL})
endforeach()
get_target_property(QWINDOWS_PATH Qt5::QWindowsIntegrationPlugin IMPORTED_LOCATION_RELEASE)

install(TARGETS mongotestcluster RUNTIME DESTINATION .)
install(FILES ${QT_DLL_PATHS} ${QWINDOWS_PATH} DESTINATION .)

set(CPACK_GENERATOR WIX)
set(CPACK_PACKAGE_EXECUTABLES "mongotestcluster" "MongoDB Test Cluster Utility")
set(CPACK_PACKAGE_FILE_NAME "mongodb-test-cluster-1.0.0")
set(CPACK_PACKAGE_NAME "MongoDB Test Cluster Utility")
set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_WIX_UPGRADE_GUID "515F37C5-FC53-4C61-A6D5-68DC78FEE34F")
include(CPack)
