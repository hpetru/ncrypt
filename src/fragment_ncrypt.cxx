#include <iostream>
#include "fragment_ncrypt.hpp"

int main(int argc, char* argv[])
{
  unsigned long long int num;
  std::cout << "Numarul lui Ion este:" << std::endl;
  std::cin >> num;

  fragment_ncrypt::Encoder enc(num);
  std::cout << enc.to_s() << std::endl;

  fragment_ncrypt::Decoder dec(enc.to_a());
  std::cout << dec.to_s() << std::endl;

  return 0;
}
