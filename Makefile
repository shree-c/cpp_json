run_tests:
	bash test/check_against_std_cases.bash

make_o3_bin:
	g++ src/json_no_rec.cpp src/printer.cpp -O3 -o bin/json_cpp

make_debug_bin:
	g++ src/json_no_rec.cpp src/printer.cpp -g -o bin/json_cpp
