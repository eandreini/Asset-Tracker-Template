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
# Variables are expanded at registration time, so CMAKE_BINARY_DIR is captured correctly.
cmake_language(DEFER CALL add_custom_command
  TARGET dfu_application_zip
  POST_BUILD
  COMMAND unzip -jo ${CMAKE_BINARY_DIR}/dfu_application.zip app.signed.bin -d ${CMAKE_BINARY_DIR}
  COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_BINARY_DIR}/app.signed.bin ${CMAKE_BINARY_DIR}/TRN91_app_signed.bin
)
