#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdint>
#include <vector>
#include <bitset>

constexpr uint8_t opcode_mov_reg_mem = 136;
constexpr uint8_t opcode_mov_immediate_to_reg_mem = 198;
constexpr uint8_t opcode_mov_immediate_to_reg = 176;
constexpr uint8_t opcode_arith_immediate_to_reg_mem = 128;

constexpr uint8_t op_arith_add = 0;
constexpr uint8_t op_arith_sub = 5;
constexpr uint8_t op_arith_cmp = 7;

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

constexpr const char* effective_addr_string[8] = {
  "bx + si",
  "bx + di",
  "bp + si",
  "bp + di",
  "si",
  "di",
  "bp",
  "bx"
};

void printOpString(const std::string& operation, const std::string& destination, const std::string& source) {
  std::cout << operation << " ";
  std::cout <<  destination << ", ";
  std::cout <<  source << std::endl;
}

constexpr bool hasOpcode(uint8_t value, uint8_t opcode) {
  return (value & opcode) == opcode;
}

void processMovRegMem(const std::vector<uint8_t>& buffer, size_t& index) {
  uint8_t low_byte = buffer[index++];
  uint8_t high_byte = buffer[index++];

  uint8_t mod = high_byte >> 6;
  uint8_t reg = (high_byte >> 3) & 0x7;
  uint8_t r_m = high_byte & 0x7;
  bool flag_w = low_byte & 1;
  bool flag_d = low_byte & 2;

  uint8_t data_1 = 0;
  uint8_t data_2 = 0;
  uint16_t data = 0;
  if (mod == 2 || mod == 1 || (mod == 0 && r_m == 6)) {
    data_1 = buffer[index++];
  }
  if (mod == 2 || (mod == 0 && r_m == 6 && flag_w == true)) {
    data_2 = buffer[index++];
  }
  if (data_2) {
    data = static_cast<uint16_t>(data_2 << 8) | data_1;
  } else {
    data = static_cast<uint16_t>(data_1);
  }

  std::string source;
  std::string destination;
  if (mod == 3)
  {
    source      = reg_string[r_m][flag_w];
    destination = reg_string[reg][flag_w];
  }
  else if (mod == 2 || mod == 1)
  {
    std::string suffix = data ? std::string(" + ") + std::to_string(data) : "";
    source = std::string("[") + effective_addr_string[r_m] + suffix + "]";
    destination = reg_string[reg][flag_w];
  }
  else if (mod == 0 && r_m == 6)
  {
    source = std::string("[") + std::to_string(data) + "]";
    destination = reg_string[reg][flag_w];
  }
  else
  {
    source = std::string("[") + effective_addr_string[r_m] + "]";
    destination = reg_string[reg][flag_w];
  }

  if (flag_d) {
    printOpString("mov", destination, source);
  } else {
    printOpString("mov", source, destination);
  }
}

void processMovImmediateReg(const std::vector<uint8_t>& buffer, size_t& index) {
  uint8_t low_byte = buffer[index++];

  uint8_t reg    = low_byte & 7;
  bool flag_w = low_byte & 8;

  uint8_t data_1 = buffer[index++];
  uint8_t data_2 = 0;
  if(flag_w) data_2 = buffer[index++];

  std::string destination = reg_string[reg][flag_w];
  std::string source;
  if(flag_w) {
    uint16_t result = static_cast<uint16_t>(data_2 << 8) | data_1;
    source = std::to_string(result);
  } else {
    source = std::to_string(data_1);
  }

  printOpString("mov", destination, source);
}

void opImmediateToMem(const std::vector<uint8_t>& buffer, size_t& index) {
  uint8_t low_byte  = buffer[index++];
  uint8_t high_byte = buffer[index++];

  bool flag_w = low_byte & 1;
  uint8_t mod = high_byte >> 6;
  uint8_t r_m = high_byte & 7;
  uint8_t op  = (high_byte >> 3) & 7;

  uint8_t disp_hi = 0;
  uint8_t disp    = 0;

  if (mod == mod_mem_8_bit || mod == mod_mem_16_bit) {
    disp = buffer[index++];
  }
  if (mod == mod_mem_16_bit) {
    disp_hi = buffer[index++];
    disp = static_cast<uint16_t>(disp_hi << 8) | disp;
  }

  std::string prefix = "byte";

  uint8_t data_1 = 0;
  uint8_t data   = 0;

  data = buffer[index++];
  if (flag_w) {
    prefix = "word";
    data_1 = buffer[index++];
    data = static_cast<uint16_t>(data_1 << 8) | data;
  }

  std::string source = prefix + " " + std::to_string(data);
  std::string destination;

  if (mod == mod_register)
  {
    destination = reg_string[r_m][flag_w];
  }
  else if (mod == mod_mem_16_bit || mod == mod_mem_8_bit)
  {
    std::string suffix = disp ? std::string(" + ") + std::to_string(disp) : "";
    destination = std::string("[") + effective_addr_string[r_m] + suffix + "]";
  }
  else if (mod == mod_mem_no && r_m == 6)
  {
    destination = std::string("[") + std::to_string(disp) + "]";
  }
  else
  {
    destination = std::string("[") + effective_addr_string[r_m] + "]";
  }

  std::string opString;
  if (hasOpcode(low_byte, opcode_mov_immediate_to_reg_mem))
  {
    opString = "mov";
  }
  else if (op == op_arith_add)
  {
    opString = "add";
  }
  else if (op == op_arith_sub)
  {
    opString = "sub";
  }
  else if (op == op_arith_cmp)
  {
    opString = "cmp";
  }

  printOpString(opString, destination, source);
}

int processOperations(const std::vector<uint8_t>& buffer) {
  size_t size = buffer.size();
  size_t index = 0;
  while (index < size) {
    uint8_t low_byte = buffer[index];
    if ( hasOpcode(low_byte, opcode_mov_immediate_to_reg) )
    {
      processMovImmediateReg(buffer, index);
    }
    else if ( hasOpcode(low_byte, opcode_mov_reg_mem) )
    {
      processMovRegMem(buffer, index);
    }
    else if (hasOpcode(low_byte, opcode_mov_immediate_to_reg_mem))
    {
      opImmediateToMem(buffer, index);
    }
    else if (hasOpcode(low_byte, opcode_arith_immediate_to_reg_mem))
    {
      opImmediateToMem(buffer, index);
    }
    else
    {
      std::cerr << "Operation [" << std::to_string(low_byte) << "] is unknown operation" << std::endl;
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

  for (uint8_t b : buffer) {
    std::cout << std::bitset<8>(b) << " ";
  }
  std::cout << std::endl << std::endl;

  //std::cout << "; " << argv[1] << std::endl << std::endl;
  std::cout << "bits 16" << std::endl << std::endl;

  int result = processOperations(buffer);

  return result;
}
