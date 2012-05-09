



 // e.g. Kage, Solomon's Key
class CNROM : public GamePak::MemoryMapper {
 public:
  static const int number = 1;
  CNROM(GamePak& gamepak) : GamePak::MemoryMapper(gamepak) {
  }
  void Initialize() {
    gamepak.SetVROM(0x2000, 0x0000, 0);
    gamepak.SwitchPrgRom(0,0x8000,16);
    gamepak.SwitchPrgRom((gamepak.prg_count()-1),0xC000,16);
  }

  void Write(uint16_t address,uint8_t data) {
    data &= gamepak.Read(address); // Simulate bus conflict
    gamepak.SetVROM(0x2000, 0x0000, (data&3));
    //gamepak.SwitchPrgRom(data,0x8000,16);
  }

  void CpuTick() {

  }
  void PpuTick() {

  }
 private:


};
