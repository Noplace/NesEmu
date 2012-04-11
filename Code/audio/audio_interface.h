namespace audio {

class AudioInterface {
 public:
  virtual int Initialize(uint32_t sample_rate,uint8_t channels,uint8_t bits) = 0;
  virtual int Deinitialize() = 0;
  virtual int Write(void* data,size_t size) = 0;
 protected:
  WAVEFORMATEX wave_format_;
};

}