if (context.back() == Context::INSIDE_CURLY) {
  std::shared_ptr<Json_obj> temp_obj_ptr =
      std::dynamic_pointer_cast<Json_obj>(mouth);
  temp_obj_ptr->get_value().insert({last_key, new_obj});
} else if (context.back() == Context::INSIDE_ARRAY) {
  std::shared_ptr<Json_arr> temp_arr_ptr =
      std::dynamic_pointer_cast<Json_arr>(mouth);
  temp_arr_ptr->get_value().push_back(new_obj);
} else {
  mouth_stack.push_back(new_obj);
}
