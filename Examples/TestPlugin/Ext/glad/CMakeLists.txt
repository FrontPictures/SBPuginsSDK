project(glad)

add_library(${PROJECT_NAME} SHARED
    include/glad/gl.h
    include/KHR/khrplatform.h
    src/gl.c
    )

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_definitions(${PROJECT_NAME} PUBLIC GLAD_API_CALL_EXPORT)
target_compile_definitions(${PROJECT_NAME} PRIVATE GLAD_API_CALL_EXPORT_BUILD)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} opengl32)
else()
    target_link_libraries(${PROJECT_NAME} dl)
endif()

if(BINDIR)
    set_target_properties(${PROJECT_NAME}
        PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${BINDIR}"
        RUNTIME_OUTPUT_DIRECTORY "${BINDIR}"
        )
endif()

install(TARGETS ${PROJECT_NAME} ${SB_BUILT_LIBS_INSTALL_DEST})
