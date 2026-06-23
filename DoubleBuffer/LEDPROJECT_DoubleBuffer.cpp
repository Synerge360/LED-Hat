//DoubleBuffer.cpp: Cpp file for double buffers

#include "LEDPROJECT_DoubleBuffer.h"

//Create new Double Buffer Object with given size
DoubleBuffer::DoubleBuffer(int defaultsize) 
{
  bufferSize = defaultsize;
  resetBuffer();
}

//Delete Double Buffer Object to save memory
DoubleBuffer::~DoubleBuffer()
{
}

//Set new size for buffer & resets
void DoubleBuffer::setBufferSize(int size)
{
  bufferSize = size;
  resetBuffer();
}

//Returns size of the buffer
int DoubleBuffer::getBufferSize()
{
  return bufferSize;
}

//Resets the buffer by reallocating memory and setting it to 0x00
void DoubleBuffer::resetBuffer()
{
  ObjectBuffer[0] = (uint8_t*)realloc(ObjectBuffer[0], bufferSize);
  ObjectBuffer[1] = (uint8_t*)realloc(ObjectBuffer[1], bufferSize);
  memset(ObjectBuffer[0], 0, bufferSize);
  memset(ObjectBuffer[1], 0, bufferSize);

  //Reset all variables
  currentBuffer = 0;
  bufferLength[0] = 0;
  bufferLength[1] = 0;
  readBufferOffset[0] = 0;
  readBufferOffset[1] = 0;
}

//Returns the size of the buffer that can be written
int DoubleBuffer::availableForWrite()
{
  return (bufferSize - (bufferLength[currentBuffer] - readBufferOffset[currentBuffer]));
}

int DoubleBuffer::writeToBuffer(const void *buffer, int writeSpace)
{
  int space = availableForWrite();

  if (writeSpace > space) {
    writeSpace = space;
  }

  if (writeSpace == 0) {
    return 0;
  }

  memcpy(&ObjectBuffer[currentBuffer][bufferLength[currentBuffer]], buffer, writeSpace);
  bufferLength[currentBuffer] += writeSpace;

  return writeSpace;
}

int DoubleBuffer::readBuffer(void *buffer, int readsize)
{
  int availabeSpace = available();

  if (readsize > availabeSpace) {
    readsize = availabeSpace;
  }

  if (readsize == 0) {
    return 0;
  }

  memcpy(buffer, &ObjectBuffer[currentBuffer][readBufferOffset[currentBuffer]], readsize);
  readBufferOffset[currentBuffer] += readsize;

  return readsize;
}

void* DoubleBuffer::getData()
{
  return (void*)ObjectBuffer[currentBuffer];
}

int DoubleBuffer::available()
{
  return bufferLength[currentBuffer] - readBufferOffset[currentBuffer];
}

void DoubleBuffer::swapBuffers(int length)
{
  if (currentBuffer == 0) {
    currentBuffer = 1;
  } else {
    currentBuffer = 0;
  }

  bufferLength[currentBuffer] = length;
  readBufferOffset[currentBuffer] = 0;
}
