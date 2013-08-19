
class MMC3 : public GamePak::MemoryMapper {
 public:
  static const int number = 4;
  MMC3(GamePak& gamepak) : GamePak::MemoryMapper(gamepak) {
  }
  void Initialize() {
    gamepak.SetVROM(0x2000, 0x0000, 0);

    gamepak.SwitchPrgRom(0,0x8000,16);
    gamepak.SwitchPrgRom(gamepak.prg_count()-1,0xC000,16);
    ppu = &gamepak.nes().ppu();
    cpu = &gamepak.nes().cpu();
    reg1 = 0;
    irq_counter = 0;
    //irq_latch_reg = 0xFF;
    irq_latch_reg = 0xFF;
    last_ppu_addr = 0;
    a12_low_count = 0;
    save_ram = false;
    irq_enabled = false;
    //irq_reload = false;
    inside_tick = false;
    mmc3_alt_behavior = false;
    last_rise_time = -17; //so it would start from beginning
    timer = 0;//36;
  }

  void Write(uint16_t address,uint8_t data) {
    #ifdef _DEBUG
    {
      char str[255];
      sprintf(str,"MMC3 Write - 0x%04x - 0x%02x\n",address,data);
      OutputDebugString(str);
    }
    #endif
    bool odd = address & 0x1;
    int reg = (address - 0x8000) >> 13;


    if (reg == 0 && odd == false) { //(address == 0x8000) {
      reg1 = data;
    }

    if (reg == 0 && odd == true) { //(address == 0x8001) {
      ExecuteCommand(data);
    }

    if (reg == 1 && odd == false) { //(address == 0xA000) {
      mirriong = (data&0x01);
      const uint8_t sel[2][4] = { {0,1,0,1}, {0,0,2,2} };
      for(unsigned m=0; m<4; ++m) 
        gamepak.Nta[m] = &gamepak.NRAM[0x400 * sel[mirriong][m]];
      //for(unsigned m=0; m<4; ++m)
      //  Nta[m] = &NRAM[0x400 * (m & ((reg2&1) ? 2 : 1))];
    }

    if (reg == 1 && odd == true) { //(address == 0xA001) {
      save_ram = (data&0x80)>>7;
    }

    if (reg == 2 && odd == false) { //(address == 0xC000) {
      //irq_latch_reg = data;
      irq_latch_reg = data;
    }

    if (reg == 2 && odd == true) { //(address == 0xC001) {
      irq_counter = 0;
      if ( mmc3_alt_behavior ) 
            irq_reload = true;
      //irq_reload = true;
    }

    if (reg == 3 && odd == false) { //(address == 0xE000) {
      //irq_counter = irq_latch_reg;
      //disable irq
      irq_enabled = false;
      //gamepak.nes().cpu().RaiseIRQLine();
      cpu->LowerIRQLine();
    }

    if (reg == 3 && odd == true) { //(address == 0xE001) {
      //enable irq
      irq_enabled = true;
    }

  }

  void ExecuteCommand(uint8_t data) {
    unsigned const vrom_select_table[6][3] = {
      {0x0000,0x1000,0x800},
      {0x0800,0x1800,0x800},
      {0x1000,0x0000,0x400},
      {0x1400,0x0400,0x400},
      {0x1800,0x0800,0x400},
      {0x1C00,0x0C00,0x400},
    };
    unsigned const prg_select_table[2][3] = {
      {0x8000,0xC000,8},
      {0xA000,0xA000,8},
    };
    if (reg1.cmd >=0 && reg1.cmd<6)
      gamepak.SetVROM(vrom_select_table[reg1.cmd][2],vrom_select_table[reg1.cmd][reg1.chr_addr_select],data);
    else
      gamepak.SwitchPrgRom(data,prg_select_table[reg1.cmd-6][reg1.prg_addr_select],prg_select_table[reg1.cmd-6][2]);
    if (reg1.prg_addr_select == 0)  { 
      gamepak.SwitchPrgRom(((gamepak.prg_count()) - 2) , 0xC000,8); 
    } else {
      gamepak.SwitchPrgRom(((gamepak.prg_count()) - 2) , 0x8000,8); 
    }
    gamepak.SwitchPrgRom(((gamepak.prg_count()) - 1) , 0xE000,8);
  }

  void CpuTick() {

  }
  void PpuTick() {
    uint32_t address = gamepak.nes().ppu().address();
    unsigned old_a12 = (last_ppu_addr & 0x1000)>>12; 
    unsigned new_a12 = (address & 0x1000)>>12; 
    last_ppu_addr = address;
    static int tcounter  =0;
    if (old_a12 == 0 && new_a12 == 1) {
        /*#ifdef _DEBUG
        {
          char str[255];
          sprintf(str,"MMC3 toggle count %d\n",timer);
          OutputDebugString(str);
        }
        #endif*/
      if (timer == 0) {
        auto old = irq_counter;
        if (irq_counter == 0 || irq_reload)
          irq_counter = irq_latch_reg;
        else
          --irq_counter;
        
        if ( (!mmc3_alt_behavior && old != 0 || irq_reload) && irq_counter == 0 && irq_enabled ) {
            cpu->RaiseIRQLine();
        }

        irq_reload = false;
        timer = 16;
      }
      --timer;  
    }
    /*uint32_t address = gamepak.nes().ppu().address();
    unsigned old_a12 = (last_ppu_addr & 0x1000)>>12; 
    unsigned new_a12 = (address & 0x1000)>>12; 
    static int tcounter  =0;
    if (old_a12 == 0 && new_a12 == 1) {
        #ifdef _DEBUG
        {
          char str[255];
          sprintf(str,"MMC3 toggle count %d\n",timer);
          OutputDebugString(str);
        }
        #endif
      if ((timer)>35) {
        auto old = irq_counter;
        if (irq_counter == 0 || irq_reload)
          irq_counter = irq_latch_reg;
        else
          --irq_counter;
        
        if ( (!mmc3_alt_behavior && old != 0 || irq_reload) && irq_counter == 0 && irq_enabled ) {
            cpu->RaiseIRQLine();
        }

        irq_reload = false;
        timer = 0;
        tcounter++;
        /*#ifdef _DEBUG
        {
          char str[255];
          sprintf(str,"MMC3 scanline count %d\n",tcounter);
          OutputDebugString(str);
        }
        #endif*/
     /* }
      ++timer;  
    }*/
    //last_ppu_addr = address;
    /*uint32_t address = gamepak.nes().ppu().address();
    unsigned old_a12 = (last_ppu_addr & 0x1000)>>12; 
    unsigned new_a12 = (address & 0x1000)>>12; 
    //if (old_a12 == 0 && new_a12==0)
    //  ++timer;

    if (old_a12 == 0 && new_a12==1) {
      //if (timer > 114) {
        if (irq_counter == 0) {
          irq_reload = false;
          irq_counter = irq_latch_reg;
        } else {
          --irq_counter;
        }
    
        if (irq_counter == 0 && irq_enabled == true) {
          cpu->RaiseIRQLine();
          //cpu->InduceIRQ();
        }
      //}
      ++timer;
     }
     last_ppu_addr = address;*/
        /*
    address = gamepak.nes().ppu().address();
    unsigned old_a12 = last_ppu_addr & 0x1000; 
    unsigned new_a12 = address & 0x1000; 
    ++timer;
    if (old_a12^new_a12) {
      if (timer > 16) {
        if (irq_counter == 0) {
          irq_reload = false;
          irq_counter = irq_latch_reg;
        } else {
          --irq_counter;
        }
    
        if (irq_counter == 0 && irq_enabled == true) {
          cpu->RaiseIRQLine();
        }
      }
      timer = 0;
    }
    last_ppu_addr = address;*/
  }
 private:
   Ppu* ppu;
   Cpu* cpu;
   int last_rise_time;
   int timer;
   uint16_t last_ppu_addr;
   uint16_t a12_low_count;
   union {
     struct {
         unsigned cmd:3;
         unsigned xxx:3;
         unsigned prg_addr_select:1;
         unsigned chr_addr_select:1;
      };
     uint8_t data;
     uint8_t operator =(uint8_t data) {
       return this->data = data;
     }
   } reg1;
   uint8_t mirriong;//0 H , 1 V 
   uint8_t irq_counter;
   uint8_t irq_latch_reg;
   bool irq_reload;
   bool save_ram;
   bool irq_enabled;
   bool inside_tick;
   bool mmc3_alt_behavior;
};
