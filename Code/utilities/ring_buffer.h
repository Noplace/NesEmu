
template<typename T>
class RingBuffer {
 public:
  RingBuffer() : counter(0),size_(0) {
    buffer = nullptr;
    Resize(0);
  }
  ~RingBuffer() {
    Resize(0);
  }
  void Resize(int size) {
    if (size != 0) {
      auto new_buffer = new T[size];
      if (buffer != nullptr) {
        memcpy(new_buffer,buffer,this->size_);
        delete []buffer;
      }
      buffer = new_buffer;
    } else {
      if (buffer != nullptr)
        delete [] buffer;
      buffer = nullptr;
    }
    this->size_ = size;
  }
  T& operator [](int index) {
    /*index = index % size;
    //if (index<0)
    index = counter+index;
    while (index<0)
      index += size;*/
    return buffer[index];
  }

  RingBuffer& operator =(T value) {
    buffer[counter++] = value;
    if (counter == size_)
      counter = 0;
    return *this;
  }

  operator T*() {
    return buffer;
  }
  int size() {   return size_;    }
 private:
  T* buffer;
  int counter;
  int size_;
};