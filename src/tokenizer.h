#include "myjson.h"
#include <iostream>

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

inline void Tokenizer::print_token_name(Token t, std::ostream &st) {
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

inline bool Tokenizer::check_rest_spell(const std::string &str) {
  for (int i = 0; i < str.length(); i++) {
    int c = getch();
    if (c != str.at(i)) {
      return false;
    }
  }
  return true;
}

inline bool Tokenizer::valid_no(const char &c) {
  return (isdigit(c) || c == '.' || c == 'E' || c == 'e' || c == '-' ||
          c == '+');
}

inline Token Tokenizer::gettoken() {
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
      return token_type = Token::TNULL;
    } else {
      std::cerr << "gettoken: error expected valid value: [n]ull" << std::endl;
      std::exit(1);
    }
  }
  // only integers are handled
  if (valid_no(c)) {
    int period_count = 0;
    if (c == '.')
      period_count++;
    last_token = "";
    last_token.push_back(c);
    char prev_digit = c;
    while (valid_no(c = getch())) {
      if (c == '.') {
        if (period_count != 0) {
          std::cerr << "gettoken: invalid number, more than one decimal points";
          std::exit(1);
        }
        period_count++;
      }
      last_token.push_back(c);
      prev_digit = c;
    }
    if (prev_digit == '.') {
      std::cerr << "gettoken: nothing after decimal point";
      std::exit(1);
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
        std::cout << last_token;
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
