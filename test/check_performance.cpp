#include "../src/print.h"
#include "../src/serializer.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <chrono>

int main(int argc, char **argv) {
  if (argc != 2)
  {
    std::cerr << "supply supply name as argument\n";
    return 1;
  }
  std::filesystem::path file_path{argv[argc - 1]};
  if (!std::filesystem::exists(file_path)) {
    std::cerr << "file doesn't exist\n";
    return 1;
  }
  if (!std::filesystem::is_regular_file(file_path)) {
    std::cerr << "not a normal file\n";
    return 1;
  }
  try{
    std::ifstream test_file_stream{file_path};
    std::ostringstream buffer;
    buffer << test_file_stream.rdbuf();  
    std::string json_string = buffer.str();
    auto start = std::chrono::high_resolution_clock::now();
    SJSON::Serializer ss{json_string};
    SJSON::Json_entity_shared_ptr s = ss.serialize();
    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms_double = stop - start;
    std::cout << "Took: " << ms_double.count() << " ms.\n";
  } catch (SJSONException &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
}
