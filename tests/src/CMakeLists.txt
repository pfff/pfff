add_executable(pffftest main test_util
	 TestHexOutput TestMTwister TestPfffBlockSampleGenerator TestPfffCLibOnFiles TestPfffHasher TestPfffHasherOnFiles
	                TestPfffOptions TestPfffOutputFormatter TestPlatform TestPostHashers TestTimings)
target_link_libraries(pffftest pffflib-static)
target_link_libraries(pffftest UnitTest++)
