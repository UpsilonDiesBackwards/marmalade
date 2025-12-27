function(compile_icon target icon_source bundle_directory)
    set(ASSETS_DIR "${bundle_directory}/Contents/Resources")
    set(PARTIAL_PLIST "${CMAKE_CURRENT_BINARY_DIR}/app.partial.plist")
    set(CAR_FILE "${ASSETS_DIR}/Assets.car")

    add_custom_command(
            OUTPUT "${CAR_FILE}" "${PARTIAL_PLIST}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${ASSETS_DIR}"
            COMMAND xcrun actool ${icon_source}
                    --compile ${ASSETS_DIR}
                    --platform macosx
                    --minimum-deployment-target 13.3
                    --app-icon marmalade
                    --output-partial-info-plist ${PARTIAL_PLIST}
            DEPENDS ${icon_source}
            COMMENT "Compiling Liquid Glass icon: ${ICON_SOURCE}"
            VERBATIM
    )

    add_custom_target("${target}_compile_icon_assets" ALL
            DEPENDS "${CAR_FILE}"
    )

    add_dependencies(${target} "${target}_compile_icon_assets")
endfunction()
