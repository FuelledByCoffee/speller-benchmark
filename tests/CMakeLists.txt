enable_testing()


add_test(NAME run_benchmark
	COMMAND bench
	WORKING_DIRECTORY ${SPELLER_LOCATION})
set_tests_properties(run_benchmark
	PROPERTIES
	REQUIRED_FILES ${SPELLER_LOCATION}/speller)

add_test(NAME wrong_directory
	COMMAND bench)
set_tests_properties(wrong_directory
	PROPERTIES
	WILL_FAIL TRUE)
