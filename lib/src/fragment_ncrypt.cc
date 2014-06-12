#include "fragment_ncrypt.hpp"

namespace fragment_ncrypt
{
  // ----------------------------------------
  Encoder::Encoder(unsigned long long int msg)
  {
    this->msg = msg;
  }

  std::string Encoder::to_s()
  {
    std::string buff = "";
    std::vector<std::vector<unsigned long long int> > tmp = this->encode();

    for(int i = 0; i < tmp.size(); i++)
    {
      buff += SSTR(tmp[i][0]);
      buff += " ";
      buff += SSTR(tmp[i][1]);
      buff += '\n';
    }
    return buff;
  }

  std::vector<std::vector<unsigned long long int> > Encoder::encode()
  {
    std::vector<int> sed;
    std::vector<unsigned long long int> res;
    std::vector<unsigned int> index;
    std::vector<std::vector<unsigned long long int> > fragments;

    int n = std::rand() % 4 + 4;
    int k = 0;
    do
    {
      k = std::rand() % 4 + 3;
    }while(n <= k);

    for(int i = 0; i < k-1; i++)
    {
      sed.push_back(std::rand() % 100 + 1);
    }

    for(int i = 1; i <= n; i++)
    {
      unsigned long long int nr = this->msg;
      for(int j = 1; j < k; j++)
      {
        nr += sed[j] * pow(i, j);
      }
      res.push_back(nr);
    }

    for(int i = 0; i < k; i++)
    {
      int item = std::rand() % (n-1) + 1;

      while(std::find(index.begin(), index.end(), item)!=index.end())
        item = std::rand() % (n-1) + 1;

      index.push_back(item);
    }

    for(int i = 0; i < index.size(); i++)
    {
      std::vector<unsigned long long int> tmp;

      tmp.push_back(index[i] + 1);
      tmp.push_back(res[index[i]]);

      fragments.push_back(tmp);
    }

    return fragments;
  }

  std::vector<std::vector<unsigned long long int> > Encoder::to_a()
  {
    return this->encode();
  }
  // ----------------------------------------
  Decoder::Decoder(std::vector<std::vector<unsigned long long int> > msg)
  {
    this->msg = msg;
  }

  unsigned long long int Decoder::decode()
  {
    float res = 0;
    long int _res_a = 1;
    long int _res_b = 1;
    float _res = 0;

    for(int i = 0; i < msg.size(); i++)
    {
      for(int j = 0; j < msg.size(); j++)
      {
        if(j != i)
        {
          _res_a *= msg[j][0];
          _res_b *= ((int)msg[i][0] - (int)msg[j][0]);
        }
      }
      res -= ((float)_res_a / _res_b) * msg[i][1];

      _res_a = 1;
      _res_b = 1;
    }

    res = round(res);
    return (unsigned long long int)std::abs((int)res);
  }

  std::string Decoder::to_s()
  {
    return SSTR(this->decode());
  }

}
