

enum AddressingModes {
  kImmediate = 0,
  kAbsolute = 1,
  kAbsoluteX = 2,
  kAbsoluteY = 3,
  kZeroPage = 4,
  kZeroPageX = 5,
  kZeroPageY = 6,
  kImplied = 7,
  kAccumulator = 8,
  kIndirect = 9,
  kIndirectX = 10,
  kIndirectY = 11,
  kRelative = 12,
  kUnused = 13
};
class CpuDebugger;
class Cpu : public Component /* CPU: Ricoh RP2A03 (based on MOS6502, almost the same as in Commodore 64) */
{
 friend CpuDebugger;
 public:
  uint8_t* RAM;
  bool reset, nmi, nmi_edge_detected;
  bool enable_irq,irq_line;
  bool disable_op_read;
  // CPU registers:
  uint16_t PC;
  uint8_t A,X,Y,S;
  uint32_t addr; //read address
  uint16_t read_address;
  uint16_t write_address;

  union /* Status flags: */
  {
      uint8_t raw;
      struct {
        bool C:1;
        bool Z:1;
        bool I:1;
        bool D:1;
        bool _4:1;
        bool _5:1;
        bool V:1;
        bool N:1;
      };
      /*RegisterBit<0> C; // carry
      RegisterBit<1> Z; // zero
      RegisterBit<2> I; // interrupt enable/disable
      RegisterBit<3> D; // decimal mode (unsupported on NES, but flag exists)
      // 4,5 (0x10,0x20) don't exist
      RegisterBit<4,2> BRK; // overflow
      RegisterBit<6> V; // overflow
      RegisterBit<7> N; // negative*/
  } P;

  //timing
  double frequency_mhz_;
  uint64_t last_cpu_cycles_;
  uint64_t cycles_per_second;
  uint64_t cycles;
  uint8_t current_inst_cycles;
  
  Cpu();

  int Initialize(Nes* nes);
  int Deinitialize();
  void Power();
  void Reset();
  void tick();
  uint8_t MemReadAccess(uint16_t addr);
  void MemWriteAccess(uint16_t address, uint8_t value);

  void Op();
  void Irq() {
    _102();
  }
  void Nmi() {
    nmi = true;
    nmi_edge_detected = false;
  }
  void RaiseIRQLine() {
    enable_irq = true;
    irq_line = enable_irq && !P.I;
  }
  void LowerIRQLine() {
    enable_irq = false;
    irq_line = false;
  }
 protected:
  typedef uint16_t (Cpu::*FetchAddressMode)(uint16_t);
  typedef void (Cpu::*Instruction)();
  static AddressingModes addressing_modes[256];
  unsigned int op,prev_op;
  int delay_irq;
  FetchAddressMode fetch_address[13];
  Instruction instructions[0x108];
    
  uint16_t wrap(uint16_t oldaddr, uint16_t newaddr)  { return (oldaddr & 0xFF00) + uint8_t(newaddr); }
  void Misfire(uint16_t old, uint16_t addr) { uint16_t q = wrap(old, addr); if(q != addr) MemReadAccess(q); }
  uint8_t   Pop()        { return MemReadAccess(0x100 | uint8_t(++S)); }
  uint16_t   Pop16()        { return Pop() | (Pop()<<8); }
  void Push(uint8_t v)   { MemWriteAccess(0x100 | uint8_t(S--), v); }
  void Push16(uint16_t data) {
    Push(data>>8); 
    Push(data&0xFF);
  }
  void PushPC() {
    Push16(PC);
  }
  uint16_t FetchAddressMode0(uint16_t address) {
    return MemReadAccess(address) + (MemReadAccess(wrap(address,address+1))<<8);
  }
  
  uint16_t FetchAddressAbsolute(uint16_t& address) {
    auto r = (MemReadAccess(address++) & 0xFF);
    r += (MemReadAccess(address++) << 8);
    return r;
  }

  uint16_t FetchAddressAbsoluteX(uint16_t& address) {
    auto r = (MemReadAccess(address++) & 0xFF);
    r=uint8_t(r); 
    r+=(MemReadAccess(address++) << 8);
    MemReadAccess(wrap(r, r+X));
    return r+X;
  }

  void InterruptVector(uint16_t int_addr) {
    unsigned t=0xFF, c=0;
    addr = int_addr;
    addr=MemReadAccess(c=addr); 
    addr+=256*MemReadAccess(wrap(c,c+1));
    t &= P.raw|(prev_op==0?0x30:0x20);// c = t;
    tick();
    Push16(PC+(op?-1:1));
    PC = addr;
    Push(t);
    //if (prev_op == 0) {
    //  P.Z = P.I = 0;
    //} else {
      P.I = 1;
    //}
  }

  void BRK();
  void ORA_INX();
  void _002();
  void _003();
  void _004();
  void ORA2();
  void ASL1();
  void _007();
  void PHP();
  void ORA3();
  void ASL2();
  void _00B();
  void _00C();
  void ORA4();
  void ASL3();
  void _00F();
  void BPL();
  void ORA_INY();
  void _012();
  void _013();
  void _014();
  void ORA6();
  void ASL4();
  void _017();
  void CLC() ;
  void ORA7();
  void _01A();
  void _01B();
  void _01C();
  void ORA8();
  void ASL5();
  void _01F();
  void JSR_ABS();
  void AND_INX();
  void _022();
  void _023();
  void BIT_ZPG();
  void AND_ZPG();
  void ROL_ZPG();
  void _027();
  void PLP();
  void AND_IMM();
  void ROL_ACC();
  void _02B();
  void BIT_ABS();
  void AND_ABS();
  void ROL_ABS();
  void _02F();
  void BMI_REL();
  void AND_INY();
  void _032();
  void _033();
  void _034();
  void AND_ZPX();
  void ROL_ZPX();
  void _037();
  void SEC_IMP();
  void AND_ABY();
  void _03A();
  void _03B();
  void _03C();
  void AND_ABX();
  void ROL_ABX();
  void _03F();
  void RTI_IMP();
  void EOR_INX();
  void _042();
  void _043();
  void _044();
  void EOR_ZPG();
  void LSR_ZPG();
  void _047();
  void PHA();
  void EOR_IMM();
  void LSR_ACC();
  void _04B();
  void JMP_ABS();
  void EOR_ABS();
  void LSR_ABS();
  void _04F();
  void BVC_REL();
  void EOR_INY();
  void _052();
  void _053();
  void _054();
  void EOR_ZPX();
  void LSR_ZPX();
  void _057();
  void CLI();
  void EOR_ABY();
  void _05A();
  void _05B();
  void _05C();
  void EOR_ABX();
  void LSR_ABX();
  void _05F();
  void RTS_IMP();
  void ADC_INX();
  void _062();
  void _063();
  void _064();
  void ADC_ZPG();
  void ROR_ZPG();
  void _067();
  void PLA_IMP();
  void ADC_IMM();
  void ROR_ACC();
  void _06B();
  void JMP_IND();
  void ADC_ABS();
  void ROR_ABS();
  void _06F();
  void BVS_REL();
  void ADC_INY();
  void _072();
  void _073();
  void _074();
  void ADC_ZPX();
  void ROR_ZPX();
  void _077();
  void SEI();
  void ADC_ABY();
  void _07A();
  void _07B();
  void _07C();
  void ADC_ABX();
  void ROR_ABX();
  void _07F();
  void _080();
  void STA_INX();
  void _082();
  void _083();
  void STY_ZPG();
  void STA_ZPG();
  void STX_ZPG();
  void _087();
  void DEY_IMP();
  void _089();
  void TXA_IMP();
  void _08B();
  void STY_ABS();
  void STA_ABS();
  void STX_ABS();
  void _08F();
  void BCC_REL();
  void STA_INY();
  void _092();
  void _093();
  void STY_ZPX();
  void STA_ZPX();
  void STX_ZPX();
  void _097();
  void TYA_IMP();
  void STA_ABY();
  void TXS_IMP();
  void _09B();
  void _09C();
  void STA_ABX();
  void _09E();
  void _09F();
  void LDY_IMM();
  void LDA_INX();
  void LDX_IMM();
  void _0A3();
  void LDY_ZPG();
  void LDA_ZPG();
  void LDX_ZPG();
  void _0A7();
  void TAY_IMP();
  void LDA_IMM();
  void TAX_IMP();
  void _0AB();
  void LDY_ABS();
  void LDA_ABS();
  void LDX_ABS();
  void _0AF();
  void BCS_REL();
  void LDA_INY();
  void _0B2();
  void _0B3();
  void LDY_ZPX();
  void LDA_ZPX();
  void LDX_ZPY();
  void _0B7();
  void CLV_IMP();
  void LDA_ABY();
  void TSX_IMP();
  void _0BB();
  void LDY_ABX();
  void LDA_ABX();
  void LDX_ABY();
  void _0BF();
  void CPY_IMM();
  void CMP_INX();
  void _0C2();
  void _0C3();
  void CPY_ZPG();
  void CMP_ZPG();
  void DEC_ZPG();
  void _0C7();
  void INY_IMP();
  void CMP_IMM();
  void DEX_IMP();
  void _0CB();
  void CPY_ABS();
  void CMP_ABS();
  void DEC_ABS();
  void _0CF();
  void BNE_REL();
  void CMP_INY();
  void _0D2();
  void _0D3();
  void _0D4();
  void CMP_ZPX();
  void DEC_ZPX();
  void _0D7();
  void CLD_IMP();
  void CMP_ABY();
  void _0DA();
  void _0DB();
  void _0DC();
  void CMP_ABX();
  void DEC_ABX();
  void _0DF();
  void CPX_IMM();
  void SBC_INX();
  void _0E2();
  void _0E3();
  void CPX_ZPG();
  void SBC_ZPG();
  void INC_ZPG();
  void _0E7();
  void INX_IMP();
  void SBC_IMM();
  void NOP_IMP();
  void _0EB();
  void CPX_ABS();
  void SBC_ABS();
  void INC_ABS();
  void _0EF();
  void BEQ_REL();
  void SBC_INY();
  void _0F2();
  void _0F3();
  void _0F4();
  void SBC_ZPX();
  void INC_ZPX();
  void _0F7();
  void SED_IMP();
  void SBC_ABY();
  void _0FA();
  void _0FB();
  void _0FC();
  void SBC_ABX();
  void INC_ABX();
  void _0FF();
  void _100();
  void _101();
  void _102();
};