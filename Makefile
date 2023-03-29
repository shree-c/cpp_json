run_tests:
	bash test/check_against_std_cases.bash

o3_bin:
	g++ src/json_no_rec.cpp src/printer.cpp -O3 -o bin/o3

debug_bin:
	g++ src/json_no_rec.cpp src/printer.cpp -g -o bin/debug
