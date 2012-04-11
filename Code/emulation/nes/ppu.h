
class Ppu : public Component /* Picture Processing Unit */
{
 public:
  union regtype // PPU register file
  {
      uint32_t value;
      // Reg0 (write)             // Reg1 (write)             // Reg2 (read)
      RegisterBit<0,8,uint32_t> sysctrl;    RegisterBit< 8,8,uint32_t> dispctrl;  RegisterBit<16,8,uint32_t> status;
      RegisterBit<0,2,uint32_t> BaseNTA;    RegisterBit< 8,1,uint32_t> Grayscale; RegisterBit<21,1,uint32_t> SPoverflow;
      RegisterBit<2,1,uint32_t> Inc;        RegisterBit< 9,1,uint32_t> ShowBG8;   RegisterBit<22,1,uint32_t> SP0hit;
      RegisterBit<3,1,uint32_t> SPaddr;     RegisterBit<10,1,uint32_t> ShowSP8;   RegisterBit<23,1,uint32_t> InVBlank;
      RegisterBit<4,1,uint32_t> BGaddr;     RegisterBit<11,1,uint32_t> ShowBG;    // Reg3 (write)
      RegisterBit<5,1,uint32_t> SPsize;     RegisterBit<12,1,uint32_t> ShowSP;    RegisterBit<24,8,uint32_t> OAMaddr;
      RegisterBit<6,1,uint32_t> SlaveFlag;  RegisterBit<11,2,uint32_t> ShowBGSP;  RegisterBit<24,2,uint32_t> OAMdata;
      RegisterBit<7,1,uint32_t> NMIenabled; RegisterBit<13,3,uint32_t> EmpRGB;    RegisterBit<26,6,uint32_t> OAMindex;
  } reg;
  
  Ppu();
  ~Ppu();
  int Initialize(Nes* nes);
  void Power();
  void Reset();
  uint8_t Read(uint16_t address);
  void Write(uint16_t address,uint8_t data);
  void Tick();
  void OnSettingsChanged();
  bool offset_toggle() { return offset_toggle_; }
  int open_bus() { return open_bus_; }
  uint16_t address() { return vaddr.raw; }
 protected:
  typedef void (Ppu::*TickFunction)();
  TickFunction tick_array[3];
  // Raw memory data as read&written by the game
  uint8_t palette[32], OAM[256];
  // Decoded sprite information, used & changed during each scanline
  struct { uint8_t sprindex, y, index, attr, x; uint16_t pattern; } OAM2[8], OAM3[8];

  union scrolltype
  {
      RegisterBit<3,16,uint32_t> raw;       // raw VRAM address (16-bit)
      RegisterBit<0, 8,uint32_t> xscroll;   // low 8 bits of first write to 2005
      RegisterBit<0, 3,uint32_t> xfine;     // low 3 bits of first write to 2005
      RegisterBit<3, 5,uint32_t> xcoarse;   // high 5 bits of first write to 2005
      RegisterBit<8, 5,uint32_t> ycoarse;   // high 5 bits of second write to 2005
      RegisterBit<13,2,uint32_t> basenta;   // nametable index (copied from 2000)
      RegisterBit<13,1,uint32_t> basenta_h; // horizontal nametable index
      RegisterBit<14,1,uint32_t> basenta_v; // vertical   nametable index
      RegisterBit<15,3,uint32_t> yfine;     // low 3 bits of second write to 2005
      RegisterBit<11,8,uint32_t> vaddrhi;   // first write to 2006 (with high 2 bits set to zero)
      RegisterBit<3, 8,uint32_t> vaddrlo;   // second write to 2006
  } scroll, vaddr;

  unsigned pat_addr, sprinpos, sproutpos, sprrenpos, sprtmp;
  uint16_t tileattr, tilepat, ioaddr;
  uint32_t bg_shift_pat, bg_shift_attr;

  int scanline, x, scanline_end, VBlankState, cycle_counter;
  int read_buffer, open_bus_, open_bus_decay_timer;
  bool even_odd_toggle, offset_toggle_;

  uint64_t cycles;
  /* Memory mapping: Convert PPU memory address into a reference to relevant data */
  uint8_t& mmap(int i);
  void rendering_tick();
  void render_pixel();
  void TickNTSC();
  void TickPAL();
};