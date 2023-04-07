#include "serializer.h"
#include "exception.h"
#include "print.h"
#include "tokenizer.h"
#include "types.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
namespace SJSON {

inline double try_convert_to_double(std::string string_to_be_converted) {
  try {
    return std::stod(string_to_be_converted);
  } catch (std::out_of_range &exception) {
    throw SJSONException("stod: conversion: number too big");
  } catch (std::invalid_argument &exception) {
    throw SJSONException("stod: conversion: trying to convert invalid string");
  }
}

void Serializer::push(std::string key, Json_entity_shared_ptr value) {
  Json_obj_shared_ptr mouth_object = std::static_pointer_cast<Json_obj>(mouth);
  mouth_object->insert(key, value);
}

void Serializer::push(Json_entity_shared_ptr value) {
  Json_arr_shared_ptr mouth_object = std::static_pointer_cast<Json_arr>(mouth);
  mouth_object->insert(value);
}

void Serializer::update_mouth(Json_entity_shared_ptr container) {
  mouth = container;
  mouth_stack.push_back(container);
}

inline bool Serializer::includes(std::array<Token, 10> &expected_token,
                                 Token t) {
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

inline bool Serializer::strToBool(const std::string &str) noexcept(false) {
  if (str == "true" || str == "1") {
    return true;
  } else if (str == "false" || str == "0") {
    return false;
  } else {
    throw SJSONException("Invalid boolean string: " + str);
  }
}

void Serializer::handle_premitive(Token c) {
  switch (c) {
  case Token::BOOLEAN: {
    if (context.back() == Context::INSIDE_ARRAY) {
      push(std::make_shared<Json_bool>(strToBool(t.get_last_token())));
    } else if (context.back() == Context::INSIDE_CURLY) {
      push(last_key,
           std::make_shared<Json_bool>(strToBool(t.get_last_token())));
    } else {
      mouth = std::make_shared<Json_bool>(strToBool(t.get_last_token()));
    }
    break;
  }
  case Token::NUMBER: {
    if (context.back() == Context::INSIDE_ARRAY) {
      push(std::make_shared<Json_number>(
          try_convert_to_double(t.get_last_token())));
    } else if (context.back() == Context::INSIDE_CURLY) {
      push(last_key, std::make_shared<Json_number>(
                         try_convert_to_double(t.get_last_token())));
    } else {
      mouth = std::make_shared<Json_number>(
          try_convert_to_double(t.get_last_token()));
    }
    break;
  }
  case Token::TNULL: {
    if (context.back() == Context::INSIDE_ARRAY) {
      push(std::make_shared<Json_null>());
    } else if (context.back() == Context::INSIDE_CURLY) {
      push(last_key, std::make_shared<Json_null>());
    } else {
      mouth = std::make_shared<Json_null>();
    }
    break;
  }
  default:
    break;
  }
}

std::shared_ptr<Json_entity> Serializer::serialize() {
  Token c;
  context.reserve(20);
  mouth_stack.reserve(20);
  while ((c = t.gettoken()) != Token::END) {
    if (!includes(expected_token, c)) {
      Printer::print_token_type(c);
      throw SJSONException("json: unexpected token");
    }
    switch (c) {
    case Token::OPEN_BRA: {
      expected_token = {Token::STRING, Token::CLOSE_BRA};
      //------------------
      std::shared_ptr<Json_obj> new_obj = std::make_shared<Json_obj>();
      if (context.back() == Context::INSIDE_CURLY) {
        push(last_key, new_obj);
      } else if (context.back() == Context::INSIDE_ARRAY) {
        push(new_obj);
      }
      update_mouth(new_obj);
      context.push_back(Context::INSIDE_CURLY);
      break;
    }
    case Token::STRING: {
      if (context.back() == Context::INSIDE_CURLY) {
        if (last_token_type == Token::COLON) {
          push(last_key, std::make_shared<Json_string>(t.get_last_token()));
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
        push(std::make_shared<Json_string>(t.get_last_token()));
        break;
      }
      // Context::NOTHING
      expected_token = {};
      mouth = std::make_shared<Json_string>(t.get_last_token());
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
        push(last_key, new_arr);
      } else if (context.back() == Context::INSIDE_ARRAY) {
        push(new_arr);
      }
      update_mouth(new_arr);
      context.push_back(Context::INSIDE_ARRAY);
      break;
    }
    case Token::CLOSE_ARR: {
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
          throw SJSONException("Mismatched context");
        }
        expected_token = {Token::COMMA, Token::CLOSE_BRA};
      } else if (context.back() == Context::INSIDE_ARRAY) {
        expected_token = {Token::COMMA, Token::CLOSE_ARR};
      } else {
        expected_token = {Token::END};
      }
      //--------
      handle_premitive(c);
      break;
    }
    // case sub handling end
    case Token::UNKNOWN: {
      throw SJSONException("json: unknown token");
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
      throw SJSONException("unclosed array");
      break;
    case Context::INSIDE_CURLY:
      throw SJSONException("unclosed object");
      break;
    default:
      throw SJSONException("context is not cleared");
      break;
    }
  }
  if (mouth == nullptr) {
    throw SJSONException("unexpected end of input");
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

} // namespace SJSON
