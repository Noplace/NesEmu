
 // Rare games
class AOROM : public GamePak::MemoryMapper {
 public:
  static const int number = 1;
  AOROM(GamePak& gamepak) : GamePak::MemoryMapper(gamepak) {
  }
  void Initialize() {
    gamepak.SetVROM(0x2000, 0x0000, 0);
    gamepak.SwitchPrgRom(0,0x8000,32);
  }

  void Write(uint16_t address,uint8_t data) {
      gamepak.SwitchPrgRom((data&7),0x8000,32);
      gamepak.Nta[0] = gamepak.Nta[1] = gamepak.Nta[2] =gamepak. Nta[3] = &gamepak.NRAM[0x400 * ((data>>4)&1)];
    //
  }

  void CpuTick() {

  }
  void PpuTick() {

  }
 private:


};