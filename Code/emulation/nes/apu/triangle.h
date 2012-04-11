struct Triangle {
  bool enabled;
  int temp_lc1,temp_lc2,length_counter, linear_counter;
  int wave_counter, phase, level;
  bool halt;
  union {
    uint8_t raw;
    struct {
      unsigned linear_counter_init:7;
      unsigned halt:1;
    };
  }reg0;

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
    if (enabled == false) return 0;//8;

    int wl = (wave_length.raw+1);
    
    // Sample may change at wavelen intervals.
    auto& S = level;
    if(!count(wave_counter, wl)) 
      return S;

    if (length_counter && linear_counter && wl >= 3)
      ++phase;
    return S = (phase & 15) ^ ((phase & 16) ? 15 : 0);
  }

  void Clock(bool length_tick,bool linear_tick) {
    int wl = wave_length.raw;

    // Length tick (all channels except DMC, but different disable bit for triangle wave)
    if(length_tick && length_counter
      && !halt)
        length_counter -= 1;

    // Linear tick (triangle wave only)
    if (linear_tick) linear_counter = reg0.halt ? reg0.linear_counter_init : (linear_counter > 0 ? linear_counter - 1 : 0);
  }
};