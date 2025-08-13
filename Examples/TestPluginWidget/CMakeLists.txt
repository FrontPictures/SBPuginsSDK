cmake_minimum_required(VERSION 3.20)

project(TestPluginWidget)

set(PLUGIN_NAME TestPlugin)

add_subdirectory(Ext)

find_package(Qt6 COMPONENTS Widgets OpenGLWidgets REQUIRED)
set(CMAKE_AUTOMOC ON)

add_library(${PROJECT_NAME} SHARED
    TestPluginWidget.cpp TestPluginWidget.h
    )

target_link_libraries(${PROJECT_NAME} PUBLIC
    SBPluginSDK
    Qt6::Widgets
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "bin/Plugins/${PLUGIN_NAME}"
    RUNTIME_OUTPUT_DIRECTORY "bin/Plugins/${PLUGIN_NAME}"
    )

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin/Plugins/${PLUGIN_NAME})
