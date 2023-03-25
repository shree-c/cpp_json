#include "myjson.h"
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <vector>
#include <string>

class Tokenizer {
public:
  Token gettoken();
  Token token_type;
  static void print_token_name(Token, std::ostream &);
  int count{0};
  std::string get_last_token() { return last_token; }

private:
  std::string buffer;
  char getch() {
    if (std::cin.eof()) {
      return -1;
    }
    if (buffer.length() == 0) {
      char c;
      std::cin.get(c);
      return c;
    }
    char c{buffer.back()};
    buffer.pop_back();
    count++;
    return c;
  }
  void ungetch(wchar_t c) {
    count--;
    buffer.push_back(c);
  }
  bool check_rest_spell(const std::string &);
  static bool valid_no(const char &);
  std::string last_token;
};

void Tokenizer::print_token_name(Token t, std::ostream &st) {
  switch (t) {
  case Token::BOOLEAN: {
    st << "boolean";
    break;
  }
  case Token::NUMBER: {
    st << "number";
    break;
  }
  case Token::STRING: {
    st << "string";
    break;
  }
  case Token::COMMA: {
    st << "comma";
    break;
  }
  case Token::TNULL: {
    st << "null";
    break;
  }
  case Token::OPEN_ARR: {
    st << "open array";
    break;
  }
  case Token::CLOSE_ARR: {
    st << "close array";
    break;
  }
  case Token::COLON: {
    st << "colon";
    break;
  }
  case Token::OPEN_BRA: {
    st << "open bracket";
    break;
  }
  case Token::CLOSE_BRA: {
    st << "close bracket";
    break;
  }
  case Token::UNKNOWN: {
    st << "unknown";
    break;
  }
  case Token::END: {
    st << "end";
    break;
  }
  default: {
    st << "default unknown";
    break;
  }
  }
}

bool Tokenizer::check_rest_spell(const std::string &str) {
  for (int i = 0; i < str.length(); i++) {
    int c = getch();
    if (c != str.at(i)) {
      return false;
    }
  }
  return true;
}

bool Tokenizer::valid_no(const char &c) {
  return (isdigit(c) || c == '.' || c == 'E' || c == 'e' || c == '-' ||
          c == '+');
}

Token Tokenizer::gettoken() {
  int c;
  while ((c = getch()) == ' ' || c == '\t' || c == '\n')
    ;
  if (c == -1) {
    return token_type = Token::END;
  }
  if (c == ',') {
    return token_type = Token::COMMA;
  }
  // handle true or false
  if (c == 't') {
    if (check_rest_spell("rue")) {
      last_token = "true";
      return token_type = Token::BOOLEAN;
    } else {
      std::cerr << "gettoken: error expected valid value: [t]rue" << std::endl;
      std::exit(1);
    }
  }
  if (c == 'f') {
    if (check_rest_spell("alse")) {
      last_token = "false";
      return token_type = Token::BOOLEAN;
    } else {
      std::cerr << "gettoken: error expected valid value: [f]alse" << std::endl;
      std::exit(1);
    }
  }
  if (c == 'n') {
    if (check_rest_spell("ull")) {
      last_token = "null";
      return token_type = Token::BOOLEAN;
    } else {
      std::cerr << "gettoken: error expected valid value: [n]ull" << std::endl;
      std::exit(1);
    }
  }
  // only integers are handled
  if (valid_no(c)) {
    last_token = "";
    last_token.push_back(c);
    while (valid_no(c = getch())) {
      last_token.push_back(c);
    }
    ungetch(c);
    return token_type = Token::NUMBER;
  }
  // string
  if (c == '"') {
    last_token = "";
    int prev = c;
    while ((c = getch()) != '"' || prev == '\\') {
      if (c == -1) {
        std::cerr << "Unterminated string" << std::endl;
        std::exit(1);
      }
      if (c == '\n') {
        std::cerr << "new lines are not allowed inside strings" << std::endl;
        std::exit(1);
      }
      last_token.push_back(c);
      prev = c;
    }
    return token_type = Token::STRING;
  }
  // array
  if (c == '[') {
    return token_type = Token::OPEN_ARR;
  }
  if (c == ']') {
    return token_type = Token::CLOSE_ARR;
  }
  if (c == ':') {
    return token_type = Token::COLON;
  }
  if (c == '}') {
    return token_type = Token::CLOSE_BRA;
  }
  if (c == '{') {
    return token_type = Token::OPEN_BRA;
  }
  // fprintf(stderr, "gettoken: unknown token_type\n");
  return token_type = Token::UNKNOWN;
}
bool includes(std::vector<Token> &expected_token, Token t) {
  for (Token x : expected_token) {
    if (t == x)
      return true;
  }
  return false;
}

bool strToBool(const std::string& str) {
    if (str == "true" || str == "1") {
        return true;
    }
    else if (str == "false" || str == "0") {
        return false;
    }
    else {
        throw std::invalid_argument("Invalid boolean string: " + str);
    }
}

std::unique_ptr<Json_entity> json(Tokenizer t) {
  Token c;
  // eats latest data
  std::unique_ptr<Json_entity> start = nullptr;
  // initially both are same
  std::unique_ptr<Json_entity> &mouth = start;
  std::vector<Token> expected_token{Token::OPEN_BRA, Token::OPEN_ARR,
                                    Token::BOOLEAN,  Token::NUMBER,
                                    Token::STRING,   Token::TNULL};
  std::vector<Context> context{Context::NOTHING};
  Token last_token_type = Token::UNKNOWN;
  while ((c = t.gettoken()) != Token::END && includes(expected_token, c)) {
    // should not break or continue in following switch
    switch (c) {
    case Token::OPEN_BRA: {
      expected_token.clear();
      expected_token = {Token::STRING, Token::CLOSE_BRA};
      context.push_back(Context::INSIDE_CURLY);
      break;
    }
    case Token::STRING: {
      expected_token.clear();
      if (context.back() == Context::INSIDE_CURLY) {
        if (last_token_type == Token::COLON) {
          expected_token = {Token::COMMA, Token::CLOSE_BRA};
          break;
        }
        if (last_token_type == Token::COMMA ||
            last_token_type == Token::OPEN_BRA) {
          expected_token = {Token::COLON};
          break;
        }
      } else if (context.back() == Context::INSIDE_ARRAY) {
        expected_token = {Token::COMMA, Token::CLOSE_ARR};
        break;
      }
      // Context::NOTHING

      //--------
      assert(context.back() == Context::NOTHING);
      assert(mouth == nullptr);
      mouth = std::make_unique<Json_string>(t.get_last_token());
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
      if (context.back() == Context::INSIDE_CURLY) {
        expected_token = {Token::COMMA, Token::CLOSE_BRA};
        break;
      }
      if (context.back() == Context::INSIDE_ARRAY) {
        expected_token = {Token::COLON, Token::CLOSE_ARR};
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
        break;
      }
      if (context.back() == Context::INSIDE_ARRAY) {
        expected_token = {Token::COMMA, Token::CLOSE_ARR};
        break;
      }
      //--------
      assert(context.back() == Context::NOTHING);
      assert(mouth == nullptr);
      switch (c) {
      case Token::BOOLEAN: {
        mouth = std::make_unique<Json_bool>(strToBool(t.get_last_token()));
        break;
      }
      case Token::NUMBER: {
        mouth = std::make_unique<Json_number>(std::stod(t.get_last_token()));
        break;
      }
      case Token::TNULL: {
        mouth = std::make_unique<Json_null>();
        break;
      }
      }
      //--------
      expected_token = {};
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
  return start;
}

enum class Color {
  RED,
  BLUE,
  GREEN,
  YELLOW
};

void print_colorful(std::string text, Color c) {
  switch (c) {
    case Color::RED:
      std::cout << "\033[31m" << text << "\033[0m";
      break;
    case Color::GREEN:
      std::cout << "\033[32m" << text << "\033[0m";
      break;
    case Color::YELLOW:
      std::cout << "\033[33;44m" << text << "\033[0m";
      break;
    default:
      std::cout << text;
      break;
  }
}

void print_json(std::unique_ptr<Json_entity> &x) {
  Data_type y = x->get_type();

  if (y == Data_type::BOOLEAN) {
    auto p = dynamic_cast<Json_bool *>(x.get());
    bool x = p->get_value();
    if (x) {
      print_colorful("true", Color::GREEN);
    } else {
      print_colorful("false", Color::RED);
    }
  }
  if (y == Data_type::STRING) {
    auto p = dynamic_cast<Json_string *>(x.get());
    //bool x = p->get_value();
    print_colorful(p->get_value(), Color::YELLOW);
  }
  if (y == Data_type::NUMBER) {
    auto p = dynamic_cast<Json_number *>(x.get());
    //bool x = p->get_value();
    print_colorful(std::to_string(p->get_value()), Color::GREEN);
  }
}

int main() {
  Tokenizer t;
  std::unique_ptr<Json_entity> x = json(t);
  print_json(x);
  std::cout << "Syntax is precise" << std::endl;
  return 0;
}
