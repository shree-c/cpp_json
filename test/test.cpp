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

int main(int argc, char **argv) {
  if (argc != 2)
  {
    std::cerr << "supply directory name as argument\n";
    return 1;
  }
  const std::filesystem::path test_directory{argv[1]};
  int total = 0;
  int failed = 0;

  for (auto const &dir_entry :
       std::filesystem::directory_iterator{test_directory}) {
    std::cout << dir_entry.path() << '\n';
    std::ifstream test_case(dir_entry.path());
    std::ostringstream buffer;
    buffer << test_case.rdbuf();
    std::string json_string = buffer.str();
    try {
      total++;
      SJSON::Serializer temp_s(json_string);
      SJSON::Json_entity_shared_ptr json_ptr = temp_s.serialize();
      std::cout << "✅" << dir_entry.path() << '\n';
    } catch (SJSONException &exception) {
      if (dir_entry.path().filename().string().starts_with('n') ||
          dir_entry.path().filename().string().starts_with('i')) {
        std::cout << "✅ " << dir_entry.path() << '\n';
        std::cout << exception.what() << '\n';
      } else {
        failed++;
        std::cout << "❌ " << dir_entry.path() << '\n';
        std::cerr << exception.what() << '\n';
      }
    }
  }
  std::cout << "total: " << total << " failed: " << failed << '\n';
}
