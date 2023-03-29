#ifndef MYJSON_H
#define MYJSON_H
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

enum class Token {
  BOOLEAN,   // 0
  NUMBER,    // 1
  STRING,    // 2
  COMMA,     // 3
  TNULL,     // 4
  OPEN_ARR,  // 5
  CLOSE_ARR, // 6
  COLON,     // 7
  OPEN_BRA,  // 8
  CLOSE_BRA, // 9
  UNKNOWN,
  END
};

enum class Context {
  INSIDE_CURLY,
  INSIDE_ARRAY,
  AFTER_COLON,
  AFTER_COMMA,
  NOTHING
};

enum class Data_type { OBJECT, BOOLEAN, NUMBER, ARRAY, TNULL, STRING };
class Json_entity {
public:
  virtual Data_type get_type() = 0;
};

class Json_obj : public Json_entity {
public:
  Data_type get_type() { return Data_type::OBJECT; };
  std::unordered_map<std::string, std::shared_ptr<Json_entity>> &get_value() { return v; }

private:
  std::unordered_map<std::string, std::shared_ptr<Json_entity>> v;
};

class Json_arr : public Json_entity {
public:
  Data_type get_type() { return Data_type::ARRAY; };
  std::vector<std::shared_ptr<Json_entity>> &get_value() { return v; }

private:
  std::vector<std::shared_ptr<Json_entity>> v;
};

class Json_number : public Json_entity {
public:
  Json_number(double d) : v(d){};
  Data_type get_type() { return Data_type::NUMBER; };
  double &get_value() { return v; }

private:
  double v;
};

class Json_string : public Json_entity {
public:
  Json_string(std::string s) : v(s){};
  Data_type get_type() { return Data_type::STRING; };
  std::string &get_value() { return v; }

private:
  std::string v;
};

class Json_bool : public Json_entity {
public:
  Json_bool(bool v) : v(v){};
  bool &get_value() { return v; }
  Data_type get_type() { return Data_type::BOOLEAN; }

private:
  bool v;
};

class Json_null : public Json_entity {
public:
  Data_type get_type() { return Data_type::TNULL; }

private:
};
#endif
