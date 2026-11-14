cmake_minimum_required(VERSION 3.16)
project(QCOM LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum macOS version" FORCE)
endif()

if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

find_package(Qt6 6.4 REQUIRED COMPONENTS Core Widgets SerialPort Network)

add_executable(QCOM
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    resources.qrc
)

if(WIN32)
    set_target_properties(QCOM PROPERTIES WIN32_EXECUTABLE TRUE)
    # Windows ICO图标 (仅MSVC，因为需要动态生成ICO)
    if(MSVC)
        target_sources(QCOM PRIVATE resources.rc)
    endif()
elseif(APPLE)
    set_target_properties(QCOM PROPERTIES MACOSX_BUNDLE TRUE)
    # macOS ICNS图标
    set(MACOSX_ICON ${CMAKE_CURRENT_SOURCE_DIR}/icons/app_icon.icns)
    if(EXISTS ${MACOSX_ICON})
        set_source_files_properties(${MACOSX_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
        target_sources(QCOM PRIVATE ${MACOSX_ICON})
        set_target_properties(QCOM PROPERTIES MACOSX_BUNDLE_ICON_FILE "app_icon.icns")
    endif()
endif()

target_link_libraries(QCOM
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::SerialPort
        Qt6::Network
)

if(WIN32)
    target_link_libraries(QCOM PRIVATE dwmapi)
endif()