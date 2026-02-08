#include <iostream>
#include <fstream>
#include <cstdint>
#include "sim86_shared.h"

constexpr uint16_t TotalRegisters = 4*3 + 2; // General Purpose (AX, BX, CX, DX), Segment (CS, DS, SS, ES), Pointer & Index (SP, BP, SI, DI), and Flag/Instruction Pointer registers
uint16_t registers[TotalRegisters + 1]{}; // Casey's register index starts with 1 instead of 0;

const char* register_string[TotalRegisters + 1] = {
  "",
  "AX",
  "BX",
  "CX",
  "DX",
  "SP",
  "BP",
  "SI",
  "DI",
  "ES",
  "CS",
  "SS",
  "DS",
  "IP",
  "FLAGS"
};

void PrintAllRegisters() {
  for (int i=1; i < TotalRegisters + 1; i++) {
    std::cout << register_string[i] << ": ";
    std::cout << std::hex << registers[i] << " ";
    std::cout << "(" << std::dec << registers[i] << ")";
    std::cout << std::endl;
  }
}

int32_t GetDataFromRegister(const register_access& Reg) {
  uint16_t reg = registers[Reg.Index];
  if (Reg.Count == 2) {
    return static_cast<uint32_t>(reg);
  }
  if (Reg.Offset == 0) {
    return static_cast<uint32_t>(reg & 0xff);
  }
  if (Reg.Offset == 1) {
    return static_cast<uint32_t>(reg >> 8);
  }
  return 0;
}

void StoreDataInRegister(const register_access& Reg, int32_t cdata) {
  uint16_t Data_16 = static_cast<uint16_t>(cdata); // Casey's lib is using 32 bit signed data

  uint16_t* reg = &registers[Reg.Index];
  if (Reg.Count == 2) {
    *reg = Data_16;
    return;
  }

  uint8_t Data_8 = static_cast<uint8_t>(Data_16);
  if (Reg.Offset == 0) {
    *reg = (*reg & 0xff00) | Data_8;
  } 
  else if (Reg.Offset == 1) {
    *reg = (*reg & 0x00ff) | (static_cast<uint16_t>(Data_8) << 8);
  }

  return;
}

void ProcessInstruction(const instruction& Instruction) {
  instruction_operand destination = Instruction.Operands[0];
  instruction_operand source      = Instruction.Operands[1];

  // First homework is for non-memory moves
  if (Instruction.Op != Op_mov) {
    std::cerr << "Not mov instruction" << std::endl;
    return;
  }
  // Take out data from destination, place it in source
  int32_t data = 0;
  if (source.Type == Operand_Immediate) {
    data = source.Immediate.Value;
  } else if (source.Type == Operand_Register) {
    data = GetDataFromRegister(source.Register);
  }
  // destination can only be register at this point
  if (destination.Type != Operand_Register) {
    std::cerr << "Unexpected destination" << std::endl;
    return;
  }
  StoreDataInRegister(destination.Register, data);
}

int main(int argc, char** argv) {

  if (SIM86_VERSION != Sim86_GetVersion()) {
    std::cerr << "Incorrect lib version: " << Sim86_GetVersion() << ", with header version " << SIM86_VERSION << std::endl;
    return 1;
  }

  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <assembled file>" << std::endl;
    return 1;
  }

  std::ifstream file(argv[1], std::ios::binary);
  if (!file) {
    std::cerr << "Could not open file: " << argv[1] << std::endl;
    return 1;
  }

  uint32_t MainMemSize = (1 << 20);
  uint8_t *source = new uint8_t[MainMemSize];

  file.read(reinterpret_cast<char*>(source), MainMemSize);
  uint32_t bytesRead = file.gcount();
  if (bytesRead == 0) {
    std::cerr << "Could not read file" << std::endl;
    return 1;
  }

  int count = 0;
  while (bytesRead) {
    instruction Instruction;
    Sim86_Decode8086Instruction(bytesRead, source, &Instruction);
    if (Instruction.Op == Op_None) {
      std::cerr << "Unable to decode instruction" << std::endl;
      return 1;
    }
    if (Instruction.Op == Op_mov) {
      std::cout << "Mov instruction" << std::endl;
    }
    ProcessInstruction(Instruction);
    count++;
    bytesRead -= Instruction.Size;
    source += Instruction.Size;
  }

  std::cout << "decoding complete, instruction count: " << count << std::endl;
  PrintAllRegisters();

  return 0;
}
