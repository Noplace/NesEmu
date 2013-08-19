
class Ppu2 : public Component /* Picture Processing Unit */
{
 public:
  Ppu2();
  ~Ppu2();
  int Initialize(Nes* nes);
  void Power();
  void Reset();
  uint8_t Read(uint16_t address);
  void Write(uint16_t address,uint8_t data);
  void Tick();
  void OnSettingsChanged();
  //bool offset_toggle() { return offset_toggle_; }
  //int open_bus() { return open_bus_; }
  //uint16_t address() { return vaddr.raw; }
 protected:
  typedef void (Ppu2::*TickFunction)();
  TickFunction tick_array[3];
  uint16_t scanline;
  uint16_t pp_cycle;
  uint16_t vram_address;
  uint8_t vram_data_latch;
  union {
    struct {
      uint8_t name_table_scroll_address:2;
      uint8_t vram_address_increment:1;
      uint8_t pattern_table_sprite_address:1;
      uint8_t pattern_table_background_address:1;
      uint8_t sprite_size:1;
      uint8_t master_slave_selection:1;
      uint8_t execute_nmi_on_vblank:1;
    };
    uint8_t raw;
  } ctrl_reg1;
  union {
    struct {
      uint8_t monochrome_mode:1;
      uint8_t background_clipping:1;
      uint8_t sprite_clipping:1;
      uint8_t background_visiblity:1;
      uint8_t sprite_visiblity:1;
      uint8_t color_emphasis:3;
    };
    uint8_t raw;
  } ctrl_reg2;
  union {
    struct {
      uint8_t unused:5;
      uint8_t lost_sprite:1;
      uint8_t sprite_0_hit:1;
      uint8_t vblank_flag:1;
    };
    uint8_t raw;
  } status_reg;
  uint8_t reload_bits;
  uint8_t spr_ram_address;
  uint8_t spr_ram_data;
  uint8_t sprite_dma;
  uint8_t spr_ram[256];
  uint8_t palette[32];
  uint8_t horizontal_scroll_origin;
  uint8_t vertical_scroll_origin;
  bool flipflip;

  uint8_t& MemoryMap(uint16_t address);
  void TickNTSC();
  void TickPAL();
};