#include "../nes.h"
//#define CPUDEBUG
#ifdef _DEBUG
#define PC_BREAKPOINT(x) if (PC==x) { DebugBreak(); }
#define CYCLE_BREAKPOINT(x) if (total_cycles==x) { DebugBreak(); }
#endif
#if defined(_DEBUG) && defined(CPUDEBUG)
#include "../debug/cpu_debugger.h"
CpuDebugger debugger;
#endif

AddressingModes Cpu::addressing_modes[256] = {
 kImplied  ,kIndirectX,kUnused,   kUnused,kUnused   ,kZeroPage,kZeroPage,kUnused,kImplied,kImmediate,kAccumulator,kUnused,kUnused,kAbsolute,kAbsolute,kUnused,
 kRelative ,kIndirectY,kUnused,   kUnused,kUnused   ,kZeroPageX,kZeroPageX,kUnused,kImplied,kAbsoluteY,kUnused,kUnused,kUnused,kAbsoluteX,kAbsoluteX,kUnused,
 kAbsolute ,kIndirectX,kUnused,   kUnused,kZeroPage ,kZeroPage,kZeroPage,kUnused,kImplied,kImmediate,kAccumulator,kUnused,kAbsolute,kAbsolute,kAbsolute,kUnused,
 kRelative ,kIndirectY,kUnused,   kUnused,kUnused   ,kZeroPageX,kZeroPageX,kUnused,kImplied,kAbsoluteY,kUnused,kUnused,kUnused,kAbsoluteX,kAbsoluteX,kUnused,
 kImplied  ,kIndirectX,kUnused,   kUnused,kUnused   ,kZeroPage,kZeroPage,kUnused,kImplied,kImmediate,kAccumulator,kUnused,kAbsolute,kAbsolute,kAbsolute,kUnused,
 kRelative ,kIndirectY,kUnused,   kUnused,kUnused   ,kZeroPageX,kZeroPageX,kUnused,kImplied,kAbsoluteY,kUnused,kUnused,kUnused,kAbsoluteX,kAbsoluteX,kUnused,
 kImplied  ,kIndirectX,kUnused,   kUnused,kUnused   ,kZeroPage,kZeroPage,kUnused,kImplied,kImmediate,kAccumulator,kUnused,kIndirect,kAbsolute,kAbsolute,kUnused,
 kRelative ,kIndirectY,kUnused,   kUnused,kUnused   ,kZeroPageX,kZeroPageX,kUnused,kImplied,kAbsoluteY,kUnused,kUnused,kUnused,kAbsoluteX,kAbsoluteX,kUnused,
 kUnused   ,kIndirectX,kUnused,   kUnused,kZeroPage ,kZeroPage,kZeroPage,kUnused,kImplied,kUnused,kImplied,kUnused,kAbsolute,kAbsolute,kAbsolute,kUnused,
 kRelative ,kIndirectY,kUnused,   kUnused,kZeroPageX,kZeroPageX,kZeroPageX,kUnused,kImplied,kAbsoluteY,kImplied,kUnused,kUnused,kAbsoluteX,kUnused,kUnused,
 kImmediate,kIndirectX,kImmediate,kUnused,kZeroPage ,kZeroPage,kZeroPage,kUnused,kImplied,kImmediate,kImplied,kUnused,kAbsolute,kAbsolute,kAbsolute,kUnused,
 kRelative ,kIndirectY,kUnused,   kUnused,kZeroPageX,kZeroPageX,kZeroPageY,kUnused,kImplied,kAbsoluteY,kImplied,kUnused,kAbsoluteX,kAbsoluteX,kAbsoluteY,kUnused,
 kImmediate,kIndirectX,kUnused,   kUnused,kZeroPage ,kZeroPage,kZeroPage,kUnused,kImplied,kImmediate,kImplied,kUnused,kAbsolute,kAbsolute,kAbsolute,kUnused,
 kRelative ,kIndirectY,kUnused,   kUnused,kUnused   ,kZeroPageX,kZeroPageX,kUnused,kImplied,kAbsoluteY,kUnused,kUnused,kUnused,kAbsoluteX,kAbsoluteX,kUnused,
 kImmediate,kIndirectX,kUnused,   kUnused,kZeroPage ,kZeroPage,kZeroPage,kUnused,kImplied,kImmediate,kImplied,kUnused,kAbsolute,kAbsolute,kAbsolute,kUnused,
 kRelative ,kIndirectY,kUnused,   kUnused,kUnused   ,kZeroPageX,kZeroPageX,kUnused,kImplied,kAbsoluteY,kUnused,kUnused,kUnused,kAbsoluteX,kAbsoluteX,kUnused,
 };

Cpu::Cpu() :  Component(), reset(false) {
  instructions[0x000] = &Cpu::BRK;     instructions[0x001] = &Cpu::ORA_INX;    instructions[0x002] = &Cpu::_002;    instructions[0x003] = &Cpu::_003; instructions[0x004] = &Cpu::_004;    instructions[0x005] = &Cpu::ORA_ZPG;    instructions[0x006] = &Cpu::ASL1;    instructions[0x007] = &Cpu::_007; instructions[0x008] = &Cpu::PHP;     instructions[0x009] = &Cpu::ORA_IMM;    instructions[0x00A] = &Cpu::ASL2;    instructions[0x00B] = &Cpu::_00B; instructions[0x00C] = &Cpu::_00C;       instructions[0x00D] = &Cpu::ORA_ABS;    instructions[0x00E] = &Cpu::ASL3;    instructions[0x00F] = &Cpu::_00F;
  instructions[0x010] = &Cpu::BPL;     instructions[0x011] = &Cpu::ORA_INY;    instructions[0x012] = &Cpu::_012;    instructions[0x013] = &Cpu::_013; instructions[0x014] = &Cpu::_014;    instructions[0x015] = &Cpu::ORA_ZPX;    instructions[0x016] = &Cpu::ASL4;    instructions[0x017] = &Cpu::_017; instructions[0x018] = &Cpu::CLC;     instructions[0x019] = &Cpu::ORA_ABY;    instructions[0x01A] = &Cpu::_01A;    instructions[0x01B] = &Cpu::_01B; instructions[0x01C] = &Cpu::_01C;       instructions[0x01D] = &Cpu::ORA_ABX;    instructions[0x01E] = &Cpu::ASL5;    instructions[0x01F] = &Cpu::_01F;
  instructions[0x020] = &Cpu::JSR_ABS; instructions[0x021] = &Cpu::AND_INX;    instructions[0x022] = &Cpu::_022;    instructions[0x023] = &Cpu::_023; instructions[0x024] = &Cpu::BIT_ZPG; instructions[0x025] = &Cpu::AND_ZPG; instructions[0x026] = &Cpu::ROL_ZPG; instructions[0x027] = &Cpu::_027; instructions[0x028] = &Cpu::PLP; instructions[0x029] = &Cpu::AND_IMM; instructions[0x02A] = &Cpu::ROL_ACC; instructions[0x02B] = &Cpu::_02B; instructions[0x02C] = &Cpu::BIT_ABS;    instructions[0x02D] = &Cpu::AND_ABS; instructions[0x02E] = &Cpu::ROL_ABS; instructions[0x02F] = &Cpu::_02F;
  instructions[0x030] = &Cpu::BMI_REL; instructions[0x031] = &Cpu::AND_INY;    instructions[0x032] = &Cpu::_032;    instructions[0x033] = &Cpu::_033; instructions[0x034] = &Cpu::_034;    instructions[0x035] = &Cpu::AND_ZPX; instructions[0x036] = &Cpu::ROL_ZPX; instructions[0x037] = &Cpu::_037; instructions[0x038] = &Cpu::SEC; instructions[0x039] = &Cpu::AND_ABY; instructions[0x03A] = &Cpu::_03A;    instructions[0x03B] = &Cpu::_03B; instructions[0x03C] = &Cpu::_03C;       instructions[0x03D] = &Cpu::AND_ABX; instructions[0x03E] = &Cpu::ROL_ABX; instructions[0x03F] = &Cpu::_03F;
  instructions[0x040] = &Cpu::RTI_IMP; instructions[0x041] = &Cpu::EOR_INX;    instructions[0x042] = &Cpu::_042;    instructions[0x043] = &Cpu::_043; instructions[0x044] = &Cpu::_044;    instructions[0x045] = &Cpu::EOR_ZPG; instructions[0x046] = &Cpu::LSR_ZPG; instructions[0x047] = &Cpu::_047; instructions[0x048] = &Cpu::PHA; instructions[0x049] = &Cpu::EOR_IMM; instructions[0x04A] = &Cpu::LSR_ACC; instructions[0x04B] = &Cpu::_04B; instructions[0x04C] = &Cpu::JMP_ABS;    instructions[0x04D] = &Cpu::EOR_ABS; instructions[0x04E] = &Cpu::LSR_ABS; instructions[0x04F] = &Cpu::_04F;
  instructions[0x050] = &Cpu::BVC_REL; instructions[0x051] = &Cpu::EOR_INY;    instructions[0x052] = &Cpu::_052;    instructions[0x053] = &Cpu::_053; instructions[0x054] = &Cpu::_054;    instructions[0x055] = &Cpu::EOR_ZPX; instructions[0x056] = &Cpu::LSR_ZPX; instructions[0x057] = &Cpu::_057; instructions[0x058] = &Cpu::CLI; instructions[0x059] = &Cpu::EOR_ABY; instructions[0x05A] = &Cpu::_05A;    instructions[0x05B] = &Cpu::_05B; instructions[0x05C] = &Cpu::_05C;       instructions[0x05D] = &Cpu::EOR_ABX; instructions[0x05E] = &Cpu::LSR_ABX; instructions[0x05F] = &Cpu::_05F;
  instructions[0x060] = &Cpu::RTS_IMP; instructions[0x061] = &Cpu::ADC_INX;    instructions[0x062] = &Cpu::_062;    instructions[0x063] = &Cpu::_063; instructions[0x064] = &Cpu::_064;    instructions[0x065] = &Cpu::ADC_ZPG; instructions[0x066] = &Cpu::ROR_ZPG; instructions[0x067] = &Cpu::_067; instructions[0x068] = &Cpu::PLA_IMP; instructions[0x069] = &Cpu::ADC_IMM; instructions[0x06A] = &Cpu::ROR_ACC; instructions[0x06B] = &Cpu::_06B; instructions[0x06C] = &Cpu::JMP_IND;    instructions[0x06D] = &Cpu::ADC_ABS; instructions[0x06E] = &Cpu::ROR_ABS; instructions[0x06F] = &Cpu::_06F;
  instructions[0x070] = &Cpu::BVS_REL; instructions[0x071] = &Cpu::ADC_INY;    instructions[0x072] = &Cpu::_072;    instructions[0x073] = &Cpu::_073; instructions[0x074] = &Cpu::_074;    instructions[0x075] = &Cpu::ADC_ZPX; instructions[0x076] = &Cpu::ROR_ZPX; instructions[0x077] = &Cpu::_077; instructions[0x078] = &Cpu::SEI; instructions[0x079] = &Cpu::ADC_ABY; instructions[0x07A] = &Cpu::_07A;    instructions[0x07B] = &Cpu::_07B; instructions[0x07C] = &Cpu::_07C;       instructions[0x07D] = &Cpu::ADC_ABX; instructions[0x07E] = &Cpu::ROR_ABX; instructions[0x07F] = &Cpu::_07F;
  instructions[0x080] = &Cpu::_080;    instructions[0x081] = &Cpu::STA_INX;    instructions[0x082] = &Cpu::_082;    instructions[0x083] = &Cpu::_083; instructions[0x084] = &Cpu::STY_ZPG; instructions[0x085] = &Cpu::STA_ZPG; instructions[0x086] = &Cpu::STX_ZPG; instructions[0x087] = &Cpu::_087; instructions[0x088] = &Cpu::DEY_IMP; instructions[0x089] = &Cpu::_089;    instructions[0x08A] = &Cpu::TXA_IMP; instructions[0x08B] = &Cpu::_08B; instructions[0x08C] = &Cpu::STY_ABS;    instructions[0x08D] = &Cpu::STA_ABS; instructions[0x08E] = &Cpu::STX_ABS; instructions[0x08F] = &Cpu::_08F;
  instructions[0x090] = &Cpu::BCC_REL; instructions[0x091] = &Cpu::STA_INY;    instructions[0x092] = &Cpu::_092;    instructions[0x093] = &Cpu::_093; instructions[0x094] = &Cpu::STY_ZPX; instructions[0x095] = &Cpu::STA_ZPX; instructions[0x096] = &Cpu::STX_ZPX; instructions[0x097] = &Cpu::_097; instructions[0x098] = &Cpu::TYA_IMP; instructions[0x099] = &Cpu::STA_ABY; instructions[0x09A] = &Cpu::TXS_IMP; instructions[0x09B] = &Cpu::_09B; instructions[0x09C] = &Cpu::_09C;       instructions[0x09D] = &Cpu::STA_ABX; instructions[0x09E] = &Cpu::_09E;    instructions[0x09F] = &Cpu::_09F;
  instructions[0x0A0] = &Cpu::LDY_IMM; instructions[0x0A1] = &Cpu::LDA_INX;    instructions[0x0A2] = &Cpu::LDX_IMM; instructions[0x0A3] = &Cpu::_0A3; instructions[0x0A4] = &Cpu::LDY_ZPG; instructions[0x0A5] = &Cpu::LDA_ZPG; instructions[0x0A6] = &Cpu::LDX_ZPG; instructions[0x0A7] = &Cpu::_0A7; instructions[0x0A8] = &Cpu::TAY_IMP; instructions[0x0A9] = &Cpu::LDA_IMM; instructions[0x0AA] = &Cpu::TAX_IMP; instructions[0x0AB] = &Cpu::_0AB; instructions[0x0AC] = &Cpu::LDY_ABS;    instructions[0x0AD] = &Cpu::LDA_ABS; instructions[0x0AE] = &Cpu::LDX_ABS; instructions[0x0AF] = &Cpu::_0AF;
  instructions[0x0B0] = &Cpu::BCS_REL; instructions[0x0B1] = &Cpu::LDA_INY;    instructions[0x0B2] = &Cpu::_0B2;    instructions[0x0B3] = &Cpu::_0B3; instructions[0x0B4] = &Cpu::LDY_ZPX; instructions[0x0B5] = &Cpu::LDA_ZPX; instructions[0x0B6] = &Cpu::LDX_ZPY; instructions[0x0B7] = &Cpu::_0B7; instructions[0x0B8] = &Cpu::CLV_IMP; instructions[0x0B9] = &Cpu::LDA_ABY; instructions[0x0BA] = &Cpu::TSX_IMP; instructions[0x0BB] = &Cpu::_0BB; instructions[0x0BC] = &Cpu::LDY_ABX;    instructions[0x0BD] = &Cpu::LDA_ABX; instructions[0x0BE] = &Cpu::LDX_ABY; instructions[0x0BF] = &Cpu::_0BF;
  instructions[0x0C0] = &Cpu::CPY_IMM; instructions[0x0C1] = &Cpu::CMP_INX;    instructions[0x0C2] = &Cpu::_0C2;    instructions[0x0C3] = &Cpu::_0C3; instructions[0x0C4] = &Cpu::CPY_ZPG; instructions[0x0C5] = &Cpu::CMP_ZPG; instructions[0x0C6] = &Cpu::DEC_ZPG; instructions[0x0C7] = &Cpu::_0C7; instructions[0x0C8] = &Cpu::INY_IMP; instructions[0x0C9] = &Cpu::CMP_IMM; instructions[0x0CA] = &Cpu::DEX_IMP; instructions[0x0CB] = &Cpu::_0CB; instructions[0x0CC] = &Cpu::CPY_ABS;    instructions[0x0CD] = &Cpu::CMP_ABS; instructions[0x0CE] = &Cpu::DEC_ABS; instructions[0x0CF] = &Cpu::_0CF;
  instructions[0x0D0] = &Cpu::BNE_REL; instructions[0x0D1] = &Cpu::CMP_INY;    instructions[0x0D2] = &Cpu::_0D2;    instructions[0x0D3] = &Cpu::_0D3; instructions[0x0D4] = &Cpu::_0D4;    instructions[0x0D5] = &Cpu::CMP_ZPX; instructions[0x0D6] = &Cpu::DEC_ZPX; instructions[0x0D7] = &Cpu::_0D7; instructions[0x0D8] = &Cpu::CLD_IMP; instructions[0x0D9] = &Cpu::CMP_ABY; instructions[0x0DA] = &Cpu::_0DA;    instructions[0x0DB] = &Cpu::_0DB; instructions[0x0DC] = &Cpu::_0DC;       instructions[0x0DD] = &Cpu::CMP_ABX; instructions[0x0DE] = &Cpu::DEC_ABX; instructions[0x0DF] = &Cpu::_0DF;
  instructions[0x0E0] = &Cpu::CPX_IMM; instructions[0x0E1] = &Cpu::SBC_INX;    instructions[0x0E2] = &Cpu::_0E2;    instructions[0x0E3] = &Cpu::_0E3; instructions[0x0E4] = &Cpu::CPX_ZPG; instructions[0x0E5] = &Cpu::SBC_ZPG; instructions[0x0E6] = &Cpu::INC_ZPG; instructions[0x0E7] = &Cpu::_0E7; instructions[0x0E8] = &Cpu::INX_IMP; instructions[0x0E9] = &Cpu::SBC_IMM; instructions[0x0EA] = &Cpu::NOP_IMP; instructions[0x0EB] = &Cpu::_0EB; instructions[0x0EC] = &Cpu::CPX_ABS;    instructions[0x0ED] = &Cpu::SBC_ABS; instructions[0x0EE] = &Cpu::INC_ABS; instructions[0x0EF] = &Cpu::_0EF;
  instructions[0x0F0] = &Cpu::BEQ_REL; instructions[0x0F1] = &Cpu::SBC_INY;    instructions[0x0F2] = &Cpu::_0F2;    instructions[0x0F3] = &Cpu::_0F3; instructions[0x0F4] = &Cpu::_0F4;    instructions[0x0F5] = &Cpu::SBC_ZPX; instructions[0x0F6] = &Cpu::INC_ZPX; instructions[0x0F7] = &Cpu::_0F7; instructions[0x0F8] = &Cpu::SED_IMP; instructions[0x0F9] = &Cpu::SBC_ABY; instructions[0x0FA] = &Cpu::_0FA;    instructions[0x0FB] = &Cpu::_0FB; instructions[0x0FC] = &Cpu::_0FC;       instructions[0x0FD] = &Cpu::SBC_ABX; instructions[0x0FE] = &Cpu::INC_ABX; instructions[0x0FF] = &Cpu::_0FF;

  fetch_address[0x00] = &Cpu::FetchAddressMode0;
}

int Cpu::Initialize(Nes* nes) {
  if (init_ == false) {
    Component::Initialize(nes);
    RAM = new uint8_t[0x800];


    init_ = true;
    return S_OK;
  }
  return S_FALSE;
}

int Cpu::Deinitialize() {
  if (init_ == false) {
    SafeDeleteArray(&RAM);
    init_ = false;
    return S_OK;
  }
  return S_FALSE;
}

void Cpu::Power() {
  nmi=false;
  nmi_edge_detected=false;
  enable_irq = false;
  //power up
  op = 0;
  cycles = 0;
  current_inst_cycles = 0;
  last_cpu_cycles_ = 0;
  frequency_mhz_ = 0;
  cycles_per_second = 0;
  reset = true;
  address_bus = 0;


  PC=0xC000;//MemReadAccess(0xFFFC);
  A=0;
  X=0;
  Y=0;
  S=0;
  P.raw = 0x34;
  irq_line = false;
  // Pre-initialize RAM the same way as FCEUX does, to improve TAS sync.
  for(unsigned a=0; a<0x800; ++a)
      RAM[a] = (a&4) ? 0xFF : 0x00;
}


void Cpu::Reset() {
  if (reset == false) {
    cycles = 0;
    reset=true;
    irq_line = false;
  }
}


__forceinline void Cpu::tick() {

  nes_->ppu().Tick();
  nes_->apu().Tick();
  nes_->gamepak().mapper->CpuTick();
  enable_irq = irq_line & !P.I;
 
  ++cycles;
  ++current_inst_cycles;
}
    
void Cpu::Op() {

  current_inst_cycles = 0;
  prev_op = op;

  op = MemReadAccess(PC++);

  //PC_BREAKPOINT(0xe088);
  #if defined(_DEBUG) && defined(CPUDEBUG)
    debugger.HookBeforeCpuStep(this);
  #endif
  (this->*(instructions[op]))();
  #if defined(_DEBUG) && defined(CPUDEBUG)
    debugger.HookAfterCpuStep(this);
    //OutputDebugString(debugger.debug_line);
    nes_->log.Channel("cpu").Log(debugger.debug_line);
  #endif

  bool nmi_now = nmi;
  if (reset) { 
    RESET();
    reset = false;
  } else if(nmi_now && !nmi_edge_detected) { 
    nmi_edge_detected = true;
    NMI();
  } else if (enable_irq==true) {
    //enable_irq = false;
    IRQ();
  }


  if(!nmi_now) 
    nmi_edge_detected=false;

}

uint8_t Cpu::MemReadAccess(uint16_t address) {
  tick();
  if(address >= 0 && address < 0x1FFF) { 
    uint8_t& r = RAM[address & 0x7FF]; return r; 
  } else if(address >= 0x2000 && address <= 0x3FFF) {
    return nes_->ppu().Read(address);
  } else if(address >= 0x4000 && address <= 0x40FF) {
    switch(address) {
        case 0x4015: return nes_->apu().Read();  
        case 0x4016: return IO::JoyRead2(); 
        //case 0x17: return 0;//IO::JoyRead2(); // write:passthru
        //default:   nes_->ppu().open_bus(); 
          
    }
    return 0x40;
  } else if (address >= 0x6000 && address <= 0xFFFF) {
    return nes_->gamepak().Read(address);
  }

  return 0;
}

void Cpu::MemWriteAccess(uint16_t address, uint8_t value)  {
    // Memory writes are turned into reads while reset is being signalled
    if (reset && address!=0x4017) {
      //khalid  -on for cpu reset compatibility
      //- turned off for compatibility with apu test
      MemReadAccess(address);
      return;
    }

    tick();
    // Map the memory from CPU's viewpoint.
    if (address >= 0 && address < 0x2000) { 
      RAM[address & 0x7FF] = value;
    } else if(address >= 0x2000 && address < 0x4000) { 
      nes_->ppu().Write(address,value); 
    } else if(address >= 0x4000 && address < 0x4020) {
      //APU
      if ((address >= 0x4000 && address < 0x4014) ||
          (address == 0x4015) ||
          (address == 0x4017)) {
          nes_->apu().Write(address, value);
      }

      //DMA // OAM DMA: Copy 256 bytes from RAM into PPU's sprite memory
      if (address == 0x4014) {
        for(unsigned b=0; b<256; ++b) {
          MemWriteAccess(0x2004, MemReadAccess(((value&7)<<8)+b));
        }
      }

      if (address == 0x4016) {
        IO::JoyWrite2(value);
      }
    } else if (address >= 0x6000 && address <= 0xFFFF) {
      nes_->gamepak().Write(address, value);
    }

}

void Cpu::BRK() {
  MemReadAccess(PC++);
  decltype(P) t;
  t.raw = P.raw|0x30; 
  PushPC();
  Push(t.raw);
  P.I = 1;
  if (nmi == true && !nmi_edge_detected)
  {
    nmi_edge_detected = true;
    PC = FetchAddressMode0(0xFFFA);
  }
  else
  {
    PC = FetchAddressMode0(0xFFFE);
  }
}

void Cpu::ORA_INX() {
  address_bus = FetchAddressIndirectX(PC);
  A |= MemReadAccess(address_bus);
  P.N = A & 0x80;
  P.Z = uint8_t(A) == 0;
}

void Cpu::_002() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= A;
  t &= MemReadAccess(PC++);
  P.C = t & 0x80;
  t = (t << 1) | (sb * 0x01);
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_003() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); 
  d=0; 
  tick();
  address_bus =MemReadAccess(c=address_bus); 
  address_bus += 256*MemReadAccess(wrap(c,c+1));


  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  P.C = t & 0x80;
  auto orig_val = t;
  t = (t << 1) | (sb * 0x01);
  MemWriteAccess(address_bus+d, orig_val);
  MemWriteAccess(address_bus+d, t);
  t = c | t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_004() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  tick();
}

void Cpu::ORA_ZPG() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  t = c | t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::ASL1() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= MemReadAccess(address_bus+d);
  P.C = t & 0x80;
  t = (t << 1) | (sb * 0x01);
  MemWriteAccess(address_bus+d, t);
  tick();
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_007() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  P.C = t & 0x80;
  t = (t << 1) | (sb * 0x01);
  MemWriteAccess(address_bus+d, t);
  tick();
  t = c | t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::PHP() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= P.raw|pbits; c = t;
  tick();
  Push(t);
}

void Cpu::ORA_IMM() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(PC++);
  t = c | t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::ASL2() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= A;
  P.C = t & 0x80;
  t = (t << 1) | (sb * 0x01);
  tick();
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_00B() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= A;
  t &= MemReadAccess(PC++);
  P.C = t & 0x80;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_00C() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  tick();
}

void Cpu::ORA_ABS() {
  address_bus = FetchAddressAbsolute(PC);
  A |= MemReadAccess(address_bus);
  P.N = A & 0x80;
  P.Z = uint8_t(A) == 0;
}

void Cpu::ASL3() {
  unsigned  d=0, pbits =  0x30;
  address_bus = MemReadAccess(PC++);
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  auto orig = MemReadAccess(address_bus+d);
  P.C = orig & 0x80;
  auto t = (orig << 1);
  MemWriteAccess(address_bus+d, orig);
  MemWriteAccess(address_bus+d, t);
  //tick();
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_00F() {
  unsigned  d=0;
  address_bus = MemReadAccess(PC++);
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  auto c = A; 
  auto orig = MemReadAccess(address_bus+d);
  P.C = orig & 0x80;
  auto t = (orig << 1);
  MemWriteAccess(address_bus+d, orig);
  MemWriteAccess(address_bus+d, t);
  t = c | t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::BPL() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= P.raw|pbits; c = t;
  t = 1;
  t <<= 1;
  t <<= 2;
  t <<= 4;
  t = c & t;
  if(!t) { tick(); Misfire(PC, address_bus = int8_t(address_bus) + PC); PC=address_bus; };
}

void Cpu::ORA_INY() {
  address_bus = FetchAddressIndirectY(PC,true);
  A |= MemReadAccess(address_bus);
  P.N = A & 0x80;
  P.Z = uint8_t(A) == 0;
}

void Cpu::_012() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =MemReadAccess(c=address_bus); address_bus += 256*MemReadAccess(wrap(c,c+1));
  MemReadAccess(wrap(address_bus, address_bus+d));
  t &= MemReadAccess(address_bus+d);
  P.C = t & 0x80;
  t = (t << 1) | (sb * 0x01);
  MemWriteAccess(address_bus+d, t);
  tick();
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_013() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =MemReadAccess(c=address_bus); address_bus += 256*MemReadAccess(wrap(c,c+1));
  MemReadAccess(wrap(address_bus, address_bus+d));
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  P.C = t & 0x80;
  auto orig_val = t;
  t = (t << 1) | (sb * 0x01);
  MemWriteAccess(address_bus+d, orig_val);
  MemWriteAccess(address_bus+d, t);
  //tick();
  t = c | t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_014() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  tick();
}

void Cpu::ORA_ZPX() {
  address_bus = FetchAddressZeroPageX(PC);
  A |= MemReadAccess(address_bus);
  P.N = A & 0x80;
  P.Z = uint8_t(A) == 0;
}

void Cpu::ASL4() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  t &= MemReadAccess(address_bus+d);
  P.C = t & 0x80;
  t = (t << 1) | (sb * 0x01);
  MemWriteAccess(address_bus+d, t);
  tick();
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_017() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  P.C = t & 0x80;
  t = (t << 1) | (sb * 0x01);
  MemWriteAccess(address_bus+d, t);
  tick();
  t = c | t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::CLC() {
  tick();
  P.C = 0;
}

void Cpu::ORA_ABY() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  Misfire(address_bus, address_bus+d);
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  t = c | t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_01A() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  tick();
}

void Cpu::_01B() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  MemReadAccess(wrap(address_bus, address_bus+d));
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  P.C = t & 0x80;
  auto orig_val = t;
  t = (t << 1) | (sb * 0x01);
  MemWriteAccess(address_bus+d, orig_val);
  MemWriteAccess(address_bus+d, t);
  //tick();
  t = c | t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_01C() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  Misfire(address_bus, address_bus+d);
  tick();
}

void Cpu::ORA_ABX() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  Misfire(address_bus, address_bus+d);
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  t = c | t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::ASL5() {
  //unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  /*address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus); 
  address_bus += 256*MemReadAccess(PC++);
  MemReadAccess(wrap(address_bus, address_bus+d));
  */
  address_bus = FetchAddressAbsoluteX(PC);
  auto orig_val = MemReadAccess(address_bus);
  P.C = orig_val & 0x80;
  auto mod_val = (orig_val << 1);
  MemWriteAccess(address_bus, orig_val);
  MemWriteAccess(address_bus, mod_val);
  //tick();
  P.N = mod_val & 0x80;
  P.Z = uint8_t(mod_val) == 0;
}

void Cpu::_01F() {
  /*unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus); 
  address_bus += 256*MemReadAccess(PC++);
  MemReadAccess(wrap(address_bus, address_bus+d));
  */
  address_bus = FetchAddressAbsoluteX(PC);
  unsigned c = A; 
  auto orig_val = MemReadAccess(address_bus);
  P.C = orig_val & 0x80;
  auto mod_val = (orig_val << 1);
  MemWriteAccess(address_bus, orig_val);
  MemWriteAccess(address_bus, mod_val);
  //tick();
  mod_val = c | mod_val;
  A = mod_val;
  P.N = mod_val & 0x80;
  P.Z = uint8_t(mod_val) == 0;
}

void Cpu::JSR_ABS() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  address_bus =uint8_t(address_bus); 
  address_bus += 256*MemReadAccess(PC++);
  tick();
  d=PC-1; Push(d>>8); Push(d);
  PC = address_bus;
}

void Cpu::AND_INX() {
  address_bus = FetchAddressIndirectX(PC);
  A = A & MemReadAccess(address_bus);
  P.N = A & 0x80;
  P.Z = uint8_t(A) == 0;
}

void Cpu::_022() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= A;
  t &= MemReadAccess(PC++);
  sb = P.C;
  P.C = t & 0x80;
  t = (t << 1) | (sb * 0x01);
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_023() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  address_bus =MemReadAccess(c=address_bus); address_bus += 256*MemReadAccess(wrap(c,c+1));
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  sb = P.C;
  P.C = t & 0x80;
  auto orig_val = t;
  t = (t << 1) | (sb * 0x01);
  MemWriteAccess(address_bus+d, orig_val);
  MemWriteAccess(address_bus+d, t);
  //tick();
  t = c & t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::BIT_ZPG() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  P.V = t & 0x40; P.N = t & 0x80;
  t = c & t;
  P.Z = uint8_t(t) == 0;
}

void Cpu::AND_ZPG() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  t = c & t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::ROL_ZPG() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= MemReadAccess(address_bus+d);
  sb = P.C;
  P.C = t & 0x80;
  t = (t << 1) | (sb * 0x01);
  MemWriteAccess(address_bus+d, t);
  tick();
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_027() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  sb = P.C;
  P.C = t & 0x80;
  t = (t << 1) | (sb * 0x01);
  MemWriteAccess(address_bus+d, t);
  tick();
  t = c & t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::PLP() {
  MemReadAccess(PC);
  FakeIncrementS();
  auto pop_val = Pop();
  P.raw = pop_val & ~0x30;
}

void Cpu::AND_IMM() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(PC++);
  t = c & t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::ROL_ACC() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= A;
  sb = P.C;
  P.C = t & 0x80;
  t = (t << 1) | (sb * 0x01);
  tick();
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_02B() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= A;
  t &= MemReadAccess(PC++);
  P.C = t & 0x80;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::BIT_ABS() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  P.V = t & 0x40; P.N = t & 0x80;
  t = c & t;
  P.Z = uint8_t(t) == 0;
}

void Cpu::AND_ABS() {
  unsigned  t=0xFF, c=0, pbits =  0x30;//0x20 for op > 0x100
  
  address_bus = FetchAddressAbsolute(PC);

  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus);
  t = c & t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::ROL_ABS() {
  //unsigned  t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = FetchAddressAbsolute(PC);

  auto orig_val = MemReadAccess(address_bus);

  unsigned sb = P.C;
  P.C = orig_val & 0x80;
  auto mod_val = (orig_val << 1) | (sb * 0x01);
  MemWriteAccess(address_bus, orig_val);
  MemWriteAccess(address_bus, mod_val);
  //tick();
  P.N = mod_val & 0x80;
  P.Z = uint8_t(mod_val) == 0;
}

void Cpu::_02F() {
  unsigned  d=0;//, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  unsigned c = A; 
  auto orig_val = MemReadAccess(address_bus+d);
  unsigned sb = P.C;
  P.C = orig_val & 0x80;
  auto mod_val = (orig_val << 1) | (sb * 0x01);
  MemWriteAccess(address_bus+d, orig_val);
  MemWriteAccess(address_bus+d, mod_val);
  //tick();
  mod_val = c & mod_val;
  A = mod_val;
  P.N = mod_val & 0x80;
  P.Z = uint8_t(mod_val) == 0;
}

void Cpu::BMI_REL() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= P.raw|pbits; c = t;
  t = 1;
  t <<= 1;
  t <<= 2;
  t <<= 4;
  t = c & t;
  if(t) { tick(); Misfire(PC, address_bus = int8_t(address_bus) + PC); PC=address_bus; };
}

void Cpu::AND_INY() {
  address_bus = FetchAddressIndirectY(PC,true);
  A &= MemReadAccess(address_bus);
  P.N = A & 0x80;
  P.Z = uint8_t(A) == 0;
}

void Cpu::_032() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =MemReadAccess(c=address_bus); address_bus += 256*MemReadAccess(wrap(c,c+1));
  MemReadAccess(wrap(address_bus, address_bus+d));
  t &= MemReadAccess(address_bus+d);
  sb = P.C;
  P.C = t & 0x80;
  t = (t << 1) | (sb * 0x01);
  MemWriteAccess(address_bus+d, t);
  tick();
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_033() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =MemReadAccess(c=address_bus); address_bus += 256*MemReadAccess(wrap(c,c+1));
  MemReadAccess(wrap(address_bus, address_bus+d));
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  sb = P.C;
  P.C = t & 0x80;
  auto orig_val = t;
  t = (t << 1) | (sb * 0x01);
  MemWriteAccess(address_bus+d, orig_val);
  MemWriteAccess(address_bus+d, t);
  //tick();
  t = c & t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_034() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  tick();
}

void Cpu::AND_ZPX() {
  address_bus = FetchAddressZeroPageX(PC);
  A &= MemReadAccess(address_bus);
  P.N = A & 0x80;
  P.Z = uint8_t(A) == 0;
}

void Cpu::ROL_ZPX() {
  address_bus = FetchAddressZeroPageX(PC);
  unsigned t = MemReadAccess(address_bus);
  unsigned sb = P.C;
  P.C = t & 0x80;
  t = (t << 1) | (sb * 0x01);
  MemWriteAccess(address_bus, t);
  tick();
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_037() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  sb = P.C;
  P.C = t & 0x80;
  t = (t << 1) | (sb * 0x01);
  MemWriteAccess(address_bus+d, t);
  tick();
  t = c & t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::SEC() {
  //unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  //t &= P.raw|pbits; c = t;
  tick();
  /*t = 1;
  t = c | t;
  P.raw = t & ~0x30;*/
  P.C = 1;
}

void Cpu::AND_ABY() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  Misfire(address_bus, address_bus+d);
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  t = c & t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_03A() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  tick();
}

void Cpu::_03B() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =uint8_t(address_bus); 
  address_bus += 256*MemReadAccess(PC++);
  MemReadAccess(wrap(address_bus, address_bus+d));
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  sb = P.C;
  P.C = t & 0x80;
  auto orig_val = t;
  t = (t << 1) | (sb * 0x01);
  MemWriteAccess(address_bus+d, orig_val);
  MemWriteAccess(address_bus+d, t);
  t = c & t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_03C() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  Misfire(address_bus, address_bus+d);
  tick();
}

void Cpu::AND_ABX() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  Misfire(address_bus, address_bus+d);
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  t = c & t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::ROL_ABX() {
  address_bus = FetchAddressAbsoluteX(PC);
  auto orig_val = MemReadAccess(address_bus);
  unsigned sb = P.C;
  P.C = orig_val & 0x80;
  auto mod_val = (orig_val << 1) | (sb * 0x01);
  MemWriteAccess(address_bus, orig_val);
  MemWriteAccess(address_bus, mod_val);
  P.N = mod_val & 0x80;
  P.Z = uint8_t(mod_val) == 0;
}

void Cpu::_03F() {
  address_bus = FetchAddressAbsoluteX(PC);
  unsigned c = A;
  auto orig_val = MemReadAccess(address_bus);
  unsigned sb = P.C;
  P.C = orig_val & 0x80;
  auto mod_val = (orig_val << 1) | (sb * 0x01);
  MemWriteAccess(address_bus, orig_val);
  MemWriteAccess(address_bus, mod_val);
  //tick();
  mod_val = c & mod_val;
  A = mod_val;
  P.N = mod_val & 0x80;
  P.Z = uint8_t(mod_val) == 0;
}

void Cpu::RTI_IMP() {
  MemReadAccess(PC++); 
  FakeIncrementS();
  P.raw = Pop();
  PC = Pop(); 
  PC |= (Pop() << 8);
}

void Cpu::EOR_INX() {
  address_bus = FetchAddressIndirectX(PC);
  A = A ^ MemReadAccess(address_bus);
  P.N = A & 0x80;
  P.Z = uint8_t(A) == 0;
}

void Cpu::_042() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= A;
  t &= MemReadAccess(PC++);
  P.C = t & 0x01;
  t = (t >> 1) | (sb * 0x80);
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_043() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  address_bus =MemReadAccess(c=address_bus); address_bus += 256*MemReadAccess(wrap(c,c+1));
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  P.C = t & 0x01;
  auto orig_val = t;
  t = (t >> 1) | (sb * 0x80);
  MemWriteAccess(address_bus+d, orig_val);
  MemWriteAccess(address_bus+d, t);
  //tick();
  t = c ^ t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_044() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  tick();
}

void Cpu::EOR_ZPG() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  t = c ^ t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::LSR_ZPG() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= MemReadAccess(address_bus+d);
  P.C = t & 0x01;
  t = (t >> 1) | (sb * 0x80);
  MemWriteAccess(address_bus+d, t);
  tick();
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_047() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  P.C = t & 0x01;
  t = (t >> 1) | (sb * 0x80);
  MemWriteAccess(address_bus+d, t);
  tick();
  t = c ^ t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::PHA() {
  tick();
  Push(A);
}

void Cpu::EOR_IMM() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(PC++);
  t = c ^ t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::LSR_ACC() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= A;
  P.C = t & 0x01;
  t = (t >> 1) | (sb * 0x80);
  tick();
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_04B() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= A;
  t &= MemReadAccess(PC++);
  P.C = t & 0x01;
  t = (t >> 1) | (sb * 0x80);
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::JMP_ABS() {
  PC = FetchAddressAbsolute(PC);
}

void Cpu::EOR_ABS() {
  unsigned  t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = FetchAddressAbsolute(PC);
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus);
  t = c ^ t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::LSR_ABS() {
  //unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = FetchAddressAbsolute(PC);

  auto orig_val = MemReadAccess(address_bus);
  P.C = orig_val & 0x01;
  auto mod_val = (orig_val >> 1);
  MemWriteAccess(address_bus, orig_val);
  MemWriteAccess(address_bus, mod_val);
  //tick();
  P.N = mod_val & 0x80;
  P.Z = uint8_t(mod_val) == 0;
}

void Cpu::_04F() {
  address_bus = FetchAddressAbsolute(PC);

  unsigned c = A; 
  auto orig_val = MemReadAccess(address_bus);
  P.C = orig_val & 0x01;
  auto mod_val = (orig_val >> 1);
  MemWriteAccess(address_bus, orig_val);
  MemWriteAccess(address_bus, mod_val);
  //tick();
  mod_val = c ^ mod_val;
  A = mod_val;
  P.N = mod_val & 0x80;
  P.Z = uint8_t(mod_val) == 0;
}

void Cpu::BVC_REL() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= P.raw|pbits; c = t;
  t = 1;
  t <<= 2;
  t <<= 4;
  t = c & t;
  if(!t) { tick(); Misfire(PC, address_bus = int8_t(address_bus) + PC); PC=address_bus; };
}

void Cpu::EOR_INY() {
  address_bus = FetchAddressIndirectY(PC,true);
  A ^= MemReadAccess(address_bus);
  P.N = A & 0x80;
  P.Z = uint8_t(A) == 0;
}

void Cpu::_052() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =MemReadAccess(c=address_bus); address_bus += 256*MemReadAccess(wrap(c,c+1));
  MemReadAccess(wrap(address_bus, address_bus+d));
  t &= MemReadAccess(address_bus+d);
  P.C = t & 0x01;
  t = (t >> 1) | (sb * 0x80);
  MemWriteAccess(address_bus+d, t);
  tick();
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_053() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =MemReadAccess(c=address_bus); address_bus += 256*MemReadAccess(wrap(c,c+1));
  MemReadAccess(wrap(address_bus, address_bus+d));
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  P.C = t & 0x01;
  auto orig_val = t;
  t = (t >> 1) | (sb * 0x80);
  MemWriteAccess(address_bus+d, orig_val);
  MemWriteAccess(address_bus+d, t);
  //tick();
  t = c ^ t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_054() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  tick();
}

void Cpu::EOR_ZPX() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  t = c ^ t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::LSR_ZPX() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  t &= MemReadAccess(address_bus+d);
  P.C = t & 0x01;
  t = (t >> 1) | (sb * 0x80);
  MemWriteAccess(address_bus+d, t);
  tick();
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_057() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  P.C = t & 0x01;
  t = (t >> 1) | (sb * 0x80);
  MemWriteAccess(address_bus+d, t);
  tick();
  t = c ^ t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::CLI() {
  tick();
  P.I = 0;
}

void Cpu::EOR_ABY() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  Misfire(address_bus, address_bus+d);
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  t = c ^ t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_05A() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  tick();
}

void Cpu::_05B() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  MemReadAccess(wrap(address_bus, address_bus+d));
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  P.C = t & 0x01;
  auto orig_val = t;
  t = (t >> 1) | (sb * 0x80);
  MemWriteAccess(address_bus+d, orig_val);
  MemWriteAccess(address_bus+d, t);
  //tick();
  t = c ^ t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_05C() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  Misfire(address_bus, address_bus+d);
  tick();
}

void Cpu::EOR_ABX() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  Misfire(address_bus, address_bus+d);
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  t = c ^ t;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::LSR_ABX() {
  address_bus = FetchAddressAbsoluteX(PC);
  auto orig_val = MemReadAccess(address_bus);
  P.C = orig_val & 0x01;
  auto mod_val = (orig_val >> 1);
  MemWriteAccess(address_bus, orig_val);
  MemWriteAccess(address_bus, mod_val);
  //tick();
  P.N = mod_val & 0x80;
  P.Z = uint8_t(mod_val) == 0;
}

void Cpu::_05F() {
  address_bus = FetchAddressAbsoluteX(PC);
  unsigned c = A; 
  auto orig_val = MemReadAccess(address_bus);
  P.C = orig_val & 0x01;
  auto mod_val = (orig_val >> 1);
  MemWriteAccess(address_bus, orig_val);
  MemWriteAccess(address_bus, mod_val);
  //tick();
  mod_val = c ^ mod_val;
  A = mod_val;
  P.N = mod_val & 0x80;
  P.Z = uint8_t(mod_val) == 0;
}

void Cpu::RTS_IMP() {
  MemReadAccess(PC++); 
  FakeIncrementS();
  PC = Pop(); 
  PC |= (Pop() << 8);
  MemReadAccess(PC++);
}

void Cpu::ADC_INX() {
  unsigned  t=0xFF, c=0;
  /*address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  address_bus =MemReadAccess(c=address_bus); address_bus += 256*MemReadAccess(wrap(c,c+1));*/
  address_bus = FetchAddressIndirectX(PC);
  t = MemReadAccess(address_bus);
  c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_062() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= A;
  t &= MemReadAccess(PC++);
  sb = P.C;
  P.C = t & 0x01;
  t = (t >> 1) | (sb * 0x80);
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_063() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  address_bus =MemReadAccess(c=address_bus); address_bus += 256*MemReadAccess(wrap(c,c+1));
  t &= MemReadAccess(address_bus+d);
  sb = P.C;
  P.C = t & 0x01;
  auto orig_val = t;
  t = (t >> 1) | (sb * 0x80);
  MemWriteAccess(address_bus+d, orig_val);
  MemWriteAccess(address_bus+d, t);
  //tick();
  c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_064() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  tick();
}

void Cpu::ADC_ZPG() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= MemReadAccess(address_bus+d);
  c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::ROR_ZPG() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= MemReadAccess(address_bus+d);
  sb = P.C;
  P.C = t & 0x01;
  t = (t >> 1) | (sb * 0x80);
  MemWriteAccess(address_bus+d, t);
  tick();
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_067() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= MemReadAccess(address_bus+d);
  sb = P.C;
  P.C = t & 0x01;
  t = (t >> 1) | (sb * 0x80);
  MemWriteAccess(address_bus+d, t);
  tick();
  c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::PLA_IMP() {
  MemReadAccess(PC);
  FakeIncrementS();
  A = Pop();
  P.N = A & 0x80;
  P.Z = uint8_t(A) == 0;
}

void Cpu::ADC_IMM() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= MemReadAccess(PC++);
  c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::ROR_ACC() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= A;
  sb = P.C;
  P.C = t & 0x01;
  t = (t >> 1) | (sb * 0x80);
  tick();
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_06B() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= A;
  t &= MemReadAccess(PC++);
  sb = P.C;
  P.C = t & 0x80;
  t = (t >> 1) | (sb * 0x80);
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
  P.V = (((t >> 5)+1)&2);
}

void Cpu::JMP_IND() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  address_bus =MemReadAccess(c=address_bus); address_bus += 256*MemReadAccess(wrap(c,c+1));
  PC = address_bus;
}

void Cpu::ADC_ABS() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  t &= MemReadAccess(address_bus+d);
  c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::ROR_ABS() {
  //unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = FetchAddressAbsolute(PC);
  auto orig_val = MemReadAccess(address_bus);
  unsigned sb = P.C;
  P.C = orig_val & 0x01;
  auto mod_val = (orig_val >> 1) | (sb * 0x80);
  MemWriteAccess(address_bus, orig_val);
  MemWriteAccess(address_bus, mod_val);
  //tick();
  P.N = mod_val & 0x80;
  P.Z = uint8_t(mod_val) == 0;
}

void Cpu::_06F() {
  //unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = FetchAddressAbsolute(PC);
  auto orig_val = MemReadAccess(address_bus);
  unsigned sb = P.C;
  P.C = orig_val & 0x01;
  auto mod_val = (orig_val >> 1) | (sb * 0x80);
  MemWriteAccess(address_bus, orig_val);
  MemWriteAccess(address_bus, mod_val);
  //tick();
  unsigned c = mod_val; 
  mod_val += A + P.C; 
  P.V = (c^mod_val) & (A^mod_val) & 0x80; 
  P.C = mod_val & 0x100;
  A = mod_val;
  P.N = mod_val & 0x80;
  P.Z = uint8_t(mod_val) == 0;
}

void Cpu::BVS_REL() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= P.raw|pbits; c = t;
  t = 1;
  t <<= 2;
  t <<= 4;
  t = c & t;
  if(t) { tick(); Misfire(PC, address_bus = int8_t(address_bus) + PC); PC=address_bus; };
}

void Cpu::ADC_INY() {
  unsigned t,c;
  address_bus = FetchAddressIndirectY(PC,true);
  t = MemReadAccess(address_bus);
  c = t; 
  t += A + P.C; 
  P.V = (c^t) & (A^t) & 0x80; 
  P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_072() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =MemReadAccess(c=address_bus); address_bus += 256*MemReadAccess(wrap(c,c+1));
  MemReadAccess(wrap(address_bus, address_bus+d));
  t &= MemReadAccess(address_bus+d);
  sb = P.C;
  P.C = t & 0x01;
  t = (t >> 1) | (sb * 0x80);
  MemWriteAccess(address_bus+d, t);
  tick();
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_073() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =MemReadAccess(c=address_bus); address_bus += 256*MemReadAccess(wrap(c,c+1));
  MemReadAccess(wrap(address_bus, address_bus+d));
  t &= MemReadAccess(address_bus+d);
  sb = P.C;
  P.C = t & 0x01;
  auto orig_val = t;
  t = (t >> 1) | (sb * 0x80);
  MemWriteAccess(address_bus+d, orig_val);
  MemWriteAccess(address_bus+d, t);
  //tick();
  c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_074() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  tick();
}

void Cpu::ADC_ZPX() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  t &= MemReadAccess(address_bus+d);
  c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::ROR_ZPX() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  t &= MemReadAccess(address_bus+d);
  sb = P.C;
  P.C = t & 0x01;
  t = (t >> 1) | (sb * 0x80);
  MemWriteAccess(address_bus+d, t);
  tick();
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_077() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  t &= MemReadAccess(address_bus+d);
  sb = P.C;
  P.C = t & 0x01;
  t = (t >> 1) | (sb * 0x80);
  MemWriteAccess(address_bus+d, t);
  tick();
  c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::SEI() {
  tick();
  P.I = 1;
}

void Cpu::ADC_ABY() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  Misfire(address_bus, address_bus+d);
  t &= MemReadAccess(address_bus+d);
  c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_07A() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  tick();
}

void Cpu::_07B() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  MemReadAccess(wrap(address_bus, address_bus+d));
  t &= MemReadAccess(address_bus+d);
  sb = P.C;
  P.C = t & 0x01;
  auto orig_val = t;
  t = (t >> 1) | (sb * 0x80);
  MemWriteAccess(address_bus+d, orig_val);
  MemWriteAccess(address_bus+d, t);
  //tick();
  c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_07C() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  Misfire(address_bus, address_bus+d);
  tick();
}

void Cpu::ADC_ABX() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  Misfire(address_bus, address_bus+d);
  t &= MemReadAccess(address_bus+d);
  c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::ROR_ABX() {
  /*unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus); 
  address_bus += 256*MemReadAccess(PC++);
  MemReadAccess(wrap(address_bus, address_bus+d));*/
  address_bus = FetchAddressAbsoluteX(PC);
  auto orig_val = MemReadAccess(address_bus);
  unsigned sb = P.C;
  P.C = orig_val & 0x01;
  auto mod_val = (orig_val >> 1) | (sb * 0x80);
  MemWriteAccess(address_bus, orig_val);
  MemWriteAccess(address_bus, mod_val);
  //tick();
  P.N = mod_val & 0x80;
  P.Z = uint8_t(mod_val) == 0;
}

void Cpu::_07F() {
  address_bus = FetchAddressAbsoluteX(PC);
  auto orig_val = MemReadAccess(address_bus);
  unsigned sb = P.C;
  P.C = orig_val & 0x01;
  auto mod_val = (orig_val >> 1) | (sb * 0x80);
  MemWriteAccess(address_bus, orig_val);
  MemWriteAccess(address_bus, mod_val);
  //tick();
  unsigned c = mod_val; 
  mod_val += A + P.C; 
  P.V = (c^mod_val) & (A^mod_val) & 0x80; 
  P.C = mod_val & 0x100;
  A = mod_val;
  P.N = mod_val & 0x80;
  P.Z = uint8_t(mod_val) == 0;
}

void Cpu::_080() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= Y;
  t &= MemReadAccess(PC++);
}

void Cpu::STA_INX() {
  address_bus = FetchAddressIndirectX(PC);
  MemWriteAccess(address_bus, A);
}

void Cpu::_082() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= MemReadAccess(PC++);
}

void Cpu::_083() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  address_bus =MemReadAccess(c=address_bus); address_bus += 256*MemReadAccess(wrap(c,c+1));
  t &= A;
  t &= X;
  MemWriteAccess(address_bus+d, t);
}

void Cpu::STY_ZPG() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= Y;
  MemWriteAccess(address_bus+d, t);
}

void Cpu::STA_ZPG() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= A;
  MemWriteAccess(address_bus+d, t);
}

void Cpu::STX_ZPG() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= X;
  MemWriteAccess(address_bus+d, t);
}

void Cpu::_087() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= A;
  t &= X;
  MemWriteAccess(address_bus+d, t);
}

void Cpu::DEY_IMP() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= Y;
  t = uint8_t(t - 1);
  tick();
  Y = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_089() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= A;
  t &= MemReadAccess(PC++);
}

void Cpu::TXA_IMP() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= X;
  tick();
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_08B() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= A;
  t &= X;
  t &= MemReadAccess(PC++);
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::STY_ABS() {
  address_bus = MemReadAccess(PC++);
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  MemWriteAccess(address_bus, Y);
}

void Cpu::STA_ABS() {
  address_bus = MemReadAccess(PC++);
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  MemWriteAccess(address_bus, A);
}

void Cpu::STX_ABS() {
  address_bus = MemReadAccess(PC++);
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  MemWriteAccess(address_bus, X);
}

void Cpu::_08F() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  t &= A;
  t &= X;
  MemWriteAccess(address_bus+d, t);
}

void Cpu::BCC_REL() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= P.raw|pbits; c = t;
  t = 1;
  t = c & t;
  if(!t) { tick(); Misfire(PC, address_bus = int8_t(address_bus) + PC); PC=address_bus; };
}

void Cpu::STA_INY() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =MemReadAccess(c=address_bus); 
  address_bus += 256*MemReadAccess(wrap(c,c+1));
  MemReadAccess(wrap(address_bus, address_bus+d));
  t &= A;
  MemWriteAccess(address_bus+d, t);
}

void Cpu::_092() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =MemReadAccess(c=address_bus); address_bus += 256*MemReadAccess(wrap(c,c+1));
  MemReadAccess(wrap(address_bus, address_bus+d));
  t &= X;
  MemWriteAccess(address_bus+d, t);
}

void Cpu::_093() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =MemReadAccess(c=address_bus); address_bus += 256*MemReadAccess(wrap(c,c+1));
  MemReadAccess(wrap(address_bus, address_bus+d));
  t &= A;
  t &= X;
  MemWriteAccess(address_bus+d, t);
}

void Cpu::STY_ZPX() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  t &= Y;
  MemWriteAccess(address_bus+d, t);
}

void Cpu::STA_ZPX() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  t &= A;
  MemWriteAccess(address_bus+d, t);
}

void Cpu::STX_ZPX() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  t &= X;
  MemWriteAccess(address_bus+d, t);
}

void Cpu::_097() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  t &= A;
  t &= X;
  MemWriteAccess(address_bus+d, t);
}

void Cpu::TYA_IMP() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= Y;
  tick();
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::STA_ABY() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  MemReadAccess(wrap(address_bus, address_bus+d));
  t &= A;
  MemWriteAccess(address_bus+d, t);
}

void Cpu::TXS_IMP() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= X;
  tick();
  S = t;
}

void Cpu::_09B() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  MemReadAccess(wrap(address_bus, address_bus+d));
  t &= A;
  t &= X;
  MemWriteAccess(wrap(address_bus, address_bus+d), t &= ((address_bus+d) >> 8));
  S = t;
}

void Cpu::_09C() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  MemReadAccess(wrap(address_bus, address_bus+d));
  t &= Y;
  MemWriteAccess(wrap(address_bus, address_bus+d), t &= ((address_bus+d) >> 8));
}

void Cpu::STA_ABX() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  MemReadAccess(wrap(address_bus, address_bus+d));
  t &= A;
  MemWriteAccess(address_bus+d, t);
}

void Cpu::_09E() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  MemReadAccess(wrap(address_bus, address_bus+d));
  t &= X;
  MemWriteAccess(wrap(address_bus, address_bus+d), t &= ((address_bus+d) >> 8));
}

void Cpu::_09F() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  MemReadAccess(wrap(address_bus, address_bus+d));
  t &= A;
  t &= X;
  MemWriteAccess(wrap(address_bus, address_bus+d), t &= ((address_bus+d) >> 8));
}

void Cpu::LDY_IMM() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= MemReadAccess(PC++);
  Y = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::LDA_INX() {
  address_bus = FetchAddressIndirectX(PC);
  A = MemReadAccess(address_bus);
  P.N = A & 0x80;
  P.Z = A == 0;
}

void Cpu::LDX_IMM() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= MemReadAccess(PC++);
  X = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_0A3() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  address_bus =MemReadAccess(c=address_bus); address_bus += 256*MemReadAccess(wrap(c,c+1));
  t &= MemReadAccess(address_bus+d);
  A = t;
  X = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::LDY_ZPG() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= MemReadAccess(address_bus+d);
  Y = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::LDA_ZPG() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= MemReadAccess(address_bus+d);
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::LDX_ZPG() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= MemReadAccess(address_bus+d);
  X = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_0A7() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= MemReadAccess(address_bus+d);
  A = t;
  X = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::TAY_IMP() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= A;
  tick();
  Y = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::LDA_IMM() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= MemReadAccess(PC++);
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::TAX_IMP() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= A;
  tick();
  X = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_0AB() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= MemReadAccess(PC++);
  A = t;
  X = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::LDY_ABS() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  t &= MemReadAccess(address_bus+d);
  Y = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::LDA_ABS() {
  address_bus = MemReadAccess(PC++) & 0xFF;
  address_bus += 256*MemReadAccess(PC++);
  A = MemReadAccess(address_bus);
  P.N = A & 0x80;
  P.Z = uint8_t(A) == 0;
}

void Cpu::LDX_ABS() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  t &= MemReadAccess(address_bus+d);
  X = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_0AF() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  t &= MemReadAccess(address_bus+d);
  A = t;
  X = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::BCS_REL() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= P.raw|pbits; c = t;
  t = 1;
  t = c & t;
  if(t) { tick(); Misfire(PC, address_bus = int8_t(address_bus) + PC); PC=address_bus; };
}

void Cpu::LDA_INY() {
  address_bus = FetchAddressIndirectY(PC,true);
  A = MemReadAccess(address_bus);
  P.N = A & 0x80;
  P.Z = uint8_t(A) == 0;
}

void Cpu::_0B2() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =MemReadAccess(c=address_bus); address_bus += 256*MemReadAccess(wrap(c,c+1));
  Misfire(address_bus, address_bus+d);
  t &= MemReadAccess(address_bus+d);
  X = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_0B3() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =MemReadAccess(c=address_bus); address_bus += 256*MemReadAccess(wrap(c,c+1));
  Misfire(address_bus, address_bus+d);
  t &= MemReadAccess(address_bus+d);
  A = t;
  X = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::LDY_ZPX() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  t &= MemReadAccess(address_bus+d);
  Y = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::LDA_ZPX() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  t &= MemReadAccess(address_bus+d);
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::LDX_ZPY() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  t &= MemReadAccess(address_bus+d);
  X = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_0B7() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  t &= MemReadAccess(address_bus+d);
  A = t;
  X = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::CLV_IMP() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= P.raw|pbits; c = t;
  tick();
  t = 1;
  t <<= 2;
  t <<= 4;
  t = uint8_t(~t);
  t = c & t;
  P.raw = t & ~0x30;
}

void Cpu::LDA_ABY() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  Misfire(address_bus, address_bus+d);
  t &= MemReadAccess(address_bus+d);
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::TSX_IMP() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= S;
  tick();
  X = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_0BB() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  Misfire(address_bus, address_bus+d);
  t &= S;
  t &= MemReadAccess(address_bus+d);
  A = t;
  X = t;
  S = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::LDY_ABX() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  Misfire(address_bus, address_bus+d);
  t &= MemReadAccess(address_bus+d);
  Y = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::LDA_ABX() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  Misfire(address_bus, address_bus+d);
  t &= MemReadAccess(address_bus+d);
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::LDX_ABY() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  Misfire(address_bus, address_bus+d);
  t &= MemReadAccess(address_bus+d);
  X = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_0BF() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  Misfire(address_bus, address_bus+d);
  t &= MemReadAccess(address_bus+d);
  A = t;
  X = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::CPY_IMM() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= Y;
  c = t; t = 0xFF;
  t &= MemReadAccess(PC++);
  t = c - t; P.C = ~t & 0x100;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::CMP_INX() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  address_bus =MemReadAccess(c=address_bus); address_bus += 256*MemReadAccess(wrap(c,c+1));
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  t = c - t; P.C = ~t & 0x100;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_0C2() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= MemReadAccess(PC++);
}

void Cpu::_0C3() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  address_bus =MemReadAccess(c=address_bus); address_bus += 256*MemReadAccess(wrap(c,c+1));
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  auto orig_val = t;
  t = uint8_t(t - 1);
  MemWriteAccess(address_bus+d, orig_val);
  MemWriteAccess(address_bus+d, t);
  //tick();
  t = c - t; P.C = ~t & 0x100;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::CPY_ZPG() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= Y;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  t = c - t; P.C = ~t & 0x100;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::CMP_ZPG() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  t = c - t; P.C = ~t & 0x100;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::DEC_ZPG() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= MemReadAccess(address_bus+d);
  t = uint8_t(t - 1);
  MemWriteAccess(address_bus+d, t);
  tick();
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_0C7() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  t = uint8_t(t - 1);
  MemWriteAccess(address_bus+d, t);
  tick();
  t = c - t; P.C = ~t & 0x100;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::INY_IMP() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= Y;
  t = uint8_t(t + 1);
  tick();
  Y = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::CMP_IMM() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(PC++);
  t = c - t; P.C = ~t & 0x100;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::DEX_IMP() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= X;
  t = uint8_t(t - 1);
  tick();
  X = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_0CB() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= A;
  t &= X;
  c = t; t = 0xFF;
  t &= MemReadAccess(PC++);
  t = c - t; P.C = ~t & 0x100;
  X = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::CPY_ABS() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  t &= Y;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  t = c - t; P.C = ~t & 0x100;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::CMP_ABS() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  t = c - t; P.C = ~t & 0x100;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::DEC_ABS() {
  //unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = FetchAddressAbsolute(PC);
  auto orig_val = MemReadAccess(address_bus);
  auto mod_val = uint8_t(orig_val - 1);
  MemWriteAccess(address_bus, orig_val);
  MemWriteAccess(address_bus, mod_val);
  //tick();
  P.N = mod_val & 0x80;
  P.Z = uint8_t(mod_val) == 0;
}

void Cpu::_0CF() {
  //unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = FetchAddressAbsolute(PC);
  //t &= A;
  unsigned c = A; 
  //t = 0xFF;
  auto orig_val = MemReadAccess(address_bus);
  auto mod_val = uint8_t(orig_val - 1);
  MemWriteAccess(address_bus, orig_val);
  MemWriteAccess(address_bus, mod_val);
  //tick();
  auto t = c - mod_val; 
  P.C = ~t & 0x100;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::BNE_REL() {

  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= P.raw|pbits; 
  c = t;
  t = 1;
  t <<= 1;
  t = c & t;
  if(!t) { tick(); Misfire(PC, address_bus = int8_t(address_bus) + PC); PC=address_bus; };
}

void Cpu::CMP_INY() {
  address_bus = FetchAddressIndirectY(PC,true);
  unsigned t = 0,c = A;
  t = MemReadAccess(address_bus);
  t = c - t;
  P.C = ~t & 0x100;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_0D2() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =MemReadAccess(c=address_bus); address_bus += 256*MemReadAccess(wrap(c,c+1));
  MemReadAccess(wrap(address_bus, address_bus+d));
  t &= MemReadAccess(address_bus+d);
  t = uint8_t(t - 1);
  MemWriteAccess(address_bus+d, t);
  tick();
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_0D3() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =MemReadAccess(c=address_bus); address_bus += 256*MemReadAccess(wrap(c,c+1));
  MemReadAccess(wrap(address_bus, address_bus+d));
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  auto orig_val = t;
  t = uint8_t(t - 1);
  MemWriteAccess(address_bus+d, orig_val);
  MemWriteAccess(address_bus+d, t);
  //tick();
  t = c - t; P.C = ~t & 0x100;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_0D4() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  tick();
}

void Cpu::CMP_ZPX() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  t = c - t; P.C = ~t & 0x100;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::DEC_ZPX() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  t &= MemReadAccess(address_bus+d);
  t = uint8_t(t - 1);
  MemWriteAccess(address_bus+d, t);
  tick();
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_0D7() {
  unsigned  d=0, t=0xFF, c=0, sb=0;
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  t = uint8_t(t - 1);
  MemWriteAccess(address_bus+d, t);
  tick();
  t = c - t; P.C = ~t & 0x100;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::CLD_IMP() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= P.raw|pbits; c = t;
  tick();
  t = 1;
  t <<= 1;
  t <<= 2;
  t = uint8_t(~t);
  t = c & t;
  P.raw = t & ~0x30;
}

void Cpu::CMP_ABY() {
  unsigned  d=0, t=0xFF, c=0, sb=0;
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  Misfire(address_bus, address_bus+d);
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  t = c - t; P.C = ~t & 0x100;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_0DA() {
  tick();
}

void Cpu::_0DB() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  MemReadAccess(wrap(address_bus, address_bus+d));
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  auto orig_val = t;
  t = uint8_t(t - 1);
  MemWriteAccess(address_bus+d, orig_val);
  MemWriteAccess(address_bus+d, t);
  //tick();
  t = c - t; P.C = ~t & 0x100;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_0DC() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  Misfire(address_bus, address_bus+d);
  tick();
}

void Cpu::CMP_ABX() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  Misfire(address_bus, address_bus+d);
  t &= A;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  t = c - t; P.C = ~t & 0x100;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::DEC_ABX() {
  address_bus = FetchAddressAbsoluteX(PC);
  auto orig_val = MemReadAccess(address_bus);
  auto mod_val = uint8_t(orig_val - 1);
  MemWriteAccess(address_bus, orig_val);
  MemWriteAccess(address_bus, mod_val);
  //tick();
  P.N = mod_val & 0x80;
  P.Z = uint8_t(mod_val) == 0;
}

void Cpu::_0DF() {
  address_bus = FetchAddressAbsoluteX(PC);
  unsigned c = A;
  auto orig_val = MemReadAccess(address_bus);
  auto mod_val = uint8_t(orig_val - 1);
  MemWriteAccess(address_bus, orig_val);
  MemWriteAccess(address_bus, mod_val);
  unsigned t = c - mod_val; P.C = ~t & 0x100;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::CPX_IMM() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= X;
  c = t; t = 0xFF;
  t &= MemReadAccess(PC++);
  t = c - t; P.C = ~t & 0x100;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::SBC_INX() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  address_bus =MemReadAccess(c=address_bus); address_bus += 256*MemReadAccess(wrap(c,c+1));
  t &= MemReadAccess(address_bus+d);
  t = uint8_t(~t);
  c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_0E2() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= MemReadAccess(PC++);
}

void Cpu::_0E3() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  address_bus =MemReadAccess(c=address_bus); address_bus += 256*MemReadAccess(wrap(c,c+1));
  t &= MemReadAccess(address_bus+d);
  auto orig_val = t;
  t = uint8_t(t + 1);
  MemWriteAccess(address_bus+d, orig_val);
  MemWriteAccess(address_bus+d, t);
  //tick();
  t = uint8_t(~t);
  c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::CPX_ZPG() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= X;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  t = c - t; P.C = ~t & 0x100;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::SBC_ZPG() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= MemReadAccess(address_bus+d);
  t = uint8_t(~t);
  c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::INC_ZPG() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= MemReadAccess(address_bus+d);
  t = uint8_t(t + 1);
  MemWriteAccess(address_bus+d, t);
  tick();
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_0E7() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= MemReadAccess(address_bus+d);
  t = uint8_t(t + 1);
  MemWriteAccess(address_bus+d, t);
  tick();
  t = uint8_t(~t);
  c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::INX_IMP() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= X;
  t = uint8_t(t + 1);
  tick();
  X = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::SBC_IMM() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= MemReadAccess(PC++);
  t = uint8_t(~t);
  c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::NOP_IMP() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  tick();
}

void Cpu::_0EB() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= MemReadAccess(PC++);
  t = uint8_t(~t);
  c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::CPX_ABS() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  t &= X;
  c = t; t = 0xFF;
  t &= MemReadAccess(address_bus+d);
  t = c - t; P.C = ~t & 0x100;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::SBC_ABS() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  t &= MemReadAccess(address_bus+d);
  t = uint8_t(~t);
  c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::INC_ABS() {
  //unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = FetchAddressAbsolute(PC);
  auto orig_val = MemReadAccess(address_bus);
  auto mod_val = uint8_t(orig_val + 1);
  MemWriteAccess(address_bus, orig_val);
  MemWriteAccess(address_bus, mod_val);
  //tick();
  P.N = mod_val & 0x80;
  P.Z = uint8_t(mod_val) == 0;
}

void Cpu::_0EF() {
  //unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = FetchAddressAbsolute(PC);
  auto orig_val = MemReadAccess(address_bus);
  auto mod_val = uint8_t(orig_val + 1);
  MemWriteAccess(address_bus, orig_val);
  MemWriteAccess(address_bus, mod_val);
  //tick();
  unsigned t = uint8_t(~mod_val);
  unsigned c = t; 
  t += A + P.C; 
  P.V = (c^t) & (A^t) & 0x80; 
  P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::BEQ_REL() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  t &= P.raw|pbits; c = t;
  t = 1;
  t <<= 1;
  t = c & t;
  if(t) { tick(); Misfire(PC, address_bus = int8_t(address_bus) + PC); PC=address_bus; };
}

void Cpu::SBC_INY() {
  address_bus = FetchAddressIndirectY(PC,true);
  unsigned c = 0,t = MemReadAccess(address_bus);
  t = uint8_t(~t);
  c = t;
  t += A + P.C; 
  P.V = (c^t) & (A^t) & 0x80; 
  P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_0F2() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =MemReadAccess(c=address_bus); address_bus += 256*MemReadAccess(wrap(c,c+1));
  MemReadAccess(wrap(address_bus, address_bus+d));
  t &= MemReadAccess(address_bus+d);
  t = uint8_t(t + 1);
  MemWriteAccess(address_bus+d, t);
  tick();
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_0F3() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =MemReadAccess(c=address_bus); address_bus += 256*MemReadAccess(wrap(c,c+1));
  MemReadAccess(wrap(address_bus, address_bus+d));
  t &= MemReadAccess(address_bus+d);
  auto orig_val = t;
  t = uint8_t(t + 1);
  MemWriteAccess(address_bus+d, orig_val);
  MemWriteAccess(address_bus+d, t);
  //tick();
  t = uint8_t(~t);
  c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_0F4() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  tick();
}

void Cpu::SBC_ZPX() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  t &= MemReadAccess(address_bus+d);
  t = uint8_t(~t);
  c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::INC_ZPX() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  t &= MemReadAccess(address_bus+d);
  t = uint8_t(t + 1);
  MemWriteAccess(address_bus+d, t);
  tick();
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_0F7() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus+d); d=0; tick();
  t &= MemReadAccess(address_bus+d);
  t = uint8_t(t + 1);
  MemWriteAccess(address_bus+d, t);
  tick();
  t = uint8_t(~t);
  c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::SED_IMP() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  t &= P.raw|pbits; c = t;
  tick();
  t = 1;
  t <<= 1;
  t <<= 2;
  t = c | t;
  P.raw = t & ~0x30;
}

void Cpu::SBC_ABY() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  Misfire(address_bus, address_bus+d);
  t &= MemReadAccess(address_bus+d);
  t = uint8_t(~t);
  c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_0FA() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  tick();
}

void Cpu::_0FB() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = Y;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  MemReadAccess(wrap(address_bus, address_bus+d));
  t &= MemReadAccess(address_bus+d);
  auto orig_val = t;
  t = uint8_t(t + 1);
  MemWriteAccess(address_bus+d, orig_val);
  MemWriteAccess(address_bus+d, t);
  //tick();
  t = uint8_t(~t);
  c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::_0FC() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus); address_bus += 256*MemReadAccess(PC++);
  Misfire(address_bus, address_bus+d);
  tick();
}

void Cpu::SBC_ABX() {
  unsigned  d=0, t=0xFF, c=0, sb=0, pbits =  0x30;//0x20 for op > 0x100
  address_bus = MemReadAccess(PC++);
  d = X;
  address_bus =uint8_t(address_bus); 
  address_bus += 256*MemReadAccess(PC++);
  Misfire(address_bus, address_bus+d);
  t &= MemReadAccess(address_bus+d);
  t = uint8_t(~t);
  c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::INC_ABX() {
  address_bus = FetchAddressAbsoluteX(PC);
  auto orig_val = MemReadAccess(address_bus);
  auto mod_val = uint8_t(orig_val + 1);
  MemWriteAccess(address_bus, orig_val);
  MemWriteAccess(address_bus, mod_val);
  //tick();
  P.N = mod_val & 0x80;
  P.Z = uint8_t(mod_val) == 0;
}

void Cpu::_0FF() {
  address_bus = FetchAddressAbsoluteX(PC);
  auto orig_val = MemReadAccess(address_bus);
  auto mod_val = uint8_t(orig_val + 1);
  MemWriteAccess(address_bus, orig_val);
  MemWriteAccess(address_bus, mod_val);
  //tick();
  unsigned t = uint8_t(~mod_val);
  unsigned c = t; t += A + P.C; P.V = (c^t) & (A^t) & 0x80; P.C = t & 0x100;
  A = t;
  P.N = t & 0x80;
  P.Z = uint8_t(t) == 0;
}

void Cpu::NMI() {
  InterruptVector(0xFFFA);
  /*unsigned  d=0, t=0xFF, c=0, sb=0;
  address_bus = 0xFFFA;
  address_bus =MemReadAccess(c=address_bus); 
  address_bus += 256*MemReadAccess(wrap(c,c+1));
  t &= P.raw|0x20; c = t;
  tick();
  d=PC-1; 
  Push16(d);
  PC = address_bus;
  Push(t);
  t = 1;
  t <<= 2;
  t = c | t;
  P.raw = t & ~0x30;*/
}

void Cpu::RESET() {
  

  InterruptVector(0xFFFC);
  reset = false;
  /*nes().cpu().MemWriteAccess(0x4017,nes().apu().last_4017_value);
  for (int i=0;i<10;++i)
    tick();  */
}

void Cpu::IRQ() {
  MemReadAccess(PC);
  Push16(PC);
  P.raw |= 0x20;
  Push(P.raw & 0xEF);
  P.I = 1;
  if (nmi == true && !nmi_edge_detected)
  {
    nmi_edge_detected = true;
    address_bus = FetchAddressMode0(0xFFFA);
  }
  else
  {
    address_bus = FetchAddressMode0(0xFFFE);
  }
  PC = address_bus;
}
