#include "myjson.h"
#include <iostream>

class Tokenizer {
public:
  Tokenizer(std::string &json_str) : buffer{json_str} {};
  Token gettoken();
  Token token_type;
  static void print_token_name(Token, std::ostream &);
  int count{0};
  std::string get_last_token() { return last_token; }

private:
  std::string &buffer;
  char getch() {
    if (buffer.length() == count) {
      return -1;
    }
    return buffer[count++];
  }
  void ungetch() {
    count--;
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
  char c = '\0';
  while ((c = getch()) == ' ' || c == '\t' || c == '\n' || c == '\r')
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
    ungetch();
    return token_type = Token::NUMBER;
  }
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
      // delete is not considered as control character but iscontrol returns
      if (std::iscntrl(c) && c != 127) {
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
