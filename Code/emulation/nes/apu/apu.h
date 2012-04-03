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
  void Tick(double dt);
  void OnSettingsChanged();
  uint8_t last_4017_value;
 protected:
  struct TickLine {
    int step_cycle;
    int next_frame_step;
    bool length,sweep,linear,envelope;
  };
  static const TickLine tick_table[2][7];
  static const uint8_t length_counters[32];
  double square_table[32];
  double tri_table[256];
  RingBuffer<double> sample_hold;
  Pulse square1,square2;
  Triangle triangle;
  Noise noise;
  DMC dmc;
  uint64_t* cpu_cycles_ptr;
  Cpu* cpu;
  uint32_t sample_counter;
  uint32_t sample_ratio;
  uint32_t cycles;
  int frame_step;
  int write_sideeffect_counter; //>0 =counter,0 = do reset,<0 = do nothing
  bool sequencer_mode;
  bool interrupt_inhibit;
  bool frame_interrupt;
};