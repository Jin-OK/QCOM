cmake_minimum_required(VERSION 3.19)
project(QCOM LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets SerialPort Network LinguistTools)

qt_standard_project_setup()

qt_add_executable(QCOM
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

set_target_properties(QCOM PROPERTIES
    AUTORCC ON
)

qt_add_resources(QCOM "resources"
    PREFIX "/"
    FILES
        icons/serial.svg
        icons/tcp_client.svg
        icons/tcp_server.svg
        icons/udp.svg
        icons/connect.svg
        icons/disconnect.svg
        icons/send.svg
        icons/clear.svg
        icons/save.svg
        icons/font.svg
        icons/scan.svg
)

qt_add_translations(
    TARGETS QCOM
    TS_FILES QCOM_za_CN.ts
)

target_link_libraries(QCOM
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::SerialPort
        Qt::Network
)

install(TARGETS QCOM
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET QCOM
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
