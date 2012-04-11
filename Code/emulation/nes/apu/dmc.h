struct DMC {
  static const uint16_t periods[16];
  bool enabled;
  int length_counter;
  int wave_counter, hold, phase, level;

  uint16_t wave_length;
  uint8_t direct_load;
  int sample_address_val,sample_address,sample_length;
  bool irq_enable;
  bool loop;
  bool irq;
  bool count(int& v, int reset) { return --v < 0 ? (v=reset),true : false; }
  template <typename MemRead>
  int Tick(MemRead& memRead,bool disable_irq) {
    if (enabled == false) return 0;//64
    int wl = (wave_length+1);
    
    // Sample may change at wavelen intervals.
    auto& S = level;
    if(!count(wave_counter, wl)) return S;
    // hold = 8 bit value, phase = number of bits buffered
    if(phase == 0) { // Nothing in sample buffer?
      if(!length_counter && loop) // Loop?
      {
          length_counter = sample_length*16 + 1;
          sample_address = (sample_address_val | 0x300) << 6;
      }
      if(length_counter > 0) // Load next 8 bits if available
      {
          // Note: Re-entrant! But not recursive, because even
          // the shortest wave length is greater than the read time.
          // TODO: proper clock
          if(wave_length>20)
              for(unsigned t=0; t<3; ++t) memRead(uint16_t(sample_address) | 0x8000);// nes_->cpu().MemReadAccess(uint16_t(sample_address) | 0x8000); // timing
          hold  = memRead(uint16_t(sample_address++) | 0x8000);//nes_->cpu().MemReadAccess(uint16_t(sample_address++) | 0x8000); // Fetch byte
          phase = 8;
          --length_counter;
      }
      else { // Otherwise, disable channel or issue IRQ
        if (irq_enable == true) {
          irq = true;
        }
        enabled = irq_enable && irq;
      }
    }
    if(phase != 0) { // Update the signal if sample buffer nonempty
      int v = direct_load;
      if(hold & (0x80 >> --phase)) v += 2; else v -= 2;
      if(v >= 0 && v <= 0x7F) direct_load = v;
    }
    return S = direct_load;
  }
};