function(compile_icon target nativeui_target icon_source bundle_directory)
    find_package(Python3 REQUIRED COMPONENTS Interpreter)

    set(GEN_ROOT "${CMAKE_CURRENT_BINARY_DIR}/generated_icons")
    set(GEN_HEADER "${GEN_ROOT}/iconmapping.h")
    set(GEN_ASSETS "${GEN_ROOT}/Codicons.xcassets")

    file(MAKE_DIRECTORY "${GEN_ROOT}")
    if (NOT EXISTS "${GEN_HEADER}")
        file(WRITE "${GEN_HEADER}" "// Placeholder\n")
    endif()

    add_custom_command(
            OUTPUT "${GEN_HEADER}" "${GEN_ASSETS}/Contents.json"
            COMMAND ${Python3_EXECUTABLE} "${CMAKE_SOURCE_DIR}/tools/bake-macos-icons.py"
                    --output-header "${GEN_HEADER}"
                    --output-assets "${GEN_ASSETS}"
                    --sources "${CMAKE_SOURCE_DIR}/src"
                    --font-file "${CMAKE_SOURCE_DIR}/res/fonts/codicon.ttf"
                    --font-header "${CMAKE_SOURCE_DIR}/vendor/iconfontcppheaders/IconsCodicons.h"
            DEPENDS "${CMAKE_SOURCE_DIR}/tools/bake-macos-icons.py"
            COMMENT "Building icon assets bundle"
            VERBATIM
    )

    target_include_directories(${nativeui_target} PRIVATE "${GEN_ROOT}")
    set_source_files_properties("${GEN_HEADER}" PROPERTIES GENERATED TRUE)

    add_custom_target(${target}_icon_gen ALL DEPENDS "${GEN_HEADER}")
    add_dependencies(${target} ${target}_icon_gen)

    set(ASSETS_DIR "${bundle_directory}/Contents/Resources")
    set(PARTIAL_PLIST "${CMAKE_CURRENT_BINARY_DIR}/app.partial.plist")
    set(CAR_FILE "${ASSETS_DIR}/Assets.car")

    add_custom_command(
            OUTPUT "${CAR_FILE}" "${PARTIAL_PLIST}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${ASSETS_DIR}"
            COMMAND xcrun actool ${icon_source} ${GEN_ASSETS}
                    --compile ${ASSETS_DIR}
                    --platform macosx
                    --minimum-deployment-target 13.3
                    --app-icon marmalade
                    --output-partial-info-plist ${PARTIAL_PLIST}
            DEPENDS ${icon_source} "${GEN_ASSETS}/Contents.json"
            COMMENT "Compiling Liquid Glass icon: ${ICON_SOURCE}"
            VERBATIM
    )

    add_custom_target("${target}_compile_icon_assets" ALL
            DEPENDS "${CAR_FILE}"
    )

    add_dependencies(${target} "${target}_compile_icon_assets")
endfunction()
