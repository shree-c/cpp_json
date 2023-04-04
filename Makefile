compile_test_file:
	g++ -std=c++20 test/test.cpp src/serializer.cpp src/printer.cpp  -o .temp_test

run_tests:
	bash test/check_against_std_cases.bash .temp_test ./test_cases

o3_bin:
	g++ src/json_no_rec.cpp src/printer.cpp -std=c++20 -O3 -o bin/o3

debug_bin:
	g++ src/json_no_rec.cpp src/printer.cpp -std=c++20 -g -o bin/debug
