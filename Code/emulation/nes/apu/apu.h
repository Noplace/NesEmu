#include "pulse.h"
#include "triangle.h"
#include "noise.h"
#include "dmc.h"

class Apu : public Component /* Audio Processing Unit */
{
 public:
  Apu();
  ~Apu();
  int Initialize(Nes* nes);
  void Power();
  void Reset();
  uint8_t Read();
  void Write(uint16_t address, uint8_t value);
  void Tick();
  void OnSettingsChanged();
  void StartupRoutine() {
    for (int i =0;i<10;++i) {
      cpu->MemWriteAccess(0x4017,last_4017_value_);
      ++cycles;
      ++tick_counter;
    }
  }
  uint8_t last_4017_value() { return last_4017_value_; };
 protected:
  struct TickLine {
    uint32_t ticks;
    uint8_t next_frame_step;
    bool length,sweep,linear,envelope;
  };
  static const TickLine ntsc_tick_table[2][7];
  static const TickLine pal_tick_table[2][7];
  static const uint8_t length_counters[32];
  const TickLine* tick_table[2];
  double square_table[32];
  double tri_table[256];
  RingBuffer<double> sample_hold;
  Pulse square1,square2;
  Triangle triangle;
  Noise noise;
  DMC dmc;
  Cpu* cpu;
  uint64_t cycles;
  uint32_t sample_counter;
  uint32_t sample_ratio;
  uint32_t tick_counter;
  uint8_t  frame_step;
  uint8_t last_4017_value_;
  bool sequencer_mode;
  bool interrupt_inhibit;
  bool frame_interrupt;
};