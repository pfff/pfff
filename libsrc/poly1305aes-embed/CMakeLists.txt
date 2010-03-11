include_directories(../poly1305aes-20050218)

# Tell CMake how to handle .S (assembly) files
foreach(file aes_any.S aes_any_constants.S poly1305_any.S poly1305_any_constants.S poly1305aes_any_isequal.S)
	set_property(SOURCE ${file} PROPERTY LANGUAGE C)
endforeach()

add_library(poly1305aes STATIC aes_any_c aes_any_constants_c empty poly1305_any_c poly1305_any_constants_c
                               poly1305aes_any_authenticate poly1305aes_any_clamp poly1305aes_any_isequal_c poly1305aes_any_verify
                               aes_any.S aes_any_constants.S poly1305_any.S poly1305_any_constants.S poly1305aes_any_isequal.S)
