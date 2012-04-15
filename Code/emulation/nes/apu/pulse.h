struct Pulse {
  bool enabled;
  int temp_lc1,temp_lc2,length_counter, envelope;
  int sweep_delay, env_delay, wave_counter, phase, level;
  bool halt;
  union {
    uint8_t raw;
    struct {
      unsigned nnnn:4;
      unsigned constant_volume_flag:1;
      unsigned halt:1;
      unsigned duty_cycle:2;
    };
  }reg0;
  union {
    uint8_t raw;
    struct {
      unsigned sweep_shift:3;
      unsigned sweep_decrease:1;
      unsigned sweep_rate:3;
      unsigned sweep_enable:1;
    };
  }reg1;

  union {
    uint8_t raw;
    struct {
      unsigned timer_high:3;
      unsigned length_counter_init:5;
    };
  }reg3;

  union {
    uint16_t raw;
    struct {
      unsigned low:8;
      unsigned high:3;
    };
  } wave_length;
  
  bool count(int& v, int reset) { return --v < 0 ? (v=reset),true : false; }
  int Tick() {
    halt = reg0.halt;
	  if (temp_lc1)
	  {
		  if (length_counter == temp_lc2)
			  length_counter = temp_lc1;
		  temp_lc1 = 0;
	  }
    if(enabled==false) 
      return 0;//8;

    int wl = (wave_length.raw+1)*2;

    int volume = length_counter ? reg0.constant_volume_flag ? reg0.nnnn : envelope : 0;
    // Sample may change at wavelen intervals.
    auto& S = level;
    if(!count(wave_counter, wl)) 
      return S;
    if(wl < 8) 
      return S = 0;//8
    
    return S = (0x9F786040u & (1u << (++phase % 8 + reg0.duty_cycle * 8))) ? volume : 0;
  }
     
  void Clock(bool length_tick,bool sweep_tick,bool envelope_tick) {
    int wl = wave_length.raw;

    // Length tick (all channels except DMC, but different disable bit for triangle wave)
    if(length_tick && length_counter && !(halt))
            length_counter -= 1;
      

    // Sweep tick (square waves only)
    if(sweep_tick && count(sweep_delay, reg1.sweep_rate))
      if(wl >= 8 && reg1.sweep_enable && reg1.sweep_shift) {
            int s = wl >> reg1.sweep_shift, d[4] = {s, s, ~s, -s};
            wl += d[reg1.sweep_decrease*2 + 0];
            if(wl < 0x800) wave_length.raw = wl;
        }
      
      // Envelope tick (square and noise channels)
      if(envelope_tick && count(env_delay, reg0.nnnn))
        if(envelope > 0 || reg0.halt)
              envelope = (envelope-1) & 15;
  }
};