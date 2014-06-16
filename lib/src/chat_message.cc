#include <chat_message.hpp>

namespace chat_message
{
  // ------------Raw-------------------
  std::string Raw::get_message()
  {
    return msg_;
  }
  size_t Raw::get_length()
  {
    return msg_.size();
  }
  // ------------Encript----------------
  std::string Encript::get_message()
  {
    std::string buff = enc_.to_s();
    buff += Raw::delimiter;
    return buff;
  }
  size_t Encript::get_length()
  {
    return get_message().size();
  }
  // ------------Decript----------------
  std::string Decript::get_message()
  {
    std::string buff = dec_.to_s();
    return buff;
  }
  size_t Decript::get_length()
  {
    return get_message().size();
  }
}
