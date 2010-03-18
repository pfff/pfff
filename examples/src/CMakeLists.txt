add_executable(pfff-timings pfff-timings utils TimeHelpers)
target_link_libraries(pfff-timings pffflib-static)

add_executable(most-similar-pair most-similar-pair utils)
target_link_libraries(most-similar-pair md5 pffflib-static)

add_executable(disk-read-experiment disk-read-experiment TimeHelpers utils)
target_link_libraries(disk-read-experiment md5 pffflib-static)

add_executable(disk-read-experiment-2 disk-read-experiment-2 TimeHelpers utils)
target_link_libraries(disk-read-experiment-2 md5 pffflib-static)

add_executable(disk-read-experiment-3 disk-read-experiment-3 TimeHelpers utils)
target_link_libraries(disk-read-experiment-3 md5 pffflib-static)
