target_sources(app PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/gmtrack.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gmtrack_shell.c
)
target_include_directories(app PRIVATE .)