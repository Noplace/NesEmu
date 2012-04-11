int WASAPI_GetSampleRate();
int WASAPI_Initialize(int rate,int channels,int bits);
int WASAPI_Deinitialize();
int WASAPI_WriteData(void* data,size_t sample_count);