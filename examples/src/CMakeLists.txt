add_executable(pfff-timings pfff-timings utils TimeHelpers)
target_link_libraries(pfff-timings pffflib-static)

add_executable(most-similar-pair most-similar-pair utils)
