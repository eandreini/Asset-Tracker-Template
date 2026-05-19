#
# Copyright (c) 2024 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project("Asset Tracker Template")

##### ACTIVATE THIS TO ENSURE <BUILD_DIR>/app/zephyr/include/generated/zephyr/app_version.h #####
##### IS ALSO UPDATED AT EVERY INCREMENTAL BUILD                                            #####
## app_version.h is regenerated whenever its listed dependencies change. By default
## Zephyr only tracks external/zephyr/.git/index, which misses commits to this repo
## (possibly a git submodule whose real index lives in the workspace .git/modules/project/).
## Zephyr provides APP_VERSION_DEPENDS on the app_version_h target exactly for this:
## any file listed there is added to the CUSTOM_COMMAND DEPENDS list via a generator
## expression, so ninja will re-run gen_version_h.cmake (and thus git-describe) on
## the next incremental build after a new commit.
# if(TARGET app_version_h)
#   find_package(Git QUIET)
#   if(GIT_FOUND)
#     execute_process(
#       COMMAND ${GIT_EXECUTABLE} rev-parse --absolute-git-dir
#       WORKING_DIRECTORY ${APPLICATION_SOURCE_DIR}
#       OUTPUT_VARIABLE _app_git_dir
#       OUTPUT_STRIP_TRAILING_WHITESPACE
#     )
#     if(EXISTS "${_app_git_dir}/index")
#       set_property(TARGET app_version_h APPEND PROPERTY
#         APP_VERSION_DEPENDS "${_app_git_dir}/index")
#     endif()
#   endif()
# endif()

# Include files that are common for all modules
zephyr_include_directories(src/common)
zephyr_include_directories(memfault/config)

# Add main application source
target_sources(app PRIVATE src/main.c src/common/gpsparams.c)

# Module source folders
add_subdirectory(src/modules/network)
add_subdirectory(src/modules/button)
add_subdirectory(src/cbor)

add_subdirectory(src/modules/gmtrack)
add_subdirectory(src/modules/test)

# Optional modules
add_subdirectory_ifdef(CONFIG_APP_POWER src/modules/power)
add_subdirectory_ifdef(CONFIG_APP_ENVIRONMENTAL src/modules/environmental)
add_subdirectory_ifdef(CONFIG_APP_LED src/modules/led)
add_subdirectory_ifdef(CONFIG_APP_LOCATION src/modules/location)
add_subdirectory_ifdef(CONFIG_APP_CLOUD src/modules/cloud)
add_subdirectory_ifdef(CONFIG_APP_FOTA src/modules/fota)
add_subdirectory_ifdef(CONFIG_APP_STORAGE src/modules/storage)
add_subdirectory_ifdef(CONFIG_APP_SGP4 src/modules/sgp4)
add_subdirectory_ifdef(CONFIG_APP_INSPECT_SHELL src/inspect)
