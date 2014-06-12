#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <math.h>

namespace grid
{
  // ---------------Helpers----------------------

  std::vector<std::vector<int> > make_c_key(int len, int _c_key[][2])
  {
    std::vector<std::vector<int> > c_key;
    for(int i = 0; i < len; i++)
    {
      std::vector<int> tmp;
      tmp.push_back(_c_key[i][0]);
      tmp.push_back(_c_key[i][1]);

      c_key.push_back(tmp);
    }
    return c_key;
  }

  // ---------------Key class--------------------

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

  Key::Key(int _n, std::vector<std::vector<int> > c_key)
  {
    this->init_key(_n);
    this->set_c_key(c_key);
  }

  Key::Key(int _n, int _c_key[][2], int _c_key_len)
  {
    this->init_key(_n);
    std::vector<std::vector<int> > c_key = grid::make_c_key(_c_key_len, _c_key);
    this->set_c_key(c_key);
  }

  Key::Key(int _n)
  {
    this->init_key(_n);

    std::srand((unsigned)time(0));
    Key tmp = *this;
    for(int grad = 0; grad <= _n / 2; grad++)
    {
      int j = 0;
      while(j < _n-grad-1)
      {
        int line, col;
        col = grad + std::rand() % (_n-grad);
        line = std::rand() % 2;

        line = line ? grad : _n-grad-1;

        while(
              (tmp.is_center(col, line) ||
              (tmp[col][line] != 0))    &&
              (tmp.col_solutions_nr(grad) > 0)
            )
        {
          col = grad + std::rand() % (_n-grad);
          line = line ? grad : _n-grad-1;
        }
        if(!tmp.is_center(col, line))
        {
          int pos[2] = { col, line };
          tmp |= pos;
          j++;
        }
      }
    }
    *this = tmp.normalize();
  }

  void Key::init_key(int size)
  {
    for(int i = 0; i < size; i++)
    {
      this->push_back(std::vector<int>(size, 0));
    }
  }

  void Key::set_c_key(std::vector<std::vector<int> > c_key)
  {
    for(int i = 0; i < c_key.size(); i++)
    {
      int m = c_key[i][0] - 1;
      int t = c_key[i][1] - 1;
      this->at(m).at(t) = 1;
    }
  }

  int Key::col_solutions_nr(int frame_nr)
  {
    int nr = 0;
    int len = this->size()-1;
    int range = len - frame_nr;

    if(this->is_center(frame_nr, frame_nr))
      return 0;

    for(int i = frame_nr; i <= range; i++)
    {
      nr += this->at(i).at(frame_nr) == 0 ? 1 : 0;
      nr += this->at(i).at(len-frame_nr) == 0 ? 1 : 0;
    }

    for(int i = frame_nr+1; i <= range-1; i++)
    {
      nr += this->at(frame_nr).at(i) == 0 ? 1 : 0;
      nr += this->at(len-frame_nr).at(i) == 0 ? 1 : 0;
    }

    return nr;
  }

  Key Key::normalize()
  {
    for(int i = 0; i < this->size(); i++)
    {
      for(int j =0; j < this->size(); j++)
        if((*this)[i][j] == -1) (*this)[i][j] = 0;
    }
    return *this;
  }

  bool Key::is_center(int x, int y)
  {
    if(!(this->size() % 2)) return 0;
    int _x = ((this->size() + 1) / 2) - 1;

    if((_x == x) && (_x == y))
      return 1;
    else
      return 0;
  }

  Key Key::operator |= (int pos[2])
  {
    int x = pos[0];
    int y = pos[1];
    for(int i = 0; i < 4; i++)
    {
      (*this)++;
      (*this)[x][y] = -1;
    }
    (*this)[x][y] = 1;
    return *this;
  }

  Key Key::operator ++(int)
  {
    Key tmp = *this;
    for(int i = 0; i < this->size(); i++)
    {
      for(int j = 0; j < this->size(); j++)
      {
        tmp[i][j] = this->at(this->size() - j - 1).at(i);
      }
    }
    *this = tmp;
    return tmp;
  }

  Key Key::operator --(int)
  {
    Key tmp = *this;
    for(int i = 0; i < this->size(); i++)
    {
      for(int j = 0; j < this->size(); j++)
      {
        tmp[i][j] = this->at(j).at(this->size() - i - 1);
      }
    }
    *this = tmp;
    return *this;
  }

  void Key::inspect()
  {
    int size = this->size();
    std::cout << "<Key[ size:" << size << std::endl;

    for(int i = 0; i < size; i++)
    {
      for(int j = 0; j < size; j++)
      {
        std::cout << this->at(i).at(j) << " ";
      }
      std::cout << std::endl;
    }
    std::cout << "]>" << std::endl;
  }

  // --------------Encoder class-----------------

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

  Encoder::Encoder(std::string msg)
  {
    this->msg = msg;

    int key_size = this->msg_key_size(msg);
    this->key = Key(key_size);
  }

  Encoder::Encoder(std::string msg, Key key)
  {
    this->msg = msg;
    this->key = key;
  }

  int Encoder::msg_key_size(std::string msg)
  {
    float _size = pow(msg.size(), 0.5);
    int size = 0;
    size = (_size - (int)_size > 0) ? (int)_size+1 : (int)_size;
    return size;
  }

  std::string Encoder::encrypt_frame(int frame_nr, std::string msg, Key key, std::string& buff, int& pos_ch)
  {
    int len = key.size()-1;

    for(int i = frame_nr; i <= len - frame_nr; i++)
      set_pos(i, frame_nr, msg, key, buff, pos_ch);

    for(int i = frame_nr+1; i <= len - frame_nr-1; i++)
      set_pos(frame_nr, i, msg, key, buff, pos_ch);

    for(int i = frame_nr+1; i <= len - frame_nr-1; i++)
      set_pos(len-frame_nr, i, msg, key, buff, pos_ch);

    for(int i = frame_nr; i <= len - frame_nr; i++)
      set_pos(i, len-frame_nr, msg, key, buff, pos_ch);

    return buff;
  }

  std::string Encoder::set_pos(int x, int y, std::string msg, Key key, std::string& buff, int& pos_ch)
  {
    int len = key.size();
    if(key[x][y])
    {
      int pos = x * len + y;
      if(pos_ch < msg.size())
      {
        buff[pos] = msg[pos_ch];
        pos_ch++;
      }
    }
    return buff;
  }

  std::string Encoder::encrypt()
  {
    std::string buff(pow(this->key.size(), 2), '~');
    Key tmp_key = this->key;
    int pos_ch = 0;
    for(int i = 0; i < tmp_key.size() / 2; i++)
    {
      for(int j = 0; j < 4; j++)
      {
        this->encrypt_frame(i, msg, tmp_key, buff, pos_ch);
        tmp_key++;
      }
    }

    // In cazul in care cheia are dimensiunea impara, plasez caracterul din centru
    if(key.size() % 2)
      buff += msg.at(msg.length()-1);

    return buff;
  }

  std::string Encoder::to_s()
  {
    return this->encrypt();
  }

  std::string Encoder::get_msg()
  {
    return this->msg;
  }

  Key Encoder::get_key()
  {
    return this->key;
  }

  // -------------------Decoder class------------

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

  Decoder::Decoder(std::string msg, Key key)
  {
    this->key = key;
    this->msg = msg;
  }

  std::string Decoder::decode_frame(int frame_nr, std::string msg, Key key)
  {
    std::string buff = "";

    int len = key.size()-1;
    for(int i = frame_nr; i <= len - frame_nr; i++)
      get_pos(i, frame_nr, msg, key, buff);

    for(int i = frame_nr+1; i <= len - frame_nr-1; i++)
      get_pos(frame_nr, i, msg, key, buff);

    for(int i = frame_nr+1; i <= len - frame_nr-1; i++)
      get_pos(len-frame_nr, i, msg, key, buff);

    for(int i = frame_nr; i <= len - frame_nr; i++)
      get_pos(i, len-frame_nr, msg, key, buff);

    return buff;
  }

  std::string Decoder::get_pos(int x, int y, std::string msg, Key key, std::string& buff)
  {
    int len = key.size();
    if(key[x][y])
    {
      int pos = x * len + y;
      buff += msg[pos];
    }
    return buff;
  }

  std::string Decoder::decode()
  {
    std::string buff = "";
    Key tmp_key = this->key;
    for(int i = 0; i < tmp_key.size() / 2; i++)
      for(int j = 0; j < 4; j++)
      {
        buff += this->decode_frame(i, this->msg, tmp_key);
        tmp_key++;
      }

    if(key.size() % 2)
    {
      buff += msg.at(msg.length()-1);
    }
    return buff;
  }

  std::string Decoder::to_s()
  {
    return this->decode();
  }

}
int main()
{
  std::cout << "Introduceti textul pentru criptare" << std::endl;
  std::cout << "___________________________________________________" << std::endl;

  std::string txt;
  std::getline(std::cin, txt);

  grid::Encoder encoder(txt);
  grid::Decoder decoder(encoder.to_s(), encoder.get_key());
  std::cout << "=================Cheia de criptare=================" << std::endl;
  encoder.get_key().inspect();
  std::cout << "=================Mesajul criptat===================" << std::endl;
  std::cout << encoder.to_s() << std::endl;

  std::cout << "=================Mesajul decriptat=================" << std::endl;
  std::cout << decoder.to_s() << std::endl;

  return 0;
}
