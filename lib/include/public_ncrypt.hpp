#ifndef LIB_PUBLIC_NCRYPT_HPP
#define LIB_PUBLIC_NCRYPT_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <math.h>
#include <sstream>
#include <stack>
#include <algorithm>

#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
            ( std::ostringstream() << std::dec << x ) ).str()

#define BYTETOBINARYPATTERN "%d%d%d%d%d%d%d%d"
#define BYTETOBINARY(byte)  \
  (byte & 0x80 ? 1 : 0), \
  (byte & 0x40 ? 1 : 0), \
  (byte & 0x20 ? 1 : 0), \
  (byte & 0x10 ? 1 : 0), \
  (byte & 0x08 ? 1 : 0), \
  (byte & 0x04 ? 1 : 0), \
  (byte & 0x02 ? 1 : 0), \
  (byte & 0x01 ? 1 : 0)

namespace public_ncrypt
{
  // -------------------------------------
  class PublicKeyItem
  {
    private:
      char ch;
      unsigned int  code;
    public:
      PublicKeyItem(char ch, int code);
      std::vector<int> get_bytes();
      void inspect();
      char get_char();
  };
  // -------------------------------------
  class PublicKeyTable : public std::vector<PublicKeyItem>
  {
    public:
      PublicKeyTable();
      void inspect();
      PublicKeyItem get_item(char);
      PublicKeyItem find_by_bites(std::vector<int>);
  };
  // -------------------------------------
  class Key
  {
    private:
      std::vector<unsigned long int> sed;
      std::vector<unsigned long int> public_sed;
      PublicKeyTable table;

      unsigned long int modulo;
      unsigned long int multiplier;
      unsigned long int inverse_modulo;

      std::vector<unsigned long int> _get_sed();
      std::vector<unsigned long int> _get_public_sed(
          std::vector<unsigned long int>, 
          unsigned long int,
          unsigned long int);
      unsigned long int _get_modulo(std::vector<unsigned long int>);
      unsigned long int _get_multiplier(unsigned long int);
      unsigned long int _get_inverse_modulo(unsigned long int, unsigned long int);
      void _init();

    public:
      Key();
      Key(std::istream&); // cheie neinitializata
      Key(std::string); // numele fisierului
      PublicKeyTable get_table();
      std::vector<unsigned long int> get_public_sed();
      std::vector<unsigned long int> get_sed();
      void inspect();
      unsigned long int get_inverse_modulo();
      unsigned long int get_modulo();
      std::string serialize();

      friend std::ostream &operator << (std::ostream&, Key);
      friend std::istream &operator >> (std::istream&, Key&);
  };
  // -------------------------------------
  class Encriptor
  {
    private:
      Key key;
      std::string msg;

      std::vector<std::string> encrypt();
      std::string encrypt_pair(std::string);
    public:
      Encriptor(std::string);
      Encriptor(std::string, Key);

      std::string to_s();
      Key get_key();
  };
  // -------------------------------------
  class Decriptor
  {
    private:
      std::string msg;
      Key key;

      std::string decrypt_fragment(unsigned long int);
      std::string decrypt();
      std::vector<unsigned long int> msg_to_vector(std::string msg);
    public:
      Decriptor(std::string msg, Key key);
      std::string to_s();
  };
  // -------------------------------------
}

#endif
