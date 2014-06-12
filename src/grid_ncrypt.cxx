#include <iostream>
#include "grid_ncrypt.hpp"

int main()
{
  std::cout << "Introduceti textul pentru criptare" << std::endl;
  std::cout << "___________________________________________________" << std::endl;

  std::string txt;
  std::getline(std::cin, txt);

  grid_ncrypt::Encoder encoder(txt);
  grid_ncrypt::Decoder decoder(encoder.to_s(), encoder.get_key());
  std::cout << "=================Cheia de criptare=================" << std::endl;
  encoder.get_key().inspect();
  std::cout << "=================Mesajul criptat===================" << std::endl;
  std::cout << encoder.to_s() << std::endl;

  std::cout << "=================Mesajul decriptat=================" << std::endl;
  std::cout << decoder.to_s() << std::endl;

  return 0;
}
