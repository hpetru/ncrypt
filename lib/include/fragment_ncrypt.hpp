#ifndef LIB_FRAGMENT_NCRYPT_HPP
#define LIB_FRAGMENT_NCRYPT_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <math.h>
#include <sstream>
#include <algorithm>


#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
            ( std::ostringstream() << std::dec << x ) ).str()

namespace fragment_ncrypt
{
  // ----------------------------------------
  class Encoder
  {
    unsigned long long int msg;

    public:
      Encoder(unsigned long long int);

      std::string to_s();
      std::vector<std::vector<unsigned long long int> > to_a();
      std::vector<std::vector<unsigned long long int> > encode();
  };
  // ----------------------------------------
  class Decoder
  {
    private:
      std::vector<std::vector<unsigned long long int> > msg;

    public:
      Decoder(std::vector<std::vector<unsigned long long int> >);
      std::string to_s();
      unsigned long long int decode();
  };
  // ----------------------------------------
}

#endif
