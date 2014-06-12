#ifndef LIB_GRID_NCRYPT_HPP
#define LIB_GRID_NCRYPT_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <math.h>

namespace grid_ncrypt
{

  class Key : public std::vector<std::vector<int> >
  {
    private:
      void init_key(int size);
      int col_solutions_nr(int);
      bool is_center(int x, int y);
      void set_c_key(std::vector<std::vector<int> >);
      std::vector<std::vector<int> > make_c_key(int[][2]);

      Key normalize();
      Key operator |= (int[2]); // exclud colurile

    public:
      Key() {};
      Key(int, std::vector<std::vector<int> >);
      Key(int);             // se genereaza automat
      Key(int, int[][2], int);

      void inspect();

      Key operator ++(int); // rotarea cheii la 90 grade
      Key operator --(int); // rotarea cheii la -90 grade
  };

  class Encoder
  {
    private:
      std::string msg;
      Key key;

      int msg_key_size(std::string);
      std::string encrypt_frame(std::string, Key, std::string&, int&);
      std::string encrypt_frame(int, std::string, Key, std::string&, int&);
      std::string set_pos(int x, int y, std::string msg, Key key, std::string& buff, int& pos_ch);
      std::string encrypt();

    public:
      Encoder(std::string); // automat genereaza cheia
      Encoder(std::string, Key);

      Key get_key();
      std::string get_msg();

      std::string to_s();
  };

  class Decoder
  {
    private:
      Key key;
      std::string msg;
      std::string decode();
      std::string decode_frame(int, std::string, Key);
      std::string get_pos(int x, int y, std::string msg, Key key, std::string& buff);
    public:
      Decoder(std::string, Key);
      std::string to_s();
  };

}

#endif
