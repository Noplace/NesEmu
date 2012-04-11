
namespace audio {

struct HiddenMembers;

class DirectSound : public AudioInterface {
 public:
  DirectSound();
  ~DirectSound();
  int Initialize(uint32_t sample_rate, uint8_t channels, uint8_t bits);
  int Deinitialize();
  int Write(void* data, size_t size);
  void set_window_handle(void* window_handle) { window_handle_ = window_handle; }
 protected:
  uint8_t* audio_data;  
  HiddenMembers* encapsulated; 
  void* window_handle_;
  uint32_t offset_;
  uint32_t written_bytes;
  uint32_t buffer_size;
  uint32_t next_pos;
};

}