#include "../nes.h"

Ppu2::Ppu2() : Component() {
  tick_array[0] = &Ppu2::TickNTSC;
  tick_array[1] = &Ppu2::TickPAL;
  tick_array[2] = nullptr;
}

Ppu2::~Ppu2() {

}


int Ppu2::Initialize(Nes* nes) {
  Component::Initialize(nes);
  memset(&spr_ram,0,sizeof(spr_ram));
  flipflip = false;
  reload_bits = 0;
  return S_OK;
}

static const uint8_t ppu2_arr[] =  {0x09,0x01,0x00,0x01,0x00,0x02,0x02,0x0D,0x08,0x10,0x08,0x24,0x00,0x00,0x04,0x2C,
    0x09,0x01,0x34,0x03,0x00,0x04,0x00,0x14,0x08,0x3A,0x00,0x02,0x00,0x20,0x2C,0x08};

void Ppu2::Power() {
  reload_bits = 0;
  flipflip = false;
  vram_data_latch = 0;
  memcpy(palette,ppu2_arr,32);
  memset(nes_->gamepak().NRAM,0x00,0x1000);
  scanline=0;
  pp_cycle=0;
}

void Ppu2::Reset() {
  reload_bits = 0;
  flipflip = false;
  vram_data_latch = 0;
  memcpy(palette,ppu2_arr,32);
  memset(nes_->gamepak().NRAM,0x00,0x1000);
  scanline=0;
  pp_cycle=0;
}

uint8_t& Ppu2::MemoryMap(uint16_t address)  {
  address &= 0x3FFF;
  if(address >= 0x3F00) { if(address%4==0) address &= 0x0F; return palette[address & 0x1F]; }

  if(address < 0x2000) 
    return nes_->gamepak().chr_banks[address>>10][address&0x3FF];
  return nes_->gamepak().Nta[(address>>10)&3][address&0x3FF];
}

uint8_t Ppu2::Read(uint16_t address) {
  uint8_t result;

  switch (address) {
    case 0x2002:
      result = status_reg.raw;
      flipflip = false;
      status_reg.vblank_flag = 0;
      break;
    case 0x2004:
      result = spr_ram_data = spr_ram[spr_ram_address];
      break;
    
    case 0x2007: {
      result = vram_data_latch;
      auto& mem_location = MemoryMap(vram_address);
      if((vram_address & 0x3F00) == 0x3F00) { // palette?
        result = mem_location;
        vram_data_latch = MemoryMap(vram_address & 0x2FFF);
      } else {
        vram_data_latch = mem_location;
      }
      if (ctrl_reg1.vram_address_increment == 1)
        vram_address += 32;
      else
        ++vram_address;
      break;
    }  
  }
  return result;
}

void Ppu2::Write(uint16_t address,uint8_t data) {

  switch (address) {
    case 0x2000:
      ctrl_reg1.raw = data;
      break;
    case 0x2001:
      ctrl_reg2.raw = data;
      break;
    case 0x2003:
      spr_ram_address = data;
      break;
    case 0x2004:
      spr_ram[spr_ram_address++] = spr_ram_data;
      break;
    case 0x2005:
      if (flipflip == false)
        horizontal_scroll_origin = reload_bits = data;
      else
        vertical_scroll_origin = reload_bits = data;
      flipflip = !flipflip;
      break;
    case 0x2006: {
      if (flipflip == false)
        reload_bits = (data&0x3F);
      else
        vram_address = (reload_bits<<8)|data;
      flipflip = !flipflip;
      break;
    }
    case 0x2007: {
      auto& mem_location = MemoryMap(vram_address);
      mem_location = data;
      if (ctrl_reg1.vram_address_increment == 1)
        vram_address += 32;
      else
        ++vram_address;
      break;
    }         
  }

}



void Ppu2::Tick() {
  (this->*(tick_array[mode_]))();
}

void Ppu2::TickNTSC() {
  for (auto i=0;i<3;++i) {

    if (scanline >= 0 && scanline <= 19) {
      nes_->cpu().nmi = status_reg.vblank_flag && ctrl_reg1.execute_nmi_on_vblank; 
    }

    if (++pp_cycle == 341) {
      if (scanline == 261) { //end of dummy scanline
        nes_->on_render();
      } else if (scanline == 262) { //end of dummy scanline
        status_reg.vblank_flag = 1;
      }
      scanline = (scanline + 1) % 263;
      pp_cycle = 0;
    }
  }
}

void Ppu2::TickPAL() {

}

void Ppu2::OnSettingsChanged() {

}