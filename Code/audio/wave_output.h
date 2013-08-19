#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinCore/types.h>
#include <mmsystem.h>
#include <mmreg.h>

#define		BUF_SIZE_LO	100						//Must be multiples of 100
#define		BUF_SIZE_HI	5000
#define		NUM_BLK_LO	2
#define		NUM_BLK_HI	16						//Can't be > 32
#define		MIN_SIZE_LO	2
#define		MIN_SIZE_HI	32
#define		MAX_SIZE_LO	8
#define		MAX_SIZE_HI	128

#define		CLAMP(v,n,x) \
			if (v<(n)) v=n; \
			else \
			if (v>(x)) v=x;

typedef struct
{
	HWAVEOUT				handle;				//Device handle
	WAVEOUTCAPS				cap;				//Device capabilities
	WAVEFORMATEXTENSIBLE	fmt;				//Wave format structure
	MMTIME					time;				//Number of samples output
	WAVEHDR					hdr[NUM_BLK_HI];	//Wave headers
	uint32_t						idle;				//Flags for blocks that aren't in the queue
	int64_t						samples;			//Sample frames output for this song
	int64_t						tbytes;				//Total bytes output by soundcard
	int32_t						ttime;				//Total time output by soundcard minus current song
	uint32_t						smpCnt;				//Last sample count
} Wave;

class WaveOutput {
 public:
  WaveOutput();
  ~WaveOutput();
  int Initialize();
  int Deinitialize();
  int Open(int32_t rate, uint32_t chn, int32_t bits, int32_t bufferlenms, int32_t prebufferms);
  int Close();
  int Flush(int32_t t);
  int32_t Write(int8_t *pIn, int32_t len);
 protected:
  uint32_t	open;									//Output device is open
  int32_t		pan,vol;							//Panning and volume level
  int32_t		paused;								//Output is paused
  uint32_t	device;	
  struct											//Structure for wave blocks
  {
	  int32_t	num;									//Maximum number of blocks allowed in queue
	  int32_t	min;									//Minimun size of block (in bytes)
	  int32_t	max;									//Maximum size of block
	  int32_t	cnt;
	  int64_t	size;
  } blk;

  struct											//Structure for managing input ring buffer
  {
	  int32_t size,asize;					//Size of buffer, allocated size
	  int32_t read,write;					//Indices of read and write positions in ring buffer
	  int64_t bytes;							//Bytes written for this song
	  int64_t total_bytes;				//Total bytes written
	  int32_t total_time;					//Total time written minus current song
	  int32_t length;							//Length of unqueued data in ring buffer
	  int32_t queued_length;			//Length of queued data taken from buffer
	  void*   pIn;								//-> ring buffer used for input
	  void*   pOut;								//-> linear buffer used by wave blocks for output
  } buffer;
  Wave wav;
  static void CALLBACK WaveOutDone(HWAVEOUT hwo, uint32_t uMsg, DWORD_PTR dwInstance, WAVEHDR *pWHdr, DWORD_PTR dwParam2);
};