#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdint>
#include <vector>

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <assembled file>" << std::endl;
    return 1;
  }

  std::ifstream file(argv[1], std::ios::binary);
  if (!file) {
    std::cerr << "Could not open file: " << argv[1] << std::endl;
    return 1;
  }
  
  file.seekg(0, std::ios::end);
  int fileSize = file.tellg();
  file.seekg(0, std::ios::beg);

  if (fileSize % sizeof(uint16_t) != 0) {
    std::cerr << "File size is not aligned to 16-bit integers\n";
    return 1;
  }

  std::size_t count = fileSize / sizeof(uint16_t);
  std::vector<uint16_t> data(count);

  file.read(reinterpret_cast<char*>(data.data()), fileSize);

  constexpr uint8_t opcode_mov = 136;
  constexpr uint8_t d = 2;
  constexpr uint8_t w = 1;
  constexpr uint8_t mod_mem_no = 0;
  constexpr uint8_t mod_mem_8_bit = 1;
  constexpr uint8_t mod_mem_16_bit = 2;
  constexpr uint8_t mod_register = 3;
  constexpr const char* reg_string[8][2] = {
    { "al", "ax" },
    { "cl", "cx" },
    { "dl", "dx" },
    { "bl", "bx" },
    { "ah", "sp" },
    { "ch", "bp" },
    { "dh", "si" },
    { "bh", "di" },
  };

  //std::cout << "; " << argv[1] << std::endl << std::endl;
  std::cout << "bits 16" << std::endl << std::endl;

  for (uint16_t word : data) {
    uint8_t high_byte = static_cast<uint8_t>(word >> 8);
    uint8_t low_byte = static_cast<uint8_t>(word & 0xFF);
    /*
    std::cout << "word: " << std::to_string(word) << std::endl;
    std::cout << "high_byte: " << std::to_string(high_byte) << std::endl;
    std::cout << "low_byte: " << std::to_string(low_byte) << std::endl;
    */
    
    if ((low_byte & opcode_mov) == 0) {
      std::cerr << "Operation is not move operation" << std::endl;
      return 1;
    }

    uint8_t mod = high_byte >> 6;
    uint8_t reg = (high_byte >> 3) & 0x7;
    uint8_t r_m = high_byte & 0x7;

    if ((mod & mod_register) == 0) {
      std::cerr << "Operation is not register move" << std::endl;
      return 1;
    }

    std::string source;
    std::string destination;
    if (low_byte & d) {
      source      = reg_string[r_m][low_byte & w];
      destination = reg_string[reg][low_byte & w];
    } else {
      source      = reg_string[reg][low_byte & w];
      destination = reg_string[r_m][low_byte & w];
    }

    std::cout << "mov ";
    std::cout <<  destination << ", ";
    std::cout <<  source << " ";
    std::cout << std::endl;
  }

  return 0;
}
