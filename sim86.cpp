#include <iostream>
#include "sim86_shared.h"

int main() {
  std::cout << (SIM86_VERSION == Sim86_GetVersion()) << std::endl;
  return 0;
}
