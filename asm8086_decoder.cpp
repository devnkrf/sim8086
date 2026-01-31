#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdint>
#include <vector>

constexpr uint8_t opcode_mov_reg_mem = 136;
constexpr uint8_t opcode_mov_immediate_to_reg_mem = 198;
constexpr uint8_t opcode_mov_immediate_to_reg = 176;

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

void processMovRegMem(const std::vector<uint8_t>& buffer, size_t& index) {
  uint8_t low_byte = buffer[index++];
  uint8_t high_byte = buffer[index++];

  uint8_t mod = high_byte >> 6;
  uint8_t reg = (high_byte >> 3) & 0x7;
  uint8_t r_m = high_byte & 0x7;
  uint8_t flag_w = low_byte & 1;
  uint8_t flag_d = low_byte & 2;

  std::string source;
  std::string destination;
  if (flag_d) {
    source      = reg_string[r_m][flag_w];
    destination = reg_string[reg][flag_w];
  } else {
    source      = reg_string[reg][flag_w];
    destination = reg_string[r_m][flag_w];
  }

  std::cout << "mov ";
  std::cout <<  destination << ", ";
  std::cout <<  source << " ";
  std::cout << std::endl;
}

void processMovImmediateReg(const std::vector<uint8_t>& buffer, size_t& index) {

}

int processMoveOperation(const std::vector<uint8_t>& buffer) {
  size_t size = buffer.size();
  size_t index = 0;
  while (index < size) {
    uint8_t low_byte = buffer[index];
    if (low_byte & opcode_mov_reg_mem) 
    {
      processMovRegMem(buffer, index);
    } 
    else if (low_byte & opcode_mov_immediate_to_reg_mem) 
    {
      std::cerr << "Not implemented" << std::endl;
      return 1;
    } 
    else if (low_byte & opcode_mov_immediate_to_reg) 
    {
      std::cerr << "Not implemented" << std::endl;
      return 1;
    } 
    else 
    {
      std::cerr << "Operation is not move operation" << std::endl;
      return 1;
    }
  }
  return 0;
}

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

  std::vector<uint8_t> buffer(fileSize);

  file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());

  //std::cout << "; " << argv[1] << std::endl << std::endl;
  std::cout << "bits 16" << std::endl << std::endl;

  int result = processMoveOperation(buffer);

  return result;
}
