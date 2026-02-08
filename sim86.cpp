#include <iostream>
#include <fstream>
#include <cstdint>
#include <unordered_map>
#include "sim86_shared.h"

constexpr uint16_t TotalRegisters = 4*3 + 2; // General Purpose (AX, BX, CX, DX), Segment (CS, DS, SS, ES), Pointer & Index (SP, BP, SI, DI), and Flag/Instruction Pointer registers
uint16_t registers[TotalRegisters + 1]{}; // Casey's register index starts with 1 instead of 0;

constexpr uint16_t ZF_Bit = 6;
constexpr uint16_t SF_Bit = 7;
constexpr uint16_t Flag_Reg = TotalRegisters; // last register in 1 based indexing
constexpr uint16_t IP_Reg   = TotalRegisters - 1;

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

std::string GetFlagString() {
  std::string result;
  if (registers[Flag_Reg] & (1 << ZF_Bit)) {
    result += "Z";
  }
  if (registers[Flag_Reg] & (1 << SF_Bit)) {
    result += "S";
  }
  return result;
}

void PrintAllRegisters() {
  for (int i=1; i < TotalRegisters; i++) {
    if (registers[i] == 0) {
      continue;
    }
    std::cout << register_string[i] << ": ";
    std::cout << std::hex << registers[i] << " ";
    std::cout << "(" << std::dec << registers[i] << ")";
    std::cout << std::endl;
  }
  if (registers[Flag_Reg] != 0) {
    std::cout << "Flags: " << GetFlagString() << std::endl;
  }

}

uint16_t GetDataFromRegister(const register_access& Reg) {
  uint16_t reg = registers[Reg.Index];
  if (Reg.Count == 2) {
    return reg;
  }
  if (Reg.Offset == 0) {
    return (reg & 0xff);
  }
  if (Reg.Offset == 1) {
    return (reg >> 8);
  }
  return 0;
}

void StoreDataInRegister(const register_access& Reg, uint16_t Data_16) {
  uint16_t* reg = &registers[Reg.Index];

  std::cout << register_string[Reg.Index] << ": "; 
  std::cout << std::hex << *reg << " -> ";
  std::cout << std::hex << Data_16 << std::endl;

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

void SetFlag(uint16_t Flag_Bit) {
  if (registers[Flag_Reg] & (1 << Flag_Bit)) {
    return;
  }

  std::string before = GetFlagString();
  registers[Flag_Reg] |= (1 << Flag_Bit);
  std::string after = GetFlagString();

  std::cout << "Flag: " << before << " -> " << after << std::endl;
}

void ResetFlag(uint16_t Flag_Bit) {
  if (!(registers[Flag_Reg] & (1 << Flag_Bit))) {
    return;
  }

  std::string before = GetFlagString();
  registers[Flag_Reg] &= ~(1 << Flag_Bit);
  std::string after = GetFlagString();

  std::cout << "Flag: " << before << " -> " << after << std::endl;
}

void UpdateInstructionPointer(const instruction& Instruction, int16_t jump) {
  uint16_t before = registers[IP_Reg];
  registers[IP_Reg] += Instruction.Size + jump;
  uint16_t after = registers[IP_Reg];

  std::cout << "IP: " << std::hex << before << " -> " << std::hex << after << std::endl;
}

void ProcessInstruction(const instruction& Instruction) {
  instruction_operand destination = Instruction.Operands[0];
  instruction_operand source      = Instruction.Operands[1];

  uint16_t data_src = 0;
  if (source.Type == Operand_Immediate) {
    data_src = static_cast<uint16_t>(source.Immediate.Value);  // Casey's lib uses s32
  } else if (source.Type == Operand_Register) {
    data_src = GetDataFromRegister(source.Register);
  }

  if (Instruction.Op == Op_mov) {
    StoreDataInRegister(destination.Register, data_src);
  }
  if (Instruction.Op == Op_add) {
    uint16_t data_dest = GetDataFromRegister(destination.Register);
    StoreDataInRegister(destination.Register, data_dest + data_src);
  }
  if (Instruction.Op == Op_sub || Instruction.Op == Op_cmp) {
    uint16_t data_dest = GetDataFromRegister(destination.Register);
    uint16_t result = data_dest - data_src;
    if (result == 0) {
      SetFlag(ZF_Bit);
    } else {
      ResetFlag(ZF_Bit);
    }
    if (static_cast<int16_t>(result) < 0) {
      SetFlag(SF_Bit);
    } else {
      ResetFlag(SF_Bit);
    }
    if (Instruction.Op == Op_sub) {
      StoreDataInRegister(destination.Register, result);
    }
  }

  int16_t jump = 0;
  bool zero_flag = (registers[Flag_Reg] & (1 << ZF_Bit));
  if (Instruction.Op == Op_jne && !zero_flag) {
    jump = static_cast<int16_t>(Instruction.Operands[0].Immediate.Value);
  }

  UpdateInstructionPointer(Instruction, jump);
}

void ProcessAllInstructions(const std::unordered_map<uint32_t, instruction>& mp, uint32_t bytesRead) {
  while (registers[IP_Reg] < bytesRead) {
    ProcessInstruction(mp.at(registers[IP_Reg]));
  }
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

  std::unordered_map<uint32_t, instruction> mp;

  uint32_t bytesToRead = bytesRead;
  uint32_t address = 0;
  while (bytesToRead) {
    instruction Instruction;
    Sim86_Decode8086Instruction(bytesToRead, source, &Instruction);
    if (Instruction.Op == Op_None) {
      std::cerr << "Unable to decode instruction" << std::endl;
      return 1;
    }
    mp[address] = Instruction;
    bytesToRead -= Instruction.Size;
    source += Instruction.Size;
    address += Instruction.Size;
  }

  ProcessAllInstructions(mp, bytesRead);
  PrintAllRegisters();

  return 0;
}
