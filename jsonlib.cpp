#include "myjson.h"
#include <iostream>
#include <map>
#include <memory>
#include <vector>

int main() {
  Json_obj s_obj;
  auto &x = s_obj;
  if (x.get_type() == Data_type::OBJECT)
    std::cout << "ok\n";
  // unique pointer cannot be copied
  std::map<std::string, std::unique_ptr<Json_entity>> ref_to_obj_map =
      std::move(x.get_value());
  ref_to_obj_map.insert({"hie", std::make_unique<Json_number>(20)});
  ref_to_obj_map.insert({"hello", std::make_unique<Json_bool>(true)});
  auto y = ref_to_obj_map.find("hie");
  std::cout << y->first;
  if (y->second->get_type() == Data_type::NUMBER)
    std::cout << "num\n";
  auto num = dynamic_cast<Json_number *>(y->second.get());
  std::cout << num->get_value();
  auto b = ref_to_obj_map.find("hello");
  auto b_v = dynamic_cast<Json_bool *>(b->second.get());
  std::cout << b_v->get_value();

  //  Json_number hello = dynamic_cast<Json_number &>(y->second);
  // if (y->second->get_type() == Data_type::NUMBER) {
  //  std::cout << "num\n";
  //}
  // making {"hello": 20}
  // mapref s_val = s_obj.get_value();
  // Json_number s_num(20);
  // s_val["hello"] = std::make_unique<Json_number>(20);
  // // i dont know about types now I just have start object
  // Json_entity &start = s_obj;
  // Data_type d = start.get_type();
  // if (d == Data_type::OBJECT) {
  //   std::cout << "object";
  // }
}
