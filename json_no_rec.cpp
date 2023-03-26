#include "myjson.h"
#include "print.h"
#include "tokenizer.h"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

bool includes(std::vector<Token> &expected_token, Token t) {
  for (Token x : expected_token) {
    if (t == x)
      return true;
  }
  return false;
}

bool strToBool(const std::string &str) {
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
  std::shared_ptr<Json_entity> start = nullptr;
  // initially both are same
  std::shared_ptr<Json_entity> mouth = start;
  std::vector<Token> expected_token{Token::OPEN_BRA, Token::OPEN_ARR,
                                    Token::BOOLEAN,  Token::NUMBER,
                                    Token::STRING,   Token::TNULL};
  std::vector<Context> context{Context::NOTHING};
  std::vector<std::shared_ptr<Json_entity>> mouth_stack;
  // mouth_stack.push_back(start);
  Token last_token_type = Token::UNKNOWN;
  std::string last_key;
  while ((c = t.gettoken()) != Token::END && includes(expected_token, c)) {
    // should not break or continue in following switch
    switch (c) {
    case Token::OPEN_BRA: {
      expected_token.clear();
      expected_token = {Token::STRING, Token::CLOSE_BRA};
      context.push_back(Context::INSIDE_CURLY);
      //------------------
      mouth_stack.push_back(std::make_shared<Json_obj>());
      //      std::shared_ptr<Json_obj> temp_obj_ptr =
      //      dynamic_cast<std::shared_ptr<Json_obj>>(
      mouth = mouth_stack.back();
      //------------------
      break;
    }

    case Token::STRING: {
      expected_token.clear();
      if (context.back() == Context::INSIDE_CURLY) {
        if (last_token_type == Token::COLON) {
          // push the key value into the container
          // getting in touch with the container of the object
          // casting mouth to object
          std::shared_ptr<Json_obj> temp_obj_ptr =
              std::dynamic_pointer_cast<Json_obj>(mouth);
          temp_obj_ptr->get_value().insert(
              {last_key, std::make_shared<Json_string>(t.get_last_token())});
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
        std::shared_ptr<Json_arr> temp_arr_ptr =
            std::dynamic_pointer_cast<Json_arr>(mouth);
        temp_arr_ptr->get_value().push_back(
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
      expected_token.clear();
      expected_token = {Token::OPEN_BRA, Token::OPEN_ARR, Token::BOOLEAN,
                        Token::NUMBER,   Token::STRING,   Token::TNULL};
      break;
    }
    case Token::OPEN_ARR: {
      expected_token.clear();
      expected_token = {Token::OPEN_BRA, Token::OPEN_ARR, Token::BOOLEAN,
                        Token::NUMBER,   Token::STRING,   Token::TNULL,
                        Token::CLOSE_ARR};
      std::shared_ptr<Json_arr> new_arr = std::make_shared<Json_arr>();
      if (context.back() == Context::INSIDE_CURLY) {
        assert(last_token_type == Token::COLON);
        std::shared_ptr<Json_obj> temp_obj_ptr =
            std::dynamic_pointer_cast<Json_obj>(mouth);
        temp_obj_ptr->get_value().insert({last_key, new_arr});
      } else if (context.back() == Context::INSIDE_ARRAY) {
        std::shared_ptr<Json_arr> temp_arr_ptr =
            std::dynamic_pointer_cast<Json_arr>(mouth);
        temp_arr_ptr->get_value().push_back(new_arr);
      }
      mouth_stack.push_back(new_arr);
      // upcast; updating mouth
      mouth = std::dynamic_pointer_cast<Json_entity>(new_arr);

      context.push_back(Context::INSIDE_ARRAY);
      break;
    }
    case Token::CLOSE_ARR: {
      expected_token.clear();
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
        std::cout << "came here" << mouth_stack.size();
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
      expected_token.clear();
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
      }
      if (context.back() == Context::INSIDE_ARRAY) {
        expected_token = {Token::COMMA, Token::CLOSE_ARR};
      }
      //--------
      switch (c) {
      case Token::BOOLEAN: {
        if (context.back() == Context::INSIDE_ARRAY) {
          std::shared_ptr<Json_arr> temp_arr =
              std::dynamic_pointer_cast<Json_arr>(mouth);
          std::cout << "pushed bool\n";
          temp_arr->get_value().push_back(
              std::make_shared<Json_bool>(strToBool(t.get_last_token())));
        } else if (context.back() == Context::INSIDE_CURLY) {
          std::shared_ptr<Json_obj> temp_arr =
              std::dynamic_pointer_cast<Json_obj>(mouth);
          temp_arr->get_value().insert(
              {last_key,
               std::make_shared<Json_bool>(strToBool(t.get_last_token()))});
        } else {
          assert(context.back() == Context::NOTHING);
          assert(mouth == nullptr);
          mouth = std::make_shared<Json_bool>(strToBool(t.get_last_token()));
        }
        break;
      }
      case Token::NUMBER: {
        if (context.back() == Context::INSIDE_ARRAY) {
          std::shared_ptr<Json_arr> temp_arr =
              std::dynamic_pointer_cast<Json_arr>(mouth);
          temp_arr->get_value().push_back(
              std::make_shared<Json_number>(std::stod(t.get_last_token())));
        } else if (context.back() == Context::INSIDE_CURLY) {
          std::shared_ptr<Json_obj> temp_arr =
              std::dynamic_pointer_cast<Json_obj>(mouth);
          temp_arr->get_value().insert(
              {last_key,
               std::make_shared<Json_number>(std::stod(t.get_last_token()))});
        } else {
          assert(context.back() == Context::NOTHING);
          assert(mouth == nullptr);
          mouth = std::make_shared<Json_bool>(strToBool(t.get_last_token()));
        }
        break;
      }
      case Token::TNULL: {
        if (context.back() == Context::INSIDE_ARRAY) {
          std::shared_ptr<Json_arr> temp_arr =
              std::dynamic_pointer_cast<Json_arr>(mouth);
          temp_arr->get_value().push_back(std::make_shared<Json_null>());
        } else if (context.back() == Context::INSIDE_CURLY) {
          std::shared_ptr<Json_obj> temp_arr =
              std::dynamic_pointer_cast<Json_obj>(mouth);
          temp_arr->get_value().insert(
              {last_key, std::make_shared<Json_null>()});
        } else {
          assert(context.back() == Context::NOTHING);
          assert(mouth == nullptr);
          mouth = std::make_shared<Json_null>();
        }
        break;
      }
      default:
        break;
      }
      //--------
      break;
    }
    default: {
      std::cerr << "Unhandled token" << std::endl;
      Tokenizer::print_token_name(c, std::cerr);
      std::exit(1);
      break;
    }
    }
    last_token_type = c;
  }
  bool exit_failure = false;
  if (c != Token::END) {
    if (c != Token::UNKNOWN) {
      exit_failure = true;
    }
    std::cerr << "All characters are not consumed : count " << t.count
              << std::endl;
  }
  if (expected_token.size() > 0) {
    std::cerr << "Expected token list is not empty: count " << t.count
              << std::endl;
    for (Token x : expected_token) {
      Tokenizer::print_token_name(x, std::cerr);
      std::cerr << std::endl;
    }
    exit_failure = true;
  }
  if (exit_failure) {
    std::exit(2);
  }
  return mouth;
}

int main() {
  Tokenizer t;
  std::shared_ptr<Json_entity> x = json(t);
  std::cout << "Syntax is precise" << std::endl;
  print_json(x);
  return 0;
}
