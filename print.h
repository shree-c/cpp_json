#include "myjson.h"

void print_spaces(int count);

enum class Color { RED, BLUE, GREEN, YELLOW, MEGENTA };

void print_json(std::shared_ptr<Json_entity>);

void print_array(std::shared_ptr<Json_entity> x);


void print_object(std::shared_ptr<Json_entity> x);

void print_json(std::shared_ptr<Json_entity> x);

void print_token_type(Token t);
