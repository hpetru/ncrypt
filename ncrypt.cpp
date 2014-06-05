#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <math.h>

namespace ncrypt
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
    std::vector<std::vector<int> > c_key = ncrypt::make_c_key(_c_key_len, _c_key);
    this->set_c_key(c_key);
  }

  Key::Key(int _n)
  {
    this->init_key(_n);

    std::srand((unsigned)time(0));
    Key tmp = *this;
    for(int col = 0; col < _n; col++)
    {
      int line = std::rand() % _n;
      while(
            (tmp.is_center(col, line) ||
            (tmp[col][line] == -1)) &&
            (tmp.col_solutions_nr(col) > 0)
          )
      {
        line = std::rand() % _n;
      }
      if(!tmp.is_center(col, line))
      {
        int pos[2] = { col, line };
        tmp |= pos;
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

  int Key::col_solutions_nr(int col)
  {
    int nr = 0;
    for(int i = 0; i < this->size(); i++)
    {
      if(this->at(col).at(i) != -1 && !this->is_center(col, i)) nr++;
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

  std::string Encoder::encrypt_frame(std::string msg, Key key, std::string& buff, int& pos_ch)
  {
    int len = key.size();
    for(int i = 0; i < len; i++)
      for(int j = 0; j < len; j++)
        if(key[i][j])
        {
          int pos = i * len + j;
          buff[pos] = msg[pos_ch];
          pos_ch++;
        }
    return buff;
  }

  std::string Encoder::encrypt()
  {
    std::string buff(pow(this->key.size(), 2), '~');
    Key tmp_key = this->key;
    int pos_ch = 0;
    for(int i = 0; i < 4; i++)
    {
      this->encrypt_frame(msg, tmp_key, buff, pos_ch);
      tmp_key++;
    }

    /*
    // TODO: De vazut de ce lucreaza fara portiunea respectiva
    if(key.size() % 2)
    {
      int pos = ((key.size() + 1) / 2) - 1;
      int chr_pos = pos * (key.size()-1) + (key.size()-1);
      buff += msg[chr_pos];
    }
    */
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
      std::string decode_frame(std::string, Key);
    public:
      Decoder(std::string, Key);
      std::string to_s();
  };

  Decoder::Decoder(std::string msg, Key key)
  {
    this->key = key;
    this->msg = msg;
  }

  std::string Decoder::decode_frame(std::string msg, Key key)
  {
    std::string buff = "";
    int len = key.size();
    for(int i = 0; i < len; i++)
      for(int j = 0; j < len; j++)
        if(key[i][j])
        {
          int pos = i * len + j;
          buff += msg[pos];
        }
    return buff;
  }

  std::string Decoder::decode()
  {
    std::string buff = "";
    Key tmp_key = this->key;

    for(int i = 0; i < 4; i++)
    {
      buff += this->decode_frame(this->msg, tmp_key);
      tmp_key++;
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
  /*
  std::vector<std::vector<int> > c_key;
  int _n = 7;
  int _c_key[][2] =  {
    {1, 7}, {7, 6},
    {1, 3}, {1, 4},
    {3, 1}, {2, 1},
    {6, 6}, {2, 3},
    {4, 6}, {5, 6},
    {3, 5}, {4, 3}
  };

  for(int i = 0; i < 7; i++)
  {
    std::vector<int> tmp;
    tmp.push_back(_c_key[i][0]);
    tmp.push_back(_c_key[i][1]);

    c_key.push_back(tmp);
  }
  ncrypt::Key key(_n, c_key);
  */
  /*
  ncrypt::Key key(_n, c_key);
  key++;
  key.inspect();
  */

  /*
  ncrypt::Key key(3);
  key.inspect();
  */

  /*
  ncrypt::Key key(_n, c_key);
  ncrypt::Encoder encoder("ABCDEFGHIJKLMNOP", key);
  std::cout << encoder.to_s() << std::endl;
  */

  /*
  ncrypt::Encoder encoder("ABCDEFGHIJKLMNOP");
  */


  /*
  ncrypt::Encoder encoder("ABCDE");
  ncrypt::Key key = encoder.get_key();
  std::cout << encoder.to_s() << std::endl;

  ncrypt::Decoder decoder(encoder.to_s(), key);
  std::cout << decoder.to_s() << std::endl;
  */

  /*
  ncrypt::Key key(_n, c_key);
  ncrypt::Encoder encoder("AGILBDKNEHJPCFMO", key);
  std::cout << encoder.to_s() << std::endl;
  */
  /*
  ncrypt::Key key(_n, c_key);
  ncrypt::Encoder encoder("ABCDEFGHIJKLMNOP", key);
  std::cout << encoder.to_s() << std::endl;

  ncrypt::Decoder decoder("omw.mnwohtisdggm~~b.de.c~~aolr~~~.oadfciinoc.i/w.", key);
  std::cout << decoder.to_s() << std::endl;
  */
  /*
  std::ifstream in("./ncrypt.in");
  std::string buff;
  std::getline(in, buff);

  std::cout << "==================Textul initial===============" << std::endl;
  std::cout << buff.size() << std::endl;

  ncrypt::Encoder encoder(buff);
  std::cout << "==================Textul criptat===============" << std::endl;
  std::cout << encoder.to_s() << std::endl;

  ncrypt::Decoder decoder(encoder.to_s(), encoder.get_key());
  std::cout << "==================Textul decriptat=============" << std::endl;
  std::cout << decoder.to_s().size() << std::endl;

  std::cout << "==================Cheia========================" << std::endl;
  encoder.get_key().inspect();
  */
  /*
  int _n = 6;
  int _c_key[][2] =  {
    {1,2}, {1,4}, {1,6},
    {2,5},
    {3,3},
    {4,2}, {4,5},
    {5,6},
    {6,4}
  };
  ncrypt::Key key(_n, _c_key, 9);
  */

  int _n = 3;
  int _c_key[][2] = {
    {1,1}, {2,3}
  };
  ncrypt::Key key(_n, _c_key, 2);
  ncrypt::Encoder encoder("criptare");
  ncrypt::Decoder decoder(encoder.to_s(), encoder.get_key());

  std::cout << encoder.to_s() << std::endl;
  std::cout << decoder.to_s() << std::endl;
  encoder.get_key().inspect();

  return 0;
}
