# This script is called from AddBenchmark in Macros.cmake
# It deletes the benchmark output files and then runs the benchmark.

IF (WIN32)

	SEPARATE_ARGUMENTS(FILES_TO_DELETE_VARS WINDOWS_COMMAND ${FILES_TO_DELETE})

	EXECUTE_PROCESS (
		COMMAND del /S /Q ${FILES_TO_DELETE_VARS}
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/../benchmarks)

	EXECUTE_PROCESS (
		COMMAND ${EXECUTABLE_OUTPUT_PATH}/Release/ogs ${benchmarkStrippedName}
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/../benchmarks/${benchmarkDir})

ELSE (WIN32)

	SEPARATE_ARGUMENTS(FILES_TO_DELETE_VARS UNIX_COMMAND ${FILES_TO_DELETE})

	EXECUTE_PROCESS (
		COMMAND rm -f ${FILES_TO_DELETE_VARS}
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/../benchmarks)

	EXECUTE_PROCESS (
		COMMAND ${EXECUTABLE_OUTPUT_PATH}/ogs ${benchmarkStrippedName}
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/../benchmarks/${benchmarkDir})

ENDIF (WIN32)
