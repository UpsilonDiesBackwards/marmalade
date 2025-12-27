function(set_native_properties target name version copyright identifier)
    if (WIN32)
        configure_file(
                ${CMAKE_SOURCE_DIR}/misc/resources/${target}.in.rc
                ${CMAKE_BINARY_DIR}/${target}.rc)

        configure_file(
                ${CMAKE_SOURCE_DIR}/misc/resources/${target}.exe.manifest
                ${CMAKE_BINARY_DIR}/${target}.exe.manifest
        )

        set_target_properties(${target} PROPERTIES
                VS_MANIFEST_FILE "${CMAKE_BINARY_DIR}/{$target}}.exe.manifest"
        )

        if (MSVC)
            if (CMAKE_BUILD_TYPE STREQUAL "Debug")
                set_target_properties(${target} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
                target_compile_definitions(${target} PRIVATE WINCONSOLE)
            elseif ()
                set_target_properties(${target} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
            endif ()
        endif ()
    elseif (APPLE)
        set_target_properties(${target} PROPERTIES
                MACOSX_BUNDLE_BUNDLE_NAME "${name}"
                MACOSX_BUNDLE_BUNDLE_VERSION "${version}"
                MACOSX_BUNDLE TRUE
                MACOSX_BUNDLE_ICON_FILE marmalade
                MACOSX_BUNDLE_ICON_NAME marmalade
                MACOSX_BUNDLE_LONG_VERSION_STRING "${version}"
                MACOSX_BUNDLE_SHORT_VERSION_STRING "${version}"
                MACOSX_BUNDLE_COPYRIGHT "${copyright}"
                MACOSX_BUNDLE_GUI_IDENTIFIER "${identifier}"
                XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@loader_path/Libraries"
                XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME TRUE
                XCODE_ATTRIBUTE_EXECUTABLE_NAME "${name}")
    endif ()
endfunction()
