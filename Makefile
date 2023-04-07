printer:
		g++ -std=c++20 -c src/printer.cpp -o printer.o -O3

serializer:
	g++ -std=c++20 -c src/serializer.cpp -o serializer.o -O3

compile_test_file: serializer printer
	g++ -std=c++20 test/test.cpp serializer.o printer.o -O3 -o .temp_test

run_tests: compile_test_file
	bash test/check_against_std_cases.bash .temp_test ./test_cases

create_test_binary: serializer
	g++ -std=c++20 test/check_performance.cpp serializer.o printer.o -o .per_bin -O3

check_performance_against_large_file: create_test_binary
	./.per_bin large_test_files/lf.json

clean:
	rm -f .per_bin
	rm -f .temp_test
	rm -f serializer.o printer.o
