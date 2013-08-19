
#include "nes.h"
#include "mappers/none.h"//0
#include "mappers/mmc1.h"//1
#include "mappers/unrom.h"//2
#include "mappers/cnrom.h"//3
//#include "mappers/mmc3.h"//4
#include "mappers/aorom.h"//7



GamePak::GamePak() : Component(), mapper(nullptr), ROM(nullptr),VROM(nullptr),NRAM(nullptr),PRAM(nullptr) {

}

GamePak::~GamePak() {
  Deinitialize();
}

int GamePak::InitializeFromFile(Nes* nes,const char* filename) {
  if (init_ == true)
    return S_FALSE;
  Component::Initialize(nes);
  init_ = true;
  FILE* fp = fopen(filename, "rb");
  if (fp == nullptr)
    return S_FALSE;
  // Read the ROM file header
  if (!(fgetc(fp)=='N' && fgetc(fp)=='E' && fgetc(fp)=='S' && fgetc(fp)=='\32')) {
    return S_FALSE;
  }
  prg_count_ = fgetc(fp);
  chr_count_ = fgetc(fp);
  ctrlbyte_   = fgetc(fp);
  mappernum_  = fgetc(fp) | (ctrlbyte_>>4);
  fgetc(fp);fgetc(fp);fgetc(fp);fgetc(fp);fgetc(fp);fgetc(fp);fgetc(fp);fgetc(fp);
  if(mappernum_ >= 0x40) mappernum_ &= 15;

  // Read the ROM data
  rom_size = prg_count_ * 0x4000;
  ROM = new uint8_t[rom_size]; 
  fread(&ROM[0], prg_count_, 0x4000, fp);
  
  has_vrom = chr_count_==0?false:true;
  if (has_vrom == true) {
    vrom_size = chr_count_ * 0x2000;
    VROM = new uint8_t[vrom_size];
    fread(&VROM[0], chr_count_, 0x2000, fp);
  } else {
    vrom_size = 0x2000;
    VROM = new uint8_t[vrom_size];
    memset(VROM,0,vrom_size);//initializing VROM/VRAM
  }

  fclose(fp);
  //printf("%u * 16kB ROM, %u * 8kB VROM, mapper %u, ctrlbyte %02X\n", rom16count, vrom8count, mappernum, ctrlbyte);
  
  return InternalInitialize();
}

int GamePak::InternalInitialize() {

  NRAM = new uint8_t[0x1000];
  PRAM = new uint8_t[0x2000];

  Nta[0] = NRAM+0x0000;
  Nta[1] = NRAM+0x0400;
  Nta[2] =  NRAM+0x0000;
  Nta[3] =  NRAM+0x0400;

  memset(&prg_banks,0,sizeof(prg_banks));
  memset(&chr_banks,0,sizeof(chr_banks));

  
  //for(unsigned v=0; v<4; ++v) 
  //  SetROM(0x4000, v*0x4000, v==3 ? -1 : 0);
  
  
  //Switch32kPrgRom(0);
  //1111 1111 1111 1111
  //rombank(0x4000,0x8000,0);
  //rombank(0x4000,0xC000,2);
  //rombank(0x4000,0x8000,0);
  //rombank(0x4000,0xc000,0);
  //rombank(0xC000,nes_->rom16count());
  switch (mappernum_) {
    case 0:   mapper = new NoMapper(*this);  break;
    case 1:   mapper = new MMC1(*this);      break;
    case 2:   mapper = new UNROM(*this);     break;
    case 3:   mapper = new CNROM(*this);     break;
    //case MMC3::number:   mapper = new MMC3(*this);      break;
    case 7:   mapper = new AOROM(*this);      break;
    default:
      mapper = nullptr;
      //return S_FALSE;
  }
  mapper->Initialize();
  return S_OK;
}

int GamePak::Deinitialize() {
  if (init_ == true) {
    SafeDeleteArray(&ROM);
    SafeDeleteArray(&VROM);
    SafeDeleteArray(&PRAM);
    SafeDeleteArray(&NRAM);
    SafeDelete(&mapper);
    init_ = false;
    return S_OK;
  }
  return S_FALSE;
}

void GamePak::SwitchPrgRom(int prg_rom_index, int baseaddress,int size_kb) {
  int i;
  prg_rom_index &= ((prg_count_)-1);
  for (i = 0; i < size_kb>>2; i++)  {
    prg_banks[(baseaddress>>12)+i] = &ROM[(((prg_rom_index<<2) + i)*0x1000) % rom_size];
  }
}

void GamePak::SetVROM(unsigned size, unsigned baseaddr, unsigned index) {
  for(unsigned v = vrom_size + index * size,
                p = baseaddr / VROM_Granularity;
                p < (baseaddr + size) / VROM_Granularity && p < VROM_Pages;
                ++p, v += VROM_Granularity)
      chr_banks[p] = &VROM[v % vrom_size];
};


uint8_t GamePak::Read(uint16_t addr) {
  
    if( (addr >> 13) == 3 ) {

      return PRAM[addr & 0x1FFF ];
    }

    if ((addr>>12)<8) {
      DebugBreak();
    }
    return prg_banks[addr>>12][addr&0xFFF];
}

void GamePak::Write(uint16_t addr, uint8_t value) {
    /*if(write && addr >= 0x8000 && nes_->mappernum() == 7) // e.g. Rare games
    {
        SetROM(0x8000, 0x8000, (value&7));
        Nta[0] = Nta[1] = Nta[2] = Nta[3] = &NRAM[0x400 * ((value>>4)&1)];
    }*/

  if(addr >= 0x8000) {
    mapper->Write(addr,value);
  }

  if( (addr >> 13) == 3 ) {
    PRAM[addr & 0x1FFF ] = value;
    #ifdef _DEBUG
      if (value != 0)
      nes_->log.Channel("gp-6000").Log("%c",value);
    #endif
  }
}

