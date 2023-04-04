#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <exception>
#include <string>

class SJSONException : public std::exception {
public:
  SJSONException(const std::string &message) : _message(message) {};

  const char * what() const noexcept override {
    return _message.c_str();
  }
private:
  std::string _message;
};
#endif
