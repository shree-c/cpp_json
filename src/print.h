#include "types.h"
namespace SJSON {

enum class Color { RED, GREEN, YELLOW, BLUE, MEGENTA };

class Printer {
public:
  static void print_json(std::shared_ptr<Json_entity>);
  static void print_token_type(Token);

private:
  static int spaces_count;
  static void print_array(Json_entity_shared_ptr);
  static void print_object(std::shared_ptr<Json_entity>);
  static void print_spaces(int);
  static void print_colorful(std::string, Color, int);
};

} // namespace SJSON
