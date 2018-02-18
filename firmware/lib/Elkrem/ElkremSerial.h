#ifndef ElkremSerialClass_h
#define ElkremSerialClass_h
#include <inttypes.h>
#include "Elkrem.h"

template <typename T, uint16_t Size>
class CircularBuffer {
public:
  CircularBuffer() :
    wp_(buf_), rp_(buf_), tail_(buf_+Size),  remain_(0) {}
  ~CircularBuffer() {}
  void push(T value) {
    if(remain_==Size)return;
    *wp_++ = value;
    remain_++;
    if (wp_ == tail_) wp_ = buf_;
  }
  T pop() {
    T result = *rp_++;
    remain_--;
    if (rp_ == tail_) rp_ = buf_;
    return result;
  }
  int remain() const {
    return remain_;
  }

private:
  T buf_[Size];
  T *wp_;
  T *rp_;
  T *tail_;
  uint16_t remain_;
};

class ElkremSerialClass
{
public:
	int available();
	char read();
	int readBytes(char *, int);
	void flush();
  void write(char);
	void print(char);
	void print(int);
	void print(unsigned int);
	void print(long);
	void print(unsigned long);
	void print(const char *);
	void print(String );
	void print(double ,int = 3);
  void println();
	void println(char);
	void println(int);
	void println(unsigned int);
	void println(long);
	void println(unsigned long);
	void println(const char *);
	void println(String);
	void println(double , int = 3);

private:
	CircularBuffer<char,256> buffer;
	void processData(MessageType,uint8_t *, uint16_t);
  void sendByteArray(uint8_t *, uint16_t);
};

extern ElkremSerialClass Serial2;

#endif
