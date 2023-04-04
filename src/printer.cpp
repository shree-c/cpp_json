#include "exception.h"
#include "print.h"
#include "types.h"
#include <iostream>
#include <memory>
namespace SJSON {
int Printer::spaces_count = 0;

void Printer::print_spaces(int count) { std::cout << std::string(count, ' '); }

void Printer::print_colorful(std::string text, Color c, int spaces) {
  print_spaces(spaces);
  switch (c) {
  case Color::RED:
    std::cout << "\033[31m" << text << "\033[0m";
    break;
  case Color::GREEN:
    std::cout << "\033[32m" << text << "\033[0m";
    break;
  case Color::YELLOW:
    std::cout << "\033[93m" << text << "\033[0m";
    break;
  case Color::BLUE:
    std::cout << "\033[36m" << text << "\033[0m";
    break;
  case Color::MEGENTA:
    std::cout << "\033[95m" << text << "\033[0m";
    break;
  default:
    std::cout << text;
    break;
  }
}

void Printer::print_array(Json_entity_shared_ptr x) {
  std::shared_ptr<Json_arr> temp_str_obj =
      std::dynamic_pointer_cast<Json_arr>(x);
  std::vector<std::shared_ptr<Json_entity>> vec = temp_str_obj->get_value();
  print_colorful("[\n", Color::MEGENTA, 0);
  spaces_count += 2;
  print_spaces(Printer::spaces_count);
  int count = 0;
  for (auto element : vec) {
    print_json(element);
    count++;
    if (count != vec.size())
      print_colorful(",", Color::MEGENTA, 0);
    else
      std::cout << '\n';
  }
  spaces_count -= 2;
  print_colorful("]", Color::MEGENTA, spaces_count);
}

void Printer::print_object(std::shared_ptr<Json_entity> x) {
  std::shared_ptr<Json_obj> temp_str_obj =
      std::dynamic_pointer_cast<Json_obj>(x);
  std::unordered_map<std::string, std::shared_ptr<Json_entity>> map =
      temp_str_obj->get_value();
  print_colorful("{\n", Color::MEGENTA, 0);
  spaces_count += 2;
  int count = 0;
  for (const auto &pair : map) {
    print_colorful("\"" + pair.first + "\"", Color::GREEN, spaces_count);
    print_colorful(" : ", Color::MEGENTA, 0);
    print_json(pair.second);
    count++;
    if (count != map.size())
      print_colorful(",\n", Color::MEGENTA, 0);
    else
      std::cout << '\n';
  }
  spaces_count -= 2;
  print_colorful("}", Color::MEGENTA, spaces_count);
}

void Printer::print_json(std::shared_ptr<Json_entity> json_node) {
  if (json_node == nullptr) {
    throw SJSONException("argument is nullptr");
  }
  Data_type type_of_obj = json_node->get_type();
  switch (type_of_obj) {
  case Data_type::STRING: {
    // std::cout << "printing string\n";
    std::shared_ptr<Json_string> temp_str_obj =
        std::dynamic_pointer_cast<Json_string>(json_node);
    print_colorful("\"" + temp_str_obj->get_value() + "\"", Color::GREEN, 0);
    break;
  }
  case Data_type::NUMBER: {
    // std::cout << "printing number\n";
    std::shared_ptr<Json_number> temp_num_obj =
        std::dynamic_pointer_cast<Json_number>(json_node);
    print_colorful(std::to_string(temp_num_obj->get_value()), Color::BLUE, 0);
    break;
  }
  case Data_type::TNULL: {
    print_colorful("null", Color::BLUE, 0);
    break;
  }
  case Data_type::BOOLEAN: {
    // std::cout << "printing boolean\n";
    std::shared_ptr<Json_bool> temp_bool_obj =
        std::dynamic_pointer_cast<Json_bool>(json_node);
    print_colorful((temp_bool_obj->get_value()) ? "true" : "false", Color::BLUE,
                   0);
    break;
  }
  case Data_type::ARRAY: {
    // std::cout << "printing array\n";
    print_array(json_node);
    break;
  }
  case Data_type::OBJECT: {
    // std::cout << "printing object\n";
    print_object(json_node);
    break;
  }
  default:
    std::cout << "This should not have happened\n";
    break;
  }
}

void Printer::print_token_type(Token t) {
  switch (t) {
  case Token::OPEN_BRA:
    std::cout << "openbra\n";
    break;
  case Token::CLOSE_BRA:
    std::cout << "close bra\n";
    break;
  case Token::OPEN_ARR:
    std::cout << "open arr\n";
    break;
  case Token::CLOSE_ARR:
    std::cout << "close arr\n";
    break;
  case Token::NUMBER:
    std::cout << "number\n";
    break;
  case Token::STRING:
    std::cout << "string\n";
    break;
  case Token::COLON:
    std::cout << "colon\n";
    break;
  case Token::COMMA:
    std::cout << "comma\n";
    break;
  case Token::BOOLEAN:
    std::cout << "boolean\n";
    break;
  case Token::TNULL:
    std::cout << "tnull\n";
    break;
  case Token::UNKNOWN:
    std::cout << "unknown\n";
    break;
  default:
    throw SJSONException("print_token_type: unexpected token");
    break;
  }
}

} // namespace SJSON
