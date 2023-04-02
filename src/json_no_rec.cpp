#include "myjson.h"
#include "print.h"
#include "tokenizer.h"
#include <array>
#include <cassert>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

inline bool includes(std::array<Token, 10> &expected_token, Token t) {
  for (int i = 0; i < 10; i++) {
    // to check against zero
    if (expected_token[i] == Token::GUARD) {
      return false;
    }
    if (expected_token[i] == t)
      return true;
  }
  return false;
}

inline bool strToBool(const std::string &str) {
  if (str == "true" || str == "1") {
    return true;
  } else if (str == "false" || str == "0") {
    return false;
  } else {
    throw std::invalid_argument("Invalid boolean string: " + str);
  }
}

std::shared_ptr<Json_entity> json(Tokenizer t) {
  Token c;
  // eats latest data
  Json_entity_shared_ptr mouth = nullptr;
  std::array<Token, 10> expected_token = {Token::OPEN_BRA, Token::OPEN_ARR,
                                          Token::BOOLEAN,  Token::NUMBER,
                                          Token::STRING,   Token::TNULL};
  std::vector<Context> context{Context::NOTHING};
  context.reserve(20);
  std::vector<Json_entity_shared_ptr> mouth_stack;
  mouth_stack.reserve(20);
  Token last_token_type = Token::UNKNOWN;
  std::string last_key;
  while ((c = t.gettoken()) != Token::END) {
    if (!includes(expected_token, c)) {
      std::cout << t.get_last_token() << "\n";
      throw "json: unexpected token";
    }
    switch (c) {
    case Token::OPEN_BRA: {
      expected_token = {Token::STRING, Token::CLOSE_BRA};
      //------------------
      std::shared_ptr<Json_obj> new_obj = std::make_shared<Json_obj>();
      if (context.back() == Context::INSIDE_CURLY) {
        Json_obj_shared_ptr temp_obj_ptr =
            std::dynamic_pointer_cast<Json_obj>(mouth);
        temp_obj_ptr->insert(last_key, new_obj);
      } else if (context.back() == Context::INSIDE_ARRAY) {
        Json_arr_shared_ptr temp_arr_ptr =
            std::dynamic_pointer_cast<Json_arr>(mouth);
        temp_arr_ptr->insert(new_obj);
      }
      mouth_stack.push_back(new_obj);
      //------------------
      // upcast
      mouth = std::dynamic_pointer_cast<Json_entity>(new_obj);
      context.push_back(Context::INSIDE_CURLY);
      break;
    }

    case Token::STRING: {
      if (context.back() == Context::INSIDE_CURLY) {
        if (last_token_type == Token::COLON) {
          Json_obj_shared_ptr temp_obj_ptr =
              std::dynamic_pointer_cast<Json_obj>(mouth);
          temp_obj_ptr->insert(
              last_key, std::make_shared<Json_string>(t.get_last_token()));
          expected_token = {Token::COMMA, Token::CLOSE_BRA};
          break;
        }
        if (last_token_type == Token::COMMA ||
            last_token_type == Token::OPEN_BRA) {
          last_key = t.get_last_token();
          expected_token = {Token::COLON};
          break;
        }
      } else if (context.back() == Context::INSIDE_ARRAY) {
        expected_token = {Token::COMMA, Token::CLOSE_ARR};
        Json_arr_shared_ptr temp_arr_ptr =
            std::dynamic_pointer_cast<Json_arr>(mouth);
        temp_arr_ptr->insert(
            std::make_shared<Json_string>(t.get_last_token()));
        break;
      }
      // Context::NOTHING

      //--------
      assert(context.back() == Context::NOTHING);
      assert(mouth == nullptr);
      mouth = std::make_shared<Json_string>(t.get_last_token());
      //--------
      expected_token = {};
      break;
    }
    case Token::COLON: {
      expected_token = {Token::OPEN_BRA, Token::OPEN_ARR, Token::BOOLEAN,
                        Token::NUMBER,   Token::STRING,   Token::TNULL};
      break;
    }
    case Token::OPEN_ARR: {
      expected_token = {Token::OPEN_BRA, Token::OPEN_ARR, Token::BOOLEAN,
                        Token::NUMBER,   Token::STRING,   Token::TNULL,
                        Token::CLOSE_ARR};
      Json_arr_shared_ptr new_arr = std::make_shared<Json_arr>();
      if (context.back() == Context::INSIDE_CURLY) {
        assert(last_token_type == Token::COLON);
        Json_obj_shared_ptr temp_obj_ptr =
            std::dynamic_pointer_cast<Json_obj>(mouth);
        temp_obj_ptr->insert(last_key, new_arr);
      } else if (context.back() == Context::INSIDE_ARRAY) {
        Json_arr_shared_ptr temp_arr_ptr =
            std::dynamic_pointer_cast<Json_arr>(mouth);
        temp_arr_ptr->insert(new_arr);
      }
      mouth_stack.push_back(new_arr);
      // upcast; updating mouth
      mouth = std::dynamic_pointer_cast<Json_entity>(new_arr);

      context.push_back(Context::INSIDE_ARRAY);
      break;
    }
    case Token::CLOSE_ARR: {
      if (context.back() != Context::INSIDE_ARRAY) {
        std::cerr << "Not inside array" << std::endl;
        std::exit(1);
      }
      context.pop_back();
      //----------------
      if (mouth_stack.size() != 1) {
        mouth_stack.pop_back();
        mouth = mouth_stack.back();
      }
      //----------------
      if (context.back() == Context::INSIDE_CURLY) {
        expected_token = {Token::COMMA, Token::CLOSE_BRA};
        break;
      }
      if (context.back() == Context::INSIDE_ARRAY) {
        expected_token = {Token::COMMA, Token::CLOSE_ARR};
        break;
      }
      expected_token = {};
      break;
    }
    case Token::CLOSE_BRA: {
      if (context.back() != Context::INSIDE_CURLY) {
        std::cerr << "Unexpected closing curly" << std::endl;
        std::exit(1);
      }
      context.pop_back();
      //--------------
      if (mouth_stack.size() != 1) {
        mouth_stack.pop_back();
        mouth = mouth_stack.back();
      }
      //--------------
      if (context.back() == Context::INSIDE_CURLY) {
        expected_token = {Token::COMMA, Token::CLOSE_BRA};
        break;
      }
      if (context.back() == Context::INSIDE_ARRAY) {
        expected_token = {Token::COMMA, Token::CLOSE_ARR};
        break;
      }
      expected_token = {};
      break;
    }
    case Token::COMMA: {
      expected_token = {Token::OPEN_BRA, Token::OPEN_ARR, Token::BOOLEAN,
                        Token::NUMBER,   Token::STRING,   Token::TNULL};
      break;
    }
    case Token::TNULL:
    case Token::BOOLEAN:
    case Token::NUMBER: {
      if (last_token_type == Token::COLON) {
        if (context.back() != Context::INSIDE_CURLY) {
          std::cerr << "This should not have happened" << std::endl;
          std::exit(1);
        }
        expected_token = {Token::COMMA, Token::CLOSE_BRA};
      } else if (context.back() == Context::INSIDE_ARRAY) {
        expected_token = {Token::COMMA, Token::CLOSE_ARR};
      } else {
        expected_token = {Token::END};
      }
      //--------
      switch (c) {
      case Token::BOOLEAN: {
        if (context.back() == Context::INSIDE_ARRAY) {
          Json_arr_shared_ptr temp_arr =
              std::dynamic_pointer_cast<Json_arr>(mouth);
          temp_arr->insert(
              std::make_shared<Json_bool>(strToBool(t.get_last_token())));
        } else if (context.back() == Context::INSIDE_CURLY) {
          Json_obj_shared_ptr temp_obj =
              std::dynamic_pointer_cast<Json_obj>(mouth);
          temp_obj->insert(
              last_key,
               std::make_shared<Json_bool>(strToBool(t.get_last_token())));
        } else {
          assert(context.back() == Context::NOTHING);
          assert(mouth == nullptr);
          mouth = std::make_shared<Json_bool>(strToBool(t.get_last_token()));
        }
        break;
      }
      case Token::NUMBER: {
        if (context.back() == Context::INSIDE_ARRAY) {
          Json_arr_shared_ptr temp_arr =
              std::dynamic_pointer_cast<Json_arr>(mouth);
          temp_arr->insert(
              std::make_shared<Json_number>(std::stod(t.get_last_token())));
        } else if (context.back() == Context::INSIDE_CURLY) {
          Json_obj_shared_ptr temp_obj =
              std::dynamic_pointer_cast<Json_obj>(mouth);
          temp_obj->insert(
              last_key,
               std::make_shared<Json_number>(std::stod(t.get_last_token())));
        } else {
          assert(context.back() == Context::NOTHING);
          assert(mouth == nullptr);
          mouth = std::make_shared<Json_number>(std::stod(t.get_last_token()));
        }
        break;
      }
      case Token::TNULL: {
        if (context.back() == Context::INSIDE_ARRAY) {
          Json_arr_shared_ptr temp_arr =
              std::dynamic_pointer_cast<Json_arr>(mouth);
          temp_arr->insert(std::make_shared<Json_null>());
        } else if (context.back() == Context::INSIDE_CURLY) {
          Json_obj_shared_ptr temp_obj =
              std::dynamic_pointer_cast<Json_obj>(mouth);
          temp_obj->insert(
              last_key, std::make_shared<Json_null>());
        } else {
          assert(context.back() == Context::NOTHING);
          assert(mouth == nullptr);
          mouth = std::make_shared<Json_null>();
        }
        break;
      }
      //--------
      default:
        break;
      }
      break;
    }
    // case sub handling end
    case Token::UNKNOWN: {
      throw "json: unknown token";
      break;
    }
    default:
      break;
    }
    last_token_type = c;
  }
  if (context.back() != Context::NOTHING) {
    switch (context.back()) {
    case Context::INSIDE_ARRAY:
      throw "unclosed array";
      break;
    case Context::INSIDE_CURLY:
      throw "unclosed object";
      break;
    default:
      throw "context is not cleared\n";
      break;
    }
  }
  if (mouth == nullptr) {
    throw "unexpected end of input";
  }
  return mouth;
}

std::string &read_string() {
  static std::string buffer;
  char c;
  while (std::cin.get(c)) {
    buffer += c;
  }
  if (std::cin.eof()) {
    return buffer;
  } else if (std::cin.fail()) {
    std::cerr << "error reading from input\n";
    std::exit(1);
  }
  return buffer;
}

int main(int argc, char **argv) {
  std::string &json_str = read_string();
  Tokenizer t(json_str);
  try {
    auto start = std::chrono::high_resolution_clock::now();
    std::shared_ptr<Json_entity> x = json(t);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        end - start); // Calculate duration
    if (argc == 2) {
      print_json(x);
    }
    std::cout << "\nElapsed time: " << duration.count() << " milliseconds"
              << std::endl; // Print duration in milliseconds
  } catch (const char *err) {
    std::cerr << err << '\n';
    return 1;
  }
  return 0;
}
