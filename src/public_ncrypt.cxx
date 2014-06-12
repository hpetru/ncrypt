#include <iostream>
#include "public_ncrypt.hpp"

int main(int argc, char* argv[])
{
  std::string file_path = argv[2];
  std::string input;

  public_ncrypt::Key key(file_path);

  if(std::string(argv[1]) == "-enc")
  {
    std::cout << "Rapid scrie mesajul care trebuie criptat !! )" << std::endl;
    std::getline(std::cin, input);

    public_ncrypt::Encriptor encriptor(input, key);
    std::cout << encriptor.to_s() << std::endl;
  }
  else
  {
    std::cout << "Ce secret vreai sa iti decriptez ?" << std::endl;
    std::getline(std::cin, input);

    public_ncrypt::Decriptor decriptor(input, key);
    std::cout << decriptor.to_s() << std::endl;
  }

  return 0;
}
