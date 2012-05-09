// e.g. Rockman 2, Simon's Quest
class MMC1 : public GamePak::MemoryMapper {
 public:
  static const int number = 1;
  MMC1(GamePak& gamepak) : GamePak::MemoryMapper(gamepak) {
  }
  void Initialize() {
    gamepak.SetVROM(0x2000, 0x0000, 0);
    //for(unsigned v=0; v<4; ++v) {
      //gamepak.SetROM(0x4000, v*0x4000, v==3 ? -1 : 0);
    //}
    //gamepak.Switch16kPrgRom(0,0);
    //gamepak.Switch16kPrgRom((gamepak.nes().rom16count()-1)*4,1);


    gamepak.SwitchPrgRom(0,0x8000,16);
    gamepak.SwitchPrgRom(gamepak.prg_count()-1,0xC000,16);

    regs[0]=0x0C;
    regs[1]=0;
    regs[2]=0;
    regs[3]=0;
    counter=0; 
    cache=0;
  }

  void Write(uint16_t address,uint8_t data) {

    if(data & 0x80) { 
      regs[0]=0x0E; 
      //goto configure; 
    } else {
      cache |= (data&1) << counter;
      if(++counter == 5) {
          regs[ (address>>13) & 3 ] = data = cache;
      //configure:
          cache = counter = 0;
          const uint8_t sel[4][4] = { {0,0,0,0}, {1,1,1,1}, {0,1,0,1}, {0,0,1,1} };
          for(unsigned m=0; m<4; ++m) 
            gamepak.Nta[m] = &gamepak.NRAM[0x400 * sel[regs[0]&3][m]];
          
          
          gamepak.SetVROM(0x1000, 0x0000, ((regs[0]&16) ? regs[1] : ((regs[1]&~1)+0)));
          gamepak.SetVROM(0x1000, 0x1000, ((regs[0]&16) ? regs[2] : ((regs[1]&~1)+1)));

          if (gamepak.has_vrom == true) {
            if ((regs[0]&16)) {
              gamepak.SetVROM(0x1000, 0x0000, regs[1]);
              gamepak.SetVROM(0x1000, 0x1000, regs[2]);
              //gamepak.SetVROM(0x1000, 0x1000, regs[2]&0x0F);
            } else {
              gamepak.SetVROM(0x2000, 0x0000, regs[1]);
            }
          }

          switch( (regs[0]>>2)&3 ) {
              case 0: case 1:
                  //gamepak.SetROM(0x8000, 0x8000, (regs[3] & 0xE) / 2);
                  gamepak.SwitchPrgRom((regs[3] & 0xF),0x8000,32);
                  break;
              case 2:
                  gamepak.SwitchPrgRom(0,0x8000,16);
                  gamepak.SwitchPrgRom((regs[3] & 0xF),0xC000,16);
                  //gamepak.SetROM(0x4000, 0x8000, 0);
                  //gamepak.SetROM(0x4000, 0xC000, (regs[3] & 0xF));
                  break;
              case 3:
                  //gamepak.SetROM(0x4000, 0x8000, (regs[3] & 0xF));
                  //gamepak.SetROM(0x4000, 0xC000, ~0);

                  gamepak.SwitchPrgRom((regs[3] & 0xF),0x8000,16);
                  gamepak.SwitchPrgRom(gamepak.prg_count()-1,0xC000,16);
                 
                  break;
          }
      }
    }
  }

  void CpuTick() {

  }
  void PpuTick() {

  }
 private:
  uint8_t regs[4], counter, cache;
};