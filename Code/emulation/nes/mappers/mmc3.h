
class MMC3 : public GamePak::MemoryMapper {
 public:
  static const int number = 1;
  MMC3(GamePak& gamepak) : GamePak::MemoryMapper(gamepak) {
  }
  void Initialize() {
    gamepak.SetVROM(0x2000, 0x0000, 0);

    gamepak.SwitchPrgRom(0,0x8000,16);
    gamepak.SwitchPrgRom(gamepak.prg_count()-1,0xC000,16);
    ppu = &gamepak.nes().ppu();
    cpu = &gamepak.nes().cpu();
    reg1 = 0;
    irq_counter = 0xFF;
    //irq_latch_reg = 0xFF;
    irq_refresh = 0xFF;
    last_ppu_addr = 0;
    a12_low_count = 0;
    save_ram = false;
    irq_enabled = false;
    inside_tick = false;
    timer = 0;
  }

  void Write(uint16_t address,uint8_t data) {
    #ifdef _DEBUG
    {
      char str[255];
      sprintf(str,"MMC3 Write - 0x%04x - 0x%02x\n",address,data);
      OutputDebugString(str);
    }
    #endif
    if (address == 0x8000) {
      reg1 = data;
    }

    if (address == 0x8001) {
      ExecuteCommand(data);
    }

     if (address == 0xA000) {
      mirriong = (data&0x01);
      const uint8_t sel[2][4] = { {0,1,0,1}, {0,0,2,2} };
      for(unsigned m=0; m<4; ++m) 
        gamepak.Nta[m] = &gamepak.NRAM[0x400 * sel[mirriong][m]];
      //for(unsigned m=0; m<4; ++m)
      //  Nta[m] = &NRAM[0x400 * (m & ((reg2&1) ? 2 : 1))];
    }

    if (address == 0xA001) {
      save_ram = (data&0x80)>>7;
    }

    if (address == 0xC000) {
      //irq_latch_reg = data;
      irq_refresh = data;
    }

    if (address == 0xC001) {
      irq_latch_reg = data;
      irq_reload = true;
    }

    if (address == 0xE000) {
      //irq_counter = irq_latch_reg;
      //disable irq
      irq_enabled = false;
      gamepak.nes().cpu().RaiseIRQLine();
    }

    if (address == 0xE001) {
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
    //gamepak.SetVROM(vrom_select_table[reg1.cmd][2],vrom_select_table[reg1.cmd][reg1.chr_addr_select],data);
    switch (reg1.cmd) {
      case 0:
          data = (uint8_t)(data - (data % 2));
          if (reg1.chr_addr_select == 0)
              //Map.Switch2kChrRom(data, 0);
              gamepak.SetVROM(0x800,0x0000,data);
          else
              gamepak.SetVROM(0x800,0x1000,data);
          break;
      case 1:
          data = (uint8_t)(data - (data % 2));
          if (reg1.chr_addr_select == 0)
          {
              //Map.Switch2kChrRom(data, 1);
              gamepak.SetVROM(0x800,0x0800,data);
          }
          else
          {
              //Map.Switch2kChrRom(data, 3);
              gamepak.SetVROM(0x800, 0x1800,data);
          } break;
      case 2:
          data = (uint8_t)(data & ((gamepak.chr_count() * 8) - 1));
          if (reg1.chr_addr_select == 0)
          {
              //Map.Switch1kChrRom(data, 4);
              gamepak.SetVROM(0x400, 0x1000,data);
          }
          else
          {
              //Map.Switch1kChrRom(data, 0);
              gamepak.SetVROM(0x400,0x000,data);
          } break;
      case 3:
          if (reg1.chr_addr_select == 0)
          {
              //Map.Switch1kChrRom(data, 5);
              gamepak.SetVROM(0x400,0x1400, data);
          }
          else
          {
              //Map.Switch1kChrRom(data, 1);
              gamepak.SetVROM(0x400,0x0400, data);
          } break;
      case 4:
          if (reg1.chr_addr_select == 0)
          {
              //Map.Switch1kChrRom(data, 6);
              gamepak.SetVROM(0x400,0x1800, data);
          }
          else
          {
              //Map.Switch1kChrRom(data, 2);
            gamepak.SetVROM(0x400,0x0800,data);
          } break;
      case 5:
          if (reg1.chr_addr_select == 0)
          {
              //Map.Switch1kChrRom(data, 7);
              gamepak.SetVROM(0x400,0x1C00,data);
          }
          else
          {
              //Map.Switch1kChrRom(data, 3);
              gamepak.SetVROM(0x400,0x0C00, data);
          } break;
      case 6:
          if (reg1.prg_addr_select == 0)
          {
              //Map.Switch8kPrgRom(data * 2, 0);
              gamepak.SwitchPrgRom(data,0x8000,8);
          }
          else
          {
              //Map.Switch8kPrgRom(data * 2, 2);
              gamepak.SwitchPrgRom(data,0xC000,8);
          }
          break;
      case 7:
          //Map.Switch8kPrgRom(data * 2, 1);
          gamepak.SwitchPrgRom(data,0xA000,8);
          break;
  }
  if (reg1.prg_addr_select == 0)  { 
    gamepak.SwitchPrgRom(((gamepak.prg_count()) - 2) , 0xC000,8); 
  }
  else  {
    gamepak.SwitchPrgRom(((gamepak.prg_count()) - 2) , 0x8000,8); 
  }
  gamepak.SwitchPrgRom(((gamepak.prg_count()) - 1) , 0xE000,8);
  }

  void Tick(uint32_t address) {

    address = gamepak.nes().ppu().address();
    unsigned old_a12 = last_ppu_addr & 0x1000; 
    unsigned new_a12 = address & 0x1000; 
    ++timer;
    if (old_a12^new_a12) {
      if (timer > 16) {
        if (irq_counter == 0) {
          irq_reload = false;
          irq_counter = irq_refresh;
        } else {
          --irq_counter;
        }
    
        if (irq_counter == 0 && irq_enabled == true) {
          cpu->RaiseIRQLine();
        }
      }
      timer = 0;
    }
    last_ppu_addr = address;
  }
 private:
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
   uint8_t irq_refresh;
   uint8_t irq_latch_reg;
   uint16_t last_ppu_addr;
   uint16_t a12_low_count;
   int timer;
   bool irq_reload;
   bool save_ram;
   bool irq_enabled;
   bool inside_tick;

   Ppu* ppu;
   Cpu* cpu;
};
