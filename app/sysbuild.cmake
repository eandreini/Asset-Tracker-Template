# A.B.: This file is automatically included by nRF Connect SDK's sysbuild phase, which runs
# as a separate top-level CMake invocation that manages all images (app, mcuboot, etc.)
# and produces packaging artifacts like dfu_application.zip.
#
# Here CMAKE_BINARY_DIR is the west build directory (e.g. build-gmtngfd/), the same
# location where dfu_application.zip is generated — no "/app" postfix and path adjustment needed to remove it.
#
# HACK: this file is included early in _sysbuild/CMakeLists.txt processing
# (via sysbuild_add_subdirectory → include(sysbuild.cmake)), while zip.cmake — which
# creates the 'dfu_application_zip' target — is included much later. Using
# cmake_language(DEFER CALL ...) defers the add_custom_command registration to the end
# of the current directory's (_sysbuild/) processing, by which time the target exists.
#
# NOTE: cmake_language(DEFER CALL ...) evaluates arguments LAZILY — at call time, not
# registration time. Two things conspire to make CMAKE_CURRENT_LIST_DIR unusable here:
#   1. sysbuild.cmake is include()-d from INSIDE the sysbuild_add_subdirectory() function
#      (see external/zephyr/share/sysbuild/cmake/modules/sysbuild_extensions.cmake).
#      CMake functions have their own variable scope, so any set() here — including a
#      plain helper variable — is destroyed when that function returns.
#   2. The deferred call fires at the end of the _sysbuild/ DIRECTORY scope, which
#      outlives the function, so the local variable is already gone → empty expansion.
# Fix: write to the CMake cache (CACHE INTERNAL), which is global and flat — no scope
# boundaries, available for the entire configure run.
set(_TRN91_SYSBUILD_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE INTERNAL "")
cmake_language(DEFER CALL add_custom_command
  TARGET dfu_application_zip
  POST_BUILD
  COMMAND unzip -jo ${CMAKE_BINARY_DIR}/dfu_application.zip app.signed.bin -d ${CMAKE_BINARY_DIR}
  COMMAND ${CMAKE_COMMAND}
    -DINPUT_FILE=${CMAKE_BINARY_DIR}/app.signed.bin
    -DOUTPUT_DIR=${CMAKE_BINARY_DIR}
    -P ${_TRN91_SYSBUILD_CMAKE_DIR}/cmake/rename_firmware.cmake
)
