cmake_minimum_required(VERSION 3.16)
project(QCOM LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

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
elseif(APPLE)
    set_target_properties(QCOM PROPERTIES MACOSX_BUNDLE TRUE)
endif()

target_link_libraries(QCOM
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::SerialPort
        Qt6::Network
)