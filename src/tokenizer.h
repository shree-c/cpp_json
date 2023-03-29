#include "myjson.h"
#include <iostream>

class Tokenizer {
public:
  Tokenizer(std::string &json_str) : buffer{json_str} {};
  Token gettoken();
  Token token_type;
  static void print_token_name(Token, std::ostream &);
  int count{0};
  std::string &get_last_token() { return last_token; }

private:
  std::string &buffer;
  inline const char getch() {
    if (buffer.length() == count) {
      count++;
      return EOF;
    }
    return buffer[count++];
  }
  inline void ungetch() { count--; }
  bool check_rest_spell(const std::string &);
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

inline Token Tokenizer::gettoken() {
  int c = '\0';
  while ((c = getch()) == ' ' || c == '\t' || c == '\n' || c == '\r')
    ;
  // string
  if (c == '"') {
    last_token = "";
    bool backslash_status = false;
    bool unterminated = true;
    bool hex_status = false;
    int hex_count = 0;
    while (unterminated) {
      c = getch();
      if (c == -1) {
        throw "unterminated string, reached EOF";
      }
      if (c >= 0x00 && c <= 0x0F) {
        throw "control character inside string";
      }
      if (backslash_status) {
        if (hex_status) {
          if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') ||
              (c >= 'a' && c <= 'f')) {
            hex_count++;
            if (hex_count == 4) {
              hex_count = 0;
              hex_status = false;
              backslash_status = false;
            }
            last_token.push_back(c);
            continue;
          }
          throw "illegal hex character";
        } else {
          if (c == '"' || c == '\\' || c == 'b' || c == 'f' || c == 'n' ||
              c == 'r' || c == 't' || c == '/') {
            backslash_status = false;
            last_token.push_back(c);
            continue;
          }
          if (c == 'u') {
            hex_status = true;
            last_token.push_back(c);
            continue;
          }
        }
        throw "unknown character after backslash";
      } else {
        if (c == '"') {
          unterminated = false;
          continue;
        }
        if (c == '\\') {
          backslash_status = true;
          last_token.push_back(c);
          continue;
        }
        last_token.push_back(c);
        continue;
      }
      throw "unknown character, missed all checks\n";
    }
    return token_type = Token::STRING;
  }
  if (c == EOF) {
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
  // number
  if (std::isdigit(c) || c == '-') {
    last_token = "";
    bool start = true;
    bool loop = true;
    bool no_more_digits = false;
    bool at_least_one_digit = false;
    bool after_exponent = false;
    bool no_dot = false;
    ungetch();
    while (loop) {
      c = getch();
      if (no_dot) {
        if (c == '.') {
          throw "number: more than one decimal point";
        }
      }
      if (no_more_digits) {
        if (std::isdigit(c)) {
          throw "number: unexpected digit";
        }
        no_more_digits = false;
      }
      if (at_least_one_digit) {
        if (std::isdigit(c)) {
          last_token.push_back(c);
          at_least_one_digit = false;
          continue;
        }
        throw "number: digit expected";
      }
      if (after_exponent) {
        if (c == '+' || c == '-') {
          at_least_one_digit = true;
          after_exponent = false;
          last_token.push_back(c);
          continue;
        }
        if (std::isdigit(c)) {
          last_token.push_back(c);
          after_exponent = false;
          continue;
        }
        throw "number: unexpected token after exponent";
      }
      if (start) {
        if (c == '-' || c == '+') {
          last_token.push_back(c);
          continue;
        }
        if (std::isdigit(c)) {
          if (c == '0') {
            no_more_digits = true;
          }
          last_token.push_back(c);
          start = false;
          continue;
        }
        throw "number: unexpected token at start";
      }
      if (c == '.') {
        no_more_digits = false;
        at_least_one_digit = true;
        no_dot = true;
        last_token.push_back(c);
        continue;
      }
      if (c == 'E' || c == 'e') {
        after_exponent = true;
        last_token.push_back(c);
        continue;
      }
      if (std::isdigit(c) && !no_more_digits) {
        last_token.push_back(c);
        continue;
      }
      loop = false;
      ungetch();
    }
    return token_type = Token::NUMBER;
  }
  // string

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
  return token_type = Token::UNKNOWN;
}
