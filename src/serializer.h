#include "tokenizer.h"
#include "types.h"
#include <array>

namespace SJSON {

class Serializer {
  // calls tokenizer
public:
  Serializer(std::string &json_string) : t(Tokenizer(json_string)){};
  Json_entity_shared_ptr serialize();

private:
  // tokenizer object
  Tokenizer t;
  // state
  std::vector<Json_entity_shared_ptr> mouth_stack;
  // points to top most container on mouth stack
  Json_entity_shared_ptr mouth = nullptr;
  std::vector<Context> context{Context::NOTHING};
  Token last_token_type = Token::UNKNOWN;
  std::string last_key;
  std::array<Token, 10> expected_token = {Token::OPEN_BRA, Token::OPEN_ARR,
                                          Token::BOOLEAN,  Token::NUMBER,
                                          Token::STRING,   Token::TNULL};

  // helper functions
  inline bool includes(std::array<Token, 10> &, Token);
  inline bool strToBool(const std::string &);
  void handle_premitive(Token);
  void update_mouth(Json_entity_shared_ptr);
  // overloaded push
  void push(std::string, Json_entity_shared_ptr);
  void push(Json_entity_shared_ptr);
};
} // namespace SJSON
