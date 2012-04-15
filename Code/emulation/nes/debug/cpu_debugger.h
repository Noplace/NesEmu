
class CpuDebugger {
 public:
  char debug_line[256];
  char inst_line[25];
  uint16_t inst_pc;
  void HookBeforeCpuStep(Cpu* cpu) {
    inst_pc = cpu->PC-1;
    char inst_str[6];
    switch (cpu->op) {
  
      case 0x00: sprintf(inst_str,"brk");    break;
      case 0x01: sprintf(inst_str,"ora"); break;
      case 0x05: sprintf(inst_str,"ora");  break;
      case 0x06: sprintf(inst_str,"asl");  break;
      case 0x08: sprintf(inst_str,"php");   break; 
      case 0x09: sprintf(inst_str,"ora"); break;    
      case 0x0A: sprintf(inst_str,"asl"); break;
      case 0x0D: sprintf(inst_str,"ora");  break;
      case 0x0E: sprintf(inst_str,"asl");  break;
    
      case 0x10: sprintf(inst_str,"bpl");  break;
      case 0x11: sprintf(inst_str,"ora"); break;
      case 0x15: sprintf(inst_str,"ora"); break;
      case 0x16: sprintf(inst_str,"asl"); break;
      case 0x18: sprintf(inst_str,"clc");   break;
      case 0x19: sprintf(inst_str,"ora"); break;
      case 0x1D: sprintf(inst_str,"ora"); break;
      case 0x1E: sprintf(inst_str,"asl"); break;
    
      case 0x20: sprintf(inst_str,"jsr");  break; 
      case 0x21: sprintf(inst_str,"and"); break;
      case 0x24: sprintf(inst_str,"bit");  break;
      case 0x25: sprintf(inst_str,"and");  break;
      case 0x26: sprintf(inst_str,"rol");  break;
      case 0x28: sprintf(inst_str,"plp");   break; 
      case 0x29: sprintf(inst_str,"and"); break;
      case 0x2A: sprintf(inst_str,"rol"); break;
      case 0x2C: sprintf(inst_str,"bit");  break;
      case 0x2D: sprintf(inst_str,"and");  break;
      case 0x2E: sprintf(inst_str,"rol");  break;
    
      case 0x30: sprintf(inst_str,"bmi");  break;
      case 0x31: sprintf(inst_str,"and"); break;    
      case 0x35: sprintf(inst_str,"and"); break;
      case 0x36: sprintf(inst_str,"rol"); break;
      case 0x38: sprintf(inst_str,"sec");   break;
      case 0x39: sprintf(inst_str,"and"); break;
      case 0x3D: sprintf(inst_str,"and"); break;
      case 0x3E: sprintf(inst_str,"rol"); break;
     
      case 0x40: sprintf(inst_str,"rti");   break; 
      case 0x41: sprintf(inst_str,"eor"); break;
      case 0x45: sprintf(inst_str,"eor");  break;
      case 0x46: sprintf(inst_str,"lsr");  break;
      case 0x48: sprintf(inst_str,"pha");   break;
      case 0x49: sprintf(inst_str,"eor"); break;
      case 0x4A: sprintf(inst_str,"lsr"); break;
      case 0x4C: sprintf(inst_str,"jmp");  break;
      case 0x4D: sprintf(inst_str,"eor");  break;
      case 0x4E: sprintf(inst_str,"lsr");  break;
    
      case 0x50: sprintf(inst_str,"bvc");  break;
      case 0x51: sprintf(inst_str,"eor"); break;  
      case 0x55: sprintf(inst_str,"eor"); break;
      case 0x56: sprintf(inst_str,"lsr"); break;
      case 0x58: sprintf(inst_str,"cli");   break;
      case 0x59: sprintf(inst_str,"eor"); break;
      case 0x5D: sprintf(inst_str,"eor"); break;
      case 0x5E: sprintf(inst_str,"lsr"); break;    
    
      case 0x60: sprintf(inst_str,"rts");   break; 
      case 0x61: sprintf(inst_str,"adc"); break;
      case 0x65: sprintf(inst_str,"adc");  break;
      case 0x66: sprintf(inst_str,"ror");  break;
      case 0x68: sprintf(inst_str,"pla");   break; 
      case 0x69: sprintf(inst_str,"adc"); break;
      case 0x6A: sprintf(inst_str,"ror"); break;
      case 0x6C: sprintf(inst_str,"jmp");  break;
      case 0x6D: sprintf(inst_str,"adc");  break;
      case 0x6E: sprintf(inst_str,"ror");  break;
    
      case 0x70: sprintf(inst_str,"bvs");  break;
      case 0x71: sprintf(inst_str,"adc"); break; 
      case 0x75: sprintf(inst_str,"adc"); break;
      case 0x76: sprintf(inst_str,"ror"); break;
      case 0x78: sprintf(inst_str,"sei");   break;
      case 0x79: sprintf(inst_str,"adc"); break;
      case 0x7D: sprintf(inst_str,"adc"); break;
      case 0x7E: sprintf(inst_str,"ror"); break;    
    
 
      case 0x81: sprintf(inst_str,"sta"); break; 
      case 0x84: sprintf(inst_str,"sty");  break; 
      case 0x85: sprintf(inst_str,"sta");  break; 
      case 0x86: sprintf(inst_str,"stx");  break; 
      case 0x88: sprintf(inst_str,"dey");   break;
      case 0x8A: sprintf(inst_str,"txa");   break;  
      case 0x8C: sprintf(inst_str,"sty"); break;    
      case 0x8D: sprintf(inst_str,"sta");  break; 
      case 0x8E: sprintf(inst_str,"stx");  break;
    
      case 0x90: sprintf(inst_str,"bcc");  break;
      case 0x91: sprintf(inst_str,"sta"); break; 
      case 0x94: sprintf(inst_str,"sty"); break;
      case 0x95: sprintf(inst_str,"sta"); break;  
      case 0x96: sprintf(inst_str,"stx"); break; 
      case 0x98: sprintf(inst_str,"tya");   break;  
      case 0x99: sprintf(inst_str,"sta"); break;     
      case 0x9A: sprintf(inst_str,"txs");   break; 
      case 0x9D: sprintf(inst_str,"sta"); break;     
   
      case 0xA0: sprintf(inst_str,"ldy"); break;
      case 0xA1: sprintf(inst_str,"lda"); break;   
      case 0xA2: sprintf(inst_str,"ldx"); break;
      case 0xA4: sprintf(inst_str,"ldy");  break; 
      case 0xA5: sprintf(inst_str,"lda");  break;       
      case 0xA6: sprintf(inst_str,"ldx");  break;
      case 0xA8: sprintf(inst_str,"tay");   break;         
      case 0xA9: sprintf(inst_str,"lda"); break;      
      case 0xAA: sprintf(inst_str,"tax");   break; 
      case 0xAC: sprintf(inst_str,"ldy");  break; 
      case 0xAD: sprintf(inst_str,"lda");  break;   
      case 0xAE: sprintf(inst_str,"ldx");  break; 
    
      case 0xB0: sprintf(inst_str,"bcs");  break;
      case 0xB1: sprintf(inst_str,"lda"); break;       
      case 0xB4: sprintf(inst_str,"ldy"); break;  
      case 0xB5: sprintf(inst_str,"lda"); break; 
      case 0xB6: sprintf(inst_str,"ldx"); break;  
      case 0xB8: sprintf(inst_str,"clv");   break; 
      case 0xB9: sprintf(inst_str,"lda"); break;     
      case 0xBA: sprintf(inst_str,"tsx");   break;  
      case 0xBC: sprintf(inst_str,"ldy"); break; 
      case 0xBD: sprintf(inst_str,"lda"); break;       
      case 0xBE: sprintf(inst_str,"ldx"); break; 
    
      case 0xC0: sprintf(inst_str,"cpy"); break;
      case 0xC1: sprintf(inst_str,"cmp"); break;
      case 0xC4: sprintf(inst_str,"cpy");  break;    
      case 0xC5: sprintf(inst_str,"cmp");  break; 
      case 0xC6: sprintf(inst_str,"dec");  break;
      case 0xC8: sprintf(inst_str,"iny");   break;
      case 0xC9: sprintf(inst_str,"cmp"); break;
      case 0xCA: sprintf(inst_str,"dex");   break;
      case 0xCC: sprintf(inst_str,"cpy");  break;
      case 0xCD: sprintf(inst_str,"cmp");  break;  
      case 0xCE: sprintf(inst_str,"dec");  break;
    
      case 0xD0: sprintf(inst_str,"bne");  break;
      case 0xD1: sprintf(inst_str,"cmp"); break; 
      case 0xD5: sprintf(inst_str,"cmp"); break;
      case 0xD6: sprintf(inst_str,"dec"); break;
      case 0xD8: sprintf(inst_str,"cld");   break;    
      case 0xD9: sprintf(inst_str,"cmp"); break;     
      case 0xDD: sprintf(inst_str,"cmp"); break;   
      case 0xDE: sprintf(inst_str,"dec"); break;
   
      case 0xE0: sprintf(inst_str,"cpx"); break;
      case 0xE1: sprintf(inst_str,"sbc"); break;
      case 0xE4: sprintf(inst_str,"cpx");  break; 
      case 0xE5: sprintf(inst_str,"sbc");  break;
      case 0xE6: sprintf(inst_str,"inc");  break;
      case 0xE8: sprintf(inst_str,"inx");   break;
      case 0xE9: sprintf(inst_str,"sbc"); break;
      case 0xEA: sprintf(inst_str,"nop");   break;
      case 0xEC: sprintf(inst_str,"cpx");  break;
      case 0xED: sprintf(inst_str,"sbc");  break;
      case 0xEE: sprintf(inst_str,"inc");  break;
    
      case 0xF0: sprintf(inst_str,"beq");  break;
      case 0xF1: sprintf(inst_str,"sbc"); break; 
      case 0xF5: sprintf(inst_str,"sbc"); break;
      case 0xF6: sprintf(inst_str,"inc"); break;
      case 0xF8: sprintf(inst_str,"sed");   break;
      case 0xF9: sprintf(inst_str,"sbc"); break;
      case 0xFD: sprintf(inst_str,"sbc"); break;
      case 0xFE: sprintf(inst_str,"inc"); break;
      default:
        sprintf(inst_str,"_%03x",cpu->op);
        break;
    }
  
  
    sprintf(inst_line,"%04x - %s",inst_pc,inst_str);
  }

  void HookAfterCpuStep(Cpu* cpu) {
    char reg_dump[100];
    sprintf(reg_dump,"[A=%02x X=%02x Y=%02x PC=%04x P=%02x S=%02x][Cycles: Current=%02x (Total = %016x)]",cpu->A,cpu->X,cpu->Y,cpu->PC,cpu->P,cpu->S,cpu->current_inst_cycles,cpu->cycles);
  
  
  
    auto addressing_mode = Cpu::addressing_modes[cpu->op];
    char ops[32];
    unsigned addr2 = inst_pc+1;
    unsigned __int8 next_byte = 0xFF;
    if(addr2 >= 0 && addr2 < 0x1FFF) { 
      next_byte = cpu->RAM[addr2 & 0x7FF];
    }


    sprintf(ops,"");
    if (addressing_mode == kImmediate)
      sprintf(ops,"#%02x",next_byte);
    if (addressing_mode == kZeroPage || addressing_mode == kAbsolute)
      sprintf(ops,"%04x",cpu->address_bus);
    if (addressing_mode == kZeroPageX || addressing_mode == kAbsoluteX)
      sprintf(ops,"%04x,X",cpu->address_bus);    
    if (addressing_mode == kZeroPageY || addressing_mode == kAbsoluteY)
      sprintf(ops,"%04x,Y",cpu->address_bus);    
    
   if (addressing_mode == kIndirectX)
      sprintf(ops,"(%04x,X)",cpu->address_bus);    
   if (addressing_mode == kIndirectY)
      sprintf(ops,"(%04x),Y",cpu->address_bus);    
             
             
    if (addressing_mode == kRelative)
      sprintf(ops,"%02x",next_byte&0xff);  
  
    sprintf(debug_line,"%s\t%10s\t\t%s\n",inst_line,ops,reg_dump);
  }

};