struct Noise {
  static const uint16_t noise_periods[16];
  bool enabled;
  int temp_lc1,temp_lc2,length_counter, linear_counter, address, envelope;
  int sweep_delay, env_delay, wave_counter, hold, phase, level;
  bool halt;
  union {
    uint8_t raw;
    struct {
      unsigned nnnn:4;
      unsigned constant_volume_flag:1;
      unsigned halt:1;
      unsigned xx:2;
    };
  }reg0;

  union {
    uint8_t raw;
    struct {
      unsigned noise_frequency:4;
      unsigned xxx:3;
      unsigned noise_type:1;
    };
  }reg2;

  union {
    uint8_t raw;
    struct {
      unsigned xxx:3;
      unsigned length_counter_init:5;
    };
  }reg3;

  bool count(int& v, int reset) { return --v < 0 ? (v=reset),true : false; }

  int Tick() {
    halt = reg0.halt;
	  if (temp_lc1)
	  {
		  if (length_counter == temp_lc2)
			  length_counter = temp_lc1;
		  temp_lc1 = 0;
	  }
    if(enabled == false) return 0;//8;
    int wl = wl = noise_periods[ reg2.noise_frequency ];

    int volume = length_counter ? reg0.constant_volume_flag ? reg0.nnnn : envelope : 0;
    // Sample may change at wavelen intervals.
    auto& S = level;
    if(!count(wave_counter, wl)) return S;
    if(!hold) hold = 1;
    hold = (hold >> 1)
          | (((hold ^ (hold >> (reg2.noise_type ? 6 : 1))) & 1) << 14);
    return S = (hold & 1) ? 0 : volume;
  }

  void Clock(bool length_tick,bool envelope_tick) {
    // Length tick (all channels except DMC, but different disable bit for triangle wave)
    if(length_tick && length_counter
      && !halt)
        length_counter -= 1;

    // Envelope tick (square and noise channels)
    if(envelope_tick && count(env_delay, reg0.nnnn))
      if(envelope > 0 || reg0.halt)
            envelope = (envelope-1) & 15;
  }
};