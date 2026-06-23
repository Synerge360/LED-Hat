//DoubleBuffer.h: Header file for double buffers

#ifndef _LEDPROJECT_DOUBLE_BUFFER_H
#define _LEDPROJECT_DOUBLE_BUFFER_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

class DoubleBuffer
{
public:
  DoubleBuffer(int defaultsize);
  virtual ~DoubleBuffer();

  void resetBuffer();
  void setBufferSize(int size);
  int getBufferSize();
  int availableForWrite();
  int writeToBuffer(const void *buffer, int);
  int readBuffer(void *buffer, int);
  void* getData();
  int available(); 
  void swapBuffers(int length = 0);

private:
  uint8_t* ObjectBuffer[2] __attribute__((aligned (16))); //allocate 16 bytes of memory for the buffer
  int bufferSize; //size of the buffer
  volatile int bufferLength[2]; 
  volatile int readBufferOffset[2]; 
  volatile int currentBuffer; //current buffer being used (1 or 0)
};

#endif
