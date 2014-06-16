#ifndef CHAT_MESSAGE_HPP
#define CHAT_MESSAGE_HPP

#include <string>
#include <public_ncrypt.hpp>

namespace chat_message
{
  class Raw
  {
    public:
      enum { delimiter = '\n' };

      Raw(std::string msg) : msg_(msg) {}
      Raw() {}
      std::string get_message();
      size_t get_length();

    private:
      std::string msg_;
  };

  class Encript : public Raw
  {
    public:
      Encript(std::string msg, public_ncrypt::Key& key)
        : Raw(msg),
          key_(key),
          enc_(msg, key)
      {}

      std::string get_message();
      size_t get_length();

    private:
      public_ncrypt::Key& key_;
      std::string smsg_;
      public_ncrypt::Encriptor enc_;
  };

  class Decript : public Raw
  {
    public:
      Decript(std::string msg, public_ncrypt::Key& key)
        : Raw(msg),
          key_(key),
          dec_(msg, key)
      {
      }

      std::string get_message();
      size_t get_length();

    private:
      public_ncrypt::Key& key_;
      public_ncrypt::Decriptor dec_;
  };
}

#endif // CHAT_MESSAGE_HPP
