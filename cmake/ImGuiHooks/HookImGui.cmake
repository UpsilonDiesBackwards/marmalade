set(PATCHED_IMGUI_DIR "${CMAKE_BINARY_DIR}/patched_imgui")
file(REMOVE_RECURSE "${PATCHED_IMGUI_DIR}")
file(MAKE_DIRECTORY "${PATCHED_IMGUI_DIR}")

set(IMGUI_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui")

find_package(Python3 REQUIRED)

execute_process(
        COMMAND ${Python3_EXECUTABLE} "${CMAKE_SOURCE_DIR}/cmake/ImGuiHooks/hook_imgui.py" "${IMGUI_SRC_DIR}" "${PATCHED_IMGUI_DIR}"
        COMMENT "Adding hooks to ImGui sources"
        WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
)

file(GLOB PATCHED_FILES "${PATCHED_IMGUI_DIR}/*.cpp" "${PATCHED_IMGUI_DIR}/*.h")

foreach(PATCHED_FILE IN LISTS PATCHED_FILES)
    get_filename_component(FILENAME "${PATCHED_FILE}" NAME)

    list(FILTER IMGUI_SRC EXCLUDE REGEX "vendor/imgui/${FILENAME}")
    message(STATUS "Adding ${FILENAME} to ImGui sources")
    list(APPEND IMGUI_SRC "${PATCHED_FILE}")
endforeach()
