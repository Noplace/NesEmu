class GamePak : public Component
{
 public:
  
  class MemoryMapper {
   public:
    MemoryMapper(GamePak& gamepak) : gamepak(gamepak) {}
    ~MemoryMapper() {}
    virtual void Initialize() = 0;
    virtual void Write(uint16_t address,uint8_t data) = 0;
    virtual void CpuTick() = 0;
    virtual void PpuTick() = 0;
   protected:
    GamePak& gamepak;
  };

  static const unsigned VROM_Granularity = 0x0400, VROM_Pages = 0x2000 / VROM_Granularity;

  uint8_t* chr_banks[VROM_Pages];  
  uint8_t* prg_banks[16];
  uint8_t* Nta[4];
  uint8_t* ROM;
  uint8_t* VROM;
  uint8_t* NRAM;
  uint8_t* PRAM;
  MemoryMapper* mapper;
  uint32_t rom_size;
  uint32_t vrom_size;
  bool has_vrom;
  
  GamePak();
  ~GamePak();
  int InitializeFromFile(Nes* nes,const char* filename);
  int Deinitialize();

  //void SetROM(unsigned size, unsigned baseaddr, unsigned index);
  void SwitchPrgRom(int prg_rom_index, int baseaddress,int size_kb);
  void SetVROM(unsigned size, unsigned baseaddr, unsigned index);
  uint8_t Read(uint16_t addr);
  void Write(uint16_t addr, uint8_t value);
  uint8_t prg_count() { return prg_count_; }
  uint8_t chr_count() { return chr_count_; }
  uint8_t ctrlbyte() { return ctrlbyte_; }
  uint8_t mappernum() { return mappernum_; }
 protected:
  uint8_t prg_count_;
  uint8_t chr_count_;
  uint8_t ctrlbyte_;
  uint8_t mappernum_;
  int InternalInitialize();
 private:
  int Initialize(Nes* nes) {
    nes_ = nes;
    return S_OK;
  }
};