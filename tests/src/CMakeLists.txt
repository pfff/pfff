add_executable(pffftest main test_util
				TestPfffOptions TestPfffOutputFormatter TestHexOutput TestMTwister TestPfffBlockSampleGenerator TestTimings TestPlatform
				TestPostHashers TestPfffCLibOnFiles TestPfffHasher TestPfffHasherOnFiles)

target_link_libraries(pffftest pffflib-static)
target_link_libraries(pffftest UnitTest++)
