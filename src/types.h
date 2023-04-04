#ifndef MYJSON_H
#define MYJSON_H
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace SJSON {

// gurad to check against zero
enum class Token {
  GUARD,
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

class Json_entity;
class Json_obj;
class Json_arr;
class Json_number;
class Json_null;
class Json_bool;
class Json_string;

typedef std::shared_ptr<Json_entity> Json_entity_shared_ptr;
typedef std::shared_ptr<Json_obj> Json_obj_shared_ptr;
typedef std::shared_ptr<Json_bool> Json_bool_shared_ptr;
typedef std::shared_ptr<Json_number> Json_number_shared_ptr;
typedef std::shared_ptr<Json_null> Json_null_shared_ptr;
typedef std::shared_ptr<Json_string> Json_string_shared_ptr;
typedef std::shared_ptr<Json_arr> Json_arr_shared_ptr;

enum class Data_type { OBJECT, BOOLEAN, NUMBER, ARRAY, TNULL, STRING };
class Json_entity {
public:
  virtual Data_type get_type() = 0;
};

class Json_obj : public Json_entity {
public:
  Data_type get_type() { return Data_type::OBJECT; };
  std::unordered_map<std::string, Json_entity_shared_ptr> &get_value() {
    return v;
  }
  void insert(std::string key, Json_entity_shared_ptr value) {
    v.insert({key, value});
  }

private:
  std::unordered_map<std::string, Json_entity_shared_ptr> v;
};

class Json_arr : public Json_entity {
public:
  Data_type get_type() { return Data_type::ARRAY; };
  std::vector<Json_entity_shared_ptr> &get_value() { return v; }
  void insert(Json_entity_shared_ptr value) { v.push_back(value); }

private:
  std::vector<Json_entity_shared_ptr> v;
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
} // namespace SJSON
#endif
