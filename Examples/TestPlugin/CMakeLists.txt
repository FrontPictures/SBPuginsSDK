cmake_minimum_required(VERSION 3.20)

project(TestPlugin)

set(PLUGIN_NAME TestPlugin)

add_subdirectory(Ext)

add_library(${PROJECT_NAME} SHARED
    TestPlugin.cpp TestPlugin.h
    TestPluginImage.cpp TestPluginImage.h
    TestShader.cpp TestShader.h
    )

target_link_libraries(${PROJECT_NAME} PUBLIC
    glad
    SBPluginSDK
    sdl2
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "bin/Plugins/${PLUGIN_NAME}"
    RUNTIME_OUTPUT_DIRECTORY "bin/Plugins/${PLUGIN_NAME}"
    )

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin/Plugins/${PLUGIN_NAME})
