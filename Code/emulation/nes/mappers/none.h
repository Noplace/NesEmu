class NoMapper : public GamePak::MemoryMapper {
 public:
  static const int number = 0;
  NoMapper(GamePak& gamepak) : GamePak::MemoryMapper(gamepak) {
  }
  void Initialize() {
    gamepak.SetVROM(0x2000, 0x0000, 0);
    //for(unsigned v=0; v<4; ++v) {
    //  gamepak.SetROM(0x4000, v*0x4000, v==3 ? -1 : 0);
    //}

    if (gamepak.prg_count() == 1)
    {
        //gamepak.Switch16kPrgRom(0, 1);
       //Switch8kChrRom(0);
      int a = 1;
    }

    gamepak.SwitchPrgRom(0,0x8000,16);
    gamepak.SwitchPrgRom(gamepak.prg_count()-1,0xC000,16);
  }

  void Write(uint16_t address,uint8_t data) {

  }


  void CpuTick() {

  }
  void PpuTick() {

  }
};