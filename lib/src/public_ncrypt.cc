#include "public_ncrypt.hpp"

namespace public_ncrypt
{
  bool file_is_empty(std::ifstream& pFile)
  {
        return pFile.peek() == std::ifstream::traits_type::eof();
  }

  std::vector<std::string> split_string(std::string str, std::string del)
  {
    std::vector<std::string> vex;
    size_t pos = 0;
    std::string token;

    while ((pos = str.find(del)) != std::string::npos) {
        token = str.substr(0, pos);
        vex.push_back(token);
        str.erase(0, pos + del.length());
    }
    vex.push_back(str);
    return vex;
  }

  std::string inspect_sed_vector(std::vector<unsigned long int> vex)
  {
    std::string buff = "";
    for(int i = 0; i < vex.size(); i++)
    {
      buff += SSTR(vex[i]);
      buff += " ";
    }
    buff.erase(buff.end()-1, buff.end());
    return buff;
  }

  // Cel mai mare divizor comun
  unsigned long int cmmdc(int a, int b)
  {
    if(a == b)
      return a;
    if(a > b)
      return cmmdc(a-b, b);
    else
      return cmmdc(a, b-a);
  }

  // Inversa modulo
  int mul_inv(int a, int b)
  {
    int b0 = b, t, q;
    int x0 = 0, x1 = 1;
    if (b == 1) return 1;
    while (a > 1) {
      q = a / b;
      t = b, b = a % b, a = t;
      t = x0, x0 = x1 - q * x0, x1 = t;
    }
    if (x1 < 0) x1 += b0;
    return x1;
  }
  // ----------------------------------
  PublicKeyItem::PublicKeyItem(char ch, int code)
  {
    this->ch = ch;
    this->code = code;
  }

  void PublicKeyItem::inspect()
  {
    std::cout << this->ch << " |=> " << " ";
    printf(BYTETOBINARYPATTERN, BYTETOBINARY(this->code));
    std::cout << std::endl;
  }

  std::vector<int> PublicKeyItem::get_bytes()
  {
    int bites[8] = { BYTETOBINARY(this->code) };
    std::vector<int> buff(8, 0);
    for(int i = 0; i < 8; i++) buff[i] = bites[i];
    return buff;
  }

  char PublicKeyItem::get_char()
  {
    return this->ch;
  }
  // ----------------------------------
  PublicKeyTable::PublicKeyTable()
  {
    for(int i = 32; i < 127; i++)
    {
      this->push_back(PublicKeyItem((char)i, i-32));
    }
  }

  void PublicKeyTable::inspect()
  {
    for(int i = 0; i < size(); i++)
    {
      at(i).inspect();
    }
  }

  PublicKeyItem PublicKeyTable::find_by_bites(std::vector<int> bites)
  {
    for(int i = 0; i < this->size(); i++)
    {
      if(this->at(i).get_bytes() == bites)
        return this->at(i);
    }
  }

  PublicKeyItem PublicKeyTable::get_item(char ch)
  {
    for(int i = 0; i < this->size(); i++)
      if(this->at(i).get_char() == ch)
        return this->at(i);
  }
  // ----------------------------------
  Key::Key()
  {
    this->_init();
  }

  Key::Key(std::istream& in)
  {
    in >> (*this);
  }

  Key::Key(std::string file_name)
  {
    std::ofstream o_file;
    std::ifstream i_file;

    i_file.open(file_name.c_str());

    if(!file_is_empty(i_file) && i_file.good())
    {
      i_file >> (*this);
    }
    else
    {
      this->_init();
      o_file.open(file_name.c_str());

      o_file << (*this);
      o_file.close();
    }

    i_file.close();
  }

  void Key::_init()
  {
    this->table = PublicKeyTable();
    this->sed = this->_get_sed();

    this->modulo = this->_get_modulo(this->sed);
    this->multiplier = this->_get_multiplier(this->modulo);

    this->inverse_modulo = this->_get_inverse_modulo(this->multiplier, this->modulo);

    this->public_sed = this->_get_public_sed(this->sed, this->multiplier, this->modulo);
  }

  unsigned long int Key::get_inverse_modulo()
  {
    return this->inverse_modulo;
  }

  unsigned long int Key::get_modulo()
  {
    return this->modulo;
  }

  std::vector<unsigned long int> Key::get_public_sed()
  {
    return this->public_sed;
  }

  std::vector<unsigned long int> Key::get_sed()
  {
    return this->sed;
  }

  std::vector<unsigned long int> Key::_get_sed()
  {
    std::srand(std::time(0));
    unsigned long int _rand = 0;
    std::vector<unsigned long int> _sed;

    _rand = std::rand() % 10 + 1; _sed.push_back(_rand);
    _rand = std::rand() % (_rand+10 - _rand) + _rand; _sed.push_back(_rand);

    unsigned long int low = _sed.at(0);

    for(int i = 2; i < 16; i++)
    {
      low += _sed.at(i-1);

      std::srand(std::time(NULL));
      _rand = std::rand() % (low+50 - low) + low;

      _sed.push_back(_rand);
    }
    return _sed;
  }

  unsigned long int Key::_get_modulo(std::vector<unsigned long int> sed)
  {
    unsigned long int sum = 0;
    for(int i = 0; i < sed.size(); i++)
      sum += sed[i];
    std::srand(std::time(0));

    return std::rand() % (sum + 100 - sum) + sum;
  }

  unsigned long int Key::_get_multiplier(unsigned long int modulo)
  {
    std::srand(std::time(0));
    unsigned long int multiplier = (modulo / 2) + (std::rand() % 100);
    while(cmmdc(modulo, multiplier) != 1)
      multiplier = (modulo / 2) + (std::rand() % 100);

    return multiplier;
  }

  unsigned long int Key::_get_inverse_modulo(unsigned long int multiplier, unsigned long int modulo)
  {
    return mul_inv(multiplier, modulo);
  }

  std::vector<unsigned long int> Key::_get_public_sed(
      std::vector<unsigned long int> sed,
      unsigned long int multiplier,
      unsigned long int modulo)
  {
    std::vector<unsigned long int> public_sed;
    for(int i = 0; i < sed.size(); i++)
    {
      unsigned long int nr = (multiplier * sed[i]) % modulo;
      public_sed.push_back(nr);
    }
    return public_sed;
  }

  PublicKeyTable Key::get_table()
  {
    return this->table;
  }

  void Key::inspect()
  {
    std::cout << "<[Key modulo: " << this->modulo << " inverse_modulo: " << this->inverse_modulo << std::endl;
      std::cout << "\t sed: ";
      std::cout << public_ncrypt::inspect_sed_vector(this->sed) << std::endl;
    std::cout << "]>" << std::endl;
  }

  std::string Key::serialize()
  {
    std::string buff = "";

    buff += SSTR(this->modulo);
    buff += " ";

    buff += SSTR(this->multiplier);
    buff += " ";

    buff += SSTR(this->inverse_modulo);
    buff += '\n';

    buff += public_ncrypt::inspect_sed_vector(this->sed);
    buff += '\n';

    buff += public_ncrypt::inspect_sed_vector(this->public_sed);
    buff += '\n';

    return buff;
  }

  std::istream& operator >> (std::istream& in, Key& key)
  {
    std::string buff = "";
    std::vector<std::string> vex;

    std::getline(in, buff);
    vex = split_string(buff, " ");

    key.modulo = std::atoi(vex[0].c_str());
    key.multiplier = std::atoi(vex[1].c_str());
    key.inverse_modulo = std::atoi(vex[2].c_str());

    std::getline(in, buff);
    vex = split_string(buff, " ");
    for(int i = 0; i < vex.size(); i++)
    {
      key.sed.push_back(std::atoi(vex[i].c_str()));
    }

    std::getline(in, buff);
    vex = split_string(buff, " ");
    for(int i = 0; i < vex.size(); i++)
    {
      key.public_sed.push_back(std::atoi(vex[i].c_str()));
    }

    return in;
  }

  std::ostream& operator << (std::ostream& out, Key key)
  {
    out << key.serialize();
    return out;
  }

  // ----------------------------------
  Encriptor::Encriptor(std::string msg, Key key)
  {
    this->msg = msg;
    this->key = key;
  }

  Encriptor::Encriptor(std::string msg)
  {
    this->msg = msg;
    this->key = Key();
  }

  std::string Encriptor::encrypt_pair(std::string pair_str)
  {
    unsigned long int sum = 0;
    if(pair_str.size() == 1) pair_str += " ";
    std::vector<int> bites;

    PublicKeyTable public_table = this->key.get_table();
    for(int i = 0; i < 2; i++)
    {
      PublicKeyItem item = public_table.get_item(pair_str[i]);
      std::vector<int> _bites = item.get_bytes();
      bites.insert(bites.end(), _bites.begin(), _bites.end());
    }

    std::vector<unsigned long int> public_sed = this->key.get_public_sed();
    for(int i = 0; i < bites.size(); i++)
    {
      sum += bites[i] * public_sed[i];
    }

    return SSTR(sum);
  }

  std::vector<std::string> Encriptor::encrypt()
  {
    std::vector<std::string> res;
    int i = 0;
    for(int i = 0; i < this->msg.size(); i += 2)
    {
      res.push_back(this->encrypt_pair(msg.substr(i, 2)));
    }
    std::string pair_str = this->msg.substr(i, 2);
    return res;
  }

  std::string Encriptor::to_s()
  {
    std::string buff = "";
    std::vector<std::string> fragments = this->encrypt();
    for(int i = 0; i < fragments.size(); i++)
    {
      buff += fragments[i];
      buff += " ";
    }
    buff.erase(buff.end()-1, buff.end());
    return buff;
  }

  Key Encriptor::get_key()
  {
    return this->key;
  }
  // ----------------------------------
  Decriptor::Decriptor(std::string msg, Key key)
  {
    this->msg = msg;
    this->key = key;
  }

  std::vector<unsigned long int> Decriptor::msg_to_vector(std::string msg)
  {
    std::vector<unsigned long int> fragments;
    std::vector<std::string> fragments_str = public_ncrypt::split_string(msg, " ");

    for(int i = 0; i < fragments_str.size(); i++)
      fragments.push_back(std::atoi(fragments_str[i].c_str()));

    return fragments;
  }

  std::string Decriptor::decrypt_fragment(unsigned long int fragment)
  {
    std::string buff = "";

    unsigned long int res = (fragment * this->key.get_inverse_modulo()) % this->key.get_modulo();
    std::vector<unsigned long int> sed = this->key.get_sed();
    std::vector<int> bites;


    for(int i = 15; i >= 0; i--)
    {
      if(sed[i] > res)
        bites.push_back(0);
      else
      {
        bites.push_back(1);
        res -= sed[i];
      }
    }

    std::reverse(bites.begin(), bites.end());
    PublicKeyTable table = this->key.get_table();

    std::vector<int> bites1(bites.begin(), bites.begin() + 8);
    std::vector<int> bites2(bites.begin() + 8, bites.end());

    PublicKeyItem _item1 = table.find_by_bites(bites1);
    PublicKeyItem _item2 = table.find_by_bites(bites2);

    buff += _item1.get_char();
    buff += _item2.get_char();

    return buff;
  }

  std::string Decriptor::decrypt()
  {
    std::vector<unsigned long int> fragments = this->msg_to_vector(this->msg);
    std::string buff = "";

    for(int i = 0; i < fragments.size(); i++)
    {
      buff += this->decrypt_fragment(fragments[i]);
    }
    return buff;
  }

  std::string Decriptor::to_s()
  {
    return this->decrypt();
  }
}
