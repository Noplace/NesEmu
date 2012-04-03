#include "../nes.h"


//2 modes,6 steps (2 extra for 4mode,1extra for 5mode),4 flags(length,sweep,linear,envelope)
/*const Apu::TickLine Apu::tick_table[2][6]= {
  {
    {0,1    , false, false, false, false},        
    {3728, 2, false, false, true , true},
    {7457, 3, true , true , true , true},        
    {11185,4, false, false, true , true},
    {14914,5, true , true , true , true},        
    {14915,0, false, false, false, false}
  },
  {
    {0,1    , false, false, false, false},        
    {3728, 2, false, false, true , true},
    {7457, 3, true , true , true , true},        
    {11185,4, false, false, true , true},
    {14914,5, false, false, false, false},        
    {18640,0, true , true , true , true}
  }
};*/
/*
const Apu::TickLine Apu::tick_table[2][6]= {
  {
    { 7459,1, false, false, true , true},
    {14915,2, true , true , true , true},        
    {22373,3, false, false, true , true},
    {29831,4, true , true , true , true},
    {29832,0, false, false, false, false},
    {00000,0, false, false, false, false}     
  },
  {
    {00001,1, true , true , true , true},
    { 7459,2, false, false, true , true},
    {14915,3, true , true , true , true},        
    {22373,4, false, false, true , true},
    {29829,5, false, false, false, false},
    {37281,0, false, false, false, false}   
  }
};*/
//NTSC only
const Apu::TickLine Apu::tick_table[2][7]= {
  {
    {7459,1, false, false, true , true},
    {7456,2, true , true , true , true},        
    {7458,3, false, false, true , true},
    {7457,4, false, false, false , false},
    {1,5   , true , true , true, true},
    {1,6   , false, false, false, false},
    {7457,0, false, false, false, false}
  },
  {
    {1,1, true , true , true , true},
    {7458,2, false, false, true , true},
    {7456,3, true , true , true , true},        
    {7458,4, false, false, true , true},
    {7456,5, false, false, false, false},
    {7454,0, false, false, false, false},
    {}
  }
};

const uint8_t Apu::length_counters[32] = {
  10 , 254, 20,  2, 40,  4, 80,  6,
  160, 8  , 60, 10, 14, 12, 26, 14,
  12 , 16 , 24, 18, 48, 20, 96, 22,
  192, 24 , 72, 26, 16, 28, 32, 30 };
//const uint8_t Apu::length_counters2[32] = { 10,254,20, 2,40, 4,80, 6,160, 8,60,10,14,12,26,14,
                                 //          12, 16,24,18,48,20,96,22,192,24,72,26,16,28,32,30 };
const uint16_t Noise::noise_periods[16] = { 2,4,8,16,32,48,64,80,101,127,190,254,381,508,1017,2034 };
const uint16_t DMC::periods[16] = { 428,380,340,320,286,254,226,214,190,160,142,128,106,84,72,54 };

Apu::Apu() : Component() , cpu(nullptr),cpu_cycles_ptr(nullptr),frame_step(0) {

}

Apu::~Apu() {
  
}

int Apu::Initialize(Nes* nes) {
  Component::Initialize(nes);

  square_table[0] = 0;
  for (int i=1;i<32;++i) {
    square_table[i] = 95.52 / ((8128.0 / double(i)) + 100);
  }

  tri_table[0] = 0;
  for (int i=1;i<256;++i) {
    tri_table[i] = 163.67 / ((24329.0 / double(i)) + 100);
  }
  write_sideeffect_counter = 0;
  cpu_cycles_ptr = &nes_->cpu().cycles;
  cpu = &nes_->cpu();
  frame_step = 0;
  cycles = tick_table[0][0].step_cycle;
  return S_OK;
}

void Apu::Power() {
  memset(&square1,0,sizeof(square1));
  memset(&square2,0,sizeof(square2));
  memset(&triangle,0,sizeof(triangle));
  memset(&noise,0,sizeof(noise));
  memset(&dmc,0,sizeof(dmc));
  last_4017_value = 0;
  cycles = 0;
  cycles = tick_table[0][0].step_cycle;
  frame_step = 0;
  frame_interrupt = false;
  write_sideeffect_counter = 0;
  sequencer_mode = false;
  interrupt_inhibit = true; 
  cpu->MemWriteAccess(0x4017,last_4017_value);
  OnSettingsChanged();
}

void Apu::Reset() {
  square1.enabled = false;
  square2.enabled = false;
  triangle.enabled = false;
  noise.enabled = false;
  dmc.enabled = false;
  dmc.irq = false;
  write_sideeffect_counter = 0;
  cycles = 0;
  frame_step = 0;
  frame_interrupt = false;
  sequencer_mode = false;
  interrupt_inhibit = true; 
  cpu->MemWriteAccess(0x4017,last_4017_value);
  OnSettingsChanged();
}

uint8_t Apu::Read() {
  uint8_t res = 0;
  res |= square1.length_counter ? 1 : 0;
  res |= square2.length_counter ? 1 << 1 : 0;
  res |= triangle.length_counter ? 1 << 2 : 0;
  res |= noise.length_counter ? 1 << 3 : 0;
  res |= dmc.length_counter ? 1 << 4 : 0;
  res |= 0x40*frame_interrupt;
  res |= 0x80*dmc.irq;
  frame_interrupt = false;
  dmc.irq = false;
  cpu->enable_irq = false;
  return res;
}

void Apu::Write(uint16_t address, uint8_t value) {
  switch (address) {
    case 0x4000:
      square1.reg0.raw = value;
      break;
    case 0x4001:
      square1.reg1.raw = value;
      square1.sweep_delay = square1.reg1.sweep_rate;
      break;
    case 0x4002:
      square1.wave_length.low = value;
      break;
    case 0x4003:
      square1.reg3.raw = value;
      square1.wave_length.high = square1.reg3.timer_high;
      if (square1.enabled == true) {
        square1.length_counter = length_counters[square1.reg3.length_counter_init];
      }
      square1.env_delay = square1.reg0.nnnn;
      square1.envelope  = 15;
      square1.phase = 0;
      break;

    case 0x4004:
      square2.reg0.raw = value;
      break;
    case 0x4005:
      square2.reg1.raw = value;
      square2.sweep_delay = square1.reg1.sweep_rate;
      break;
    case 0x4006:
      square2.wave_length.low = value;
      break;
    case 0x4007:
      square2.reg3.raw = value;
      square2.wave_length.high = square2.reg3.timer_high;
      if (square2.enabled == true) {
        square2.length_counter = length_counters[square2.reg3.length_counter_init];
      }
      square2.env_delay = square2.reg0.nnnn;
      square2.envelope  = 15;
      square2.phase = 0;
      break;

    case 0x4008:
      if(triangle.reg0.halt) 
        triangle.linear_counter=value&0x7F; 
      triangle.reg0.raw = value; 
      break;
    case 0x400A:
      triangle.wave_length.low = value;
      break;
    case 0x400B:
      triangle.reg3.raw = value;
      triangle.wave_length.high = triangle.reg3.timer_high;
      if(triangle.enabled==true) {
        triangle.length_counter = length_counters[triangle.reg3.length_counter_init];
      }
      triangle.linear_counter = triangle.reg0.linear_counter_init;
      break;

    case 0x400C:
      noise.reg0.raw = value;
      break;
    case 0x400E:
      noise.reg2.raw = value;
      break;
    case 0x400F:
      noise.reg3.raw = value;
      if(noise.enabled==true) {
        noise.length_counter = length_counters[noise.reg3.length_counter_init];
      }
      noise.env_delay      = noise.reg0.nnnn;
      noise.envelope       = 15;
      break;

    case 0x4010: 
      dmc.wave_length = DMC::periods[value&0x0F]; 
      dmc.irq_enable = value & 0x80;
      dmc.loop = value & 0x40;
      break;
    case 0x4011: 
      dmc.direct_load = value & 0x7F; // dac value
      break; 
    case 0x4012: 
      dmc.sample_address_val = value;
      dmc.sample_address = (value | 0x300) << 6;
      break;
    case 0x4013: 
      dmc.sample_length = value;
      break; 

    case 0x4015:
      square1.enabled = value & 1;
      square2.enabled = value & (1 << 1);
      triangle.enabled = value & (1 << 2);
      noise.enabled = value & (1 << 3);
      dmc.enabled = value & (1 << 4);
      if (square1.enabled == false) square1.length_counter = 0;
      if (square2.enabled == false) square2.length_counter = 0;
      if (triangle.enabled == false) triangle.length_counter = 0;
      if (noise.enabled == false) noise.length_counter = 0;
      if (dmc.enabled == false) 
        dmc.length_counter = 0;
      else if (dmc.length_counter == 0)
        dmc.length_counter = dmc.sample_length*16 + 1;

      dmc.irq = false;
      break;

    case 0x4017:
      #ifdef _DEBUG
        //nes_->log.Channel("apu").Log("at PC %x , apu write 4017 = %x\n",nes_->cpu().PC-1,value);
      #endif
      last_4017_value = value;
      interrupt_inhibit = value & 0x40;
      sequencer_mode  = value & 0x80;
      write_sideeffect_counter = 2-(*cpu_cycles_ptr % 2);
      
      if (sequencer_mode == true) {
        /*square1.Clock(true,false,false);
        square2.Clock(true,false,false);
        triangle.Clock(true,false);
        noise.Clock(true,false);*/
      } else {

      }
      cycles = tick_table[sequencer_mode][0].step_cycle;
      frame_step = 0;
      if ((*cpu_cycles_ptr)&1) {
        cycles+=2;
      } else {
        ++cycles;
      }


      if(interrupt_inhibit == true)  {
        frame_interrupt = dmc.irq = false;//DMC_IRQ = false;
        cpu->LowerIRQLine();
        //cpu->enable_irq = frame_interrupt = !interrupt_inhibit;
        //cpu->irq_line = cpu->enable_irq && !cpu->P.I;
      }

      break;
  }
}
// Invoked at CPU's rate.
void Apu::Tick(double dt) {
  /*if (write_sideeffect_counter) {
    if (!--write_sideeffect_counter) {
      cycles = 0;
      frame_step = sequencer_mode == false?0:1;
    }
  }*/
  /*if (cycles++ == 7457) {
    auto& tick_line =  tick_table[sequencer_mode][frame_step];

    if (sequencer_mode == false && frame_step==4) { // && cycles >= 14914 && cycles <= 14915) {
      cpu->enable_irq = frame_interrupt = !interrupt_inhibit;
    }

    square1.Clock(tick_line.length,tick_line.sweep,tick_line.envelope);
    square2.Clock(tick_line.length,tick_line.sweep,tick_line.envelope);
    triangle.Clock(tick_line.length,tick_line.linear);
    noise.Clock(tick_line.length,tick_line.envelope);

    frame_step = tick_line.next_frame_step;
    //if (frame_step == 0) {
    //  cycles = 0;
    //}
    cycles = 0;
  }*/

  /*
  //if ((*cpu_cycles_ptr % 2) == 1) { //every other cpu cycle 
    auto& tick_line =  tick_table[sequencer_mode][frame_step];
    if (sequencer_mode == false && cycles >= 29830 && cycles <= 29832) {
      cpu->enable_irq = frame_interrupt = !interrupt_inhibit;
      cpu->irq_line = cpu->enable_irq && !cpu->P.I;
    }
    if (tick_line.step_cycle == cycles) {
      square1.Clock(tick_line.length,tick_line.sweep,tick_line.envelope);
      square2.Clock(tick_line.length,tick_line.sweep,tick_line.envelope);
      triangle.Clock(tick_line.length,tick_line.linear);
      noise.Clock(tick_line.length,tick_line.envelope);
      frame_step = tick_line.next_frame_step;
      if (frame_step == 0) {
        if (sequencer_mode == false) 
          cycles -= 29830;
        else
          cycles -= 37282;
      }
    }
    ++cycles;

  //}*/

  if (!--cycles) {
    auto& tick_line =  tick_table[sequencer_mode][frame_step];
    square1.Clock(tick_line.length,tick_line.sweep,tick_line.envelope);
    square2.Clock(tick_line.length,tick_line.sweep,tick_line.envelope);
    triangle.Clock(tick_line.length,tick_line.linear);
    noise.Clock(tick_line.length,tick_line.envelope);
		if ((sequencer_mode == false) && ((frame_step == 3) || (frame_step == 4) || (frame_step == 5)) && !(interrupt_inhibit)) {
      frame_interrupt = true;
      //cpu->enable_irq = frame_interrupt = true;
      //cpu->irq_line = cpu->enable_irq && !cpu->P.I;
      cpu->RaiseIRQLine();
    }

    //frame_step = tick_table[sequencer_mode][frame_step].next_frame_step;
    cycles = tick_table[sequencer_mode][++frame_step].step_cycle;

    if (sequencer_mode == true) {
			if (frame_step == 5)
				frame_step = 0;
    } else {
			if (frame_step == 6)
				frame_step = 0;
    }
  }

  auto sindex = square1.Tick() + square2.Tick();
  auto square_out = square_table[sindex];
    
  auto tindex = dmc.Tick([&](uint16_t address){ return cpu->MemReadAccess(address);  },cpu->enable_irq,interrupt_inhibit) + 
    3*triangle.Tick() + 2*noise.Tick();
  auto tri_out = tri_table[tindex];

  sample_hold = (square_out+tri_out) * 32767.0 ;

  auto avg_sample = [](RingBuffer<double>& samples) -> double {
    double res = 0;
    for (int i=0;i<samples.size();++i) {
      res += samples[i];
    }
    res /= samples.size();
    return res;
  };

  ++sample_counter;
  if (sample_counter >= sample_ratio) {
    auto sample = avg_sample(sample_hold);//lowpass_filter.calc(sample_hold);
    short sbuf[2];
    sbuf[0] = sbuf[1] = short(sample); //mono to stereo
    sample_counter -= sample_ratio;   
    IO::writeAudioBlock(sbuf,2*sizeof(sbuf[0]));
  }

}


void Apu::OnSettingsChanged() {
  sample_counter = 0;
  sample_ratio = uint32_t(nes_->settings.cpu_freq_hz / 44100);
  sample_hold.Resize(sample_ratio);
}