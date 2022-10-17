# This file was adapted from: https://stackoverflow.com/questions/60854495/qt5-cmake-include-all-libraries-into-executable

find_package(Qt5Core REQUIRED)

# Find absolute path to qmake, then use it to find the windeployqt executable
get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

# Use the windeployqt utility to find the dependent Qt libraries for the given target,
# and copy them into the directory specified by target_dir
function(windeployqt target target_dir)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND "${_qt_bin_dir}/windeployqt.exe"         
                --verbose 1
                --debug
                --no-svg
                --no-angle
                --no-opengl
                --no-opengl-sw
                --no-compiler-runtime
                --no-translations
                --no-system-d3d-compiler
                --dir ${target_dir}
                \"$<TARGET_FILE:${target}>\"
        COMMENT "Deploying Qt libraries using windeployqt for compilation target '${target}' ..."
    )
endfunction()