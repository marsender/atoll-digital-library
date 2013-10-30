/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

See the file LicenseBerkeleyDB.txt for redistribution information.
Copyright (c) 2002,2006 Oracle. All rights reserved.

BinaryBuffer.cpp

*******************************************************************************/

#include "BinaryBuffer.hpp"
#include "AppAssert.hpp"
#include "AppException.hpp"
#include "Trace.hpp"
#include <stdlib.h>
#include <string.h>
#include <sstream> // For std::ostringstream
//------------------------------------------------------------------------------

using namespace Common;

BinaryBuffer::BinaryBuffer()
    : bufferSize_(0),
    pBuffer_(0),
    pCursor_(0),
    pOccupancy_(0),
    ownsMemory_(true)
{
}
//------------------------------------------------------------------------------

BinaryBuffer::BinaryBuffer(const void *p, size_t size)
    : bufferSize_(0),
    pBuffer_(0),
    pCursor_(0),
    pOccupancy_(0),
    ownsMemory_(true)
{
  set(const_cast<void*>(p), size);
}
//------------------------------------------------------------------------------

BinaryBuffer::BinaryBuffer(const BinaryBuffer& buffer, size_t extra)
    : bufferSize_(0),
    pBuffer_(0),
    pCursor_(0),
    pOccupancy_(0),
    ownsMemory_(true)
{
  if (extra > 0) {
    expandBuffer(buffer.getOccupancy() + extra);
    write(buffer.getBuffer(), buffer.getOccupancy());
  }
  else {
    *this = buffer;
  }
}
//------------------------------------------------------------------------------

BinaryBuffer::BinaryBuffer(const void *p, size_t size, bool wrapper)
    : bufferSize_(0),
    pBuffer_(0),
    pCursor_(0),
    pOccupancy_(0),
    ownsMemory_(!wrapper)
{
  set(const_cast<void*>(p), size);
}
//------------------------------------------------------------------------------

BinaryBuffer::~BinaryBuffer()
{
  deleteBuffer();
}
//------------------------------------------------------------------------------

void BinaryBuffer::set(const void *p, size_t nSize)
{
  // Deal with any previous buffer details.
  deleteBuffer();

  bufferSize_ = nSize;
  if (nSize != 0) {
    if (ownsMemory_) {
      pBuffer_ = ::malloc(nSize);
      if (!pBuffer_) {
        DEF_Exception(AppException::NoMemoryError, "BinaryBuffer: failed to allocate memory");
      }
    }
    else {
      pBuffer_ = const_cast<void*>(p);
    }
    if (p != 0) {
      if (ownsMemory_)
        ::memcpy(pBuffer_, p, nSize);
      // Copied in something meaningful, therefore occupied.
      pOccupancy_ = static_cast<char*>(pBuffer_) + nSize;
    }
    else {
      // Just allocated space, therefore no occupancy yet.
      pOccupancy_ = pBuffer_;
		}
  }
  else {
    pBuffer_ = 0;
    pOccupancy_ = 0;
  }
  pCursor_ = pBuffer_;
}
//------------------------------------------------------------------------------

size_t BinaryBuffer::readSeek(void* p, size_t n)
{
  // Tried to read something from a null pointer.
  // AppAssert((n==0) || ((n>0) && (p!=0)));

  // Tried to read off the end of the data in the buffer.
  //REQUIRE(pCursor_+n <= pOccupancy_);

  size_t toread = n;
  size_t remaining = static_cast<char*>(pOccupancy_) - static_cast<char*>(pCursor_);
  if (n > remaining) {
    toread = remaining;
  }
  if (toread > 0) {
    if (p)
      ::memcpy(p, pCursor_, toread);
    pCursor_ = static_cast<char*>(pCursor_) + toread;
  }

  return toread;
}
//------------------------------------------------------------------------------

size_t BinaryBuffer::read(size_t offset, void* p, size_t n)
{
  // Tried to read something from a null pointer.
  // AppAssert((n==0) || ((n>0) && (p!=0)));

  // Tried to read off the end of the data in the buffer.
  //REQUIRE(pCursor_+n <= pOccupancy_);

  char *startPos = static_cast<char*>(pBuffer_) + offset;

  size_t toread = n;
  size_t remaining = static_cast<char*>(pOccupancy_) - startPos;
  if (n > remaining) {
    toread = remaining;
  }
  if (toread > 0) {
    ::memcpy(p, startPos, toread);
  }

  return toread;
}
//------------------------------------------------------------------------------

size_t BinaryBuffer::write(const void* p, size_t n)
{
  size_t offset = 0;
  size_t towrite = reserve(offset, n);

  return write(offset, p, towrite);
}
//------------------------------------------------------------------------------

size_t BinaryBuffer::reserve(size_t &offset, size_t n)
{
  // Tried to write something to a null pointer.
  // AppAssert((n==0) || ((n>0) && (p!=0))));

  // If we tried to write off the end then expand the buffer.
  if (static_cast<char*>(pCursor_) + n >
      static_cast<char*>(pBuffer_) + bufferSize_) {
    size_t need = static_cast<char*>(pCursor_) + n - static_cast<char*>(pBuffer_);
    expandBuffer(need);
  }

  size_t towrite = n;
  size_t remaining = (static_cast<char*>(pBuffer_) + bufferSize_) - static_cast<char*>(pCursor_);
  if (n>remaining) {
    towrite = remaining;
  }
  if (towrite>0) {
    offset = (size_t)((char*)pCursor_ - (char*)pBuffer_);
    pCursor_ = static_cast<char*>(pCursor_) + towrite;
    pOccupancy_ = (pOccupancy_ > pCursor_ ? pOccupancy_ : pCursor_);
  }

  return towrite;
}
//------------------------------------------------------------------------------

size_t BinaryBuffer::write(size_t offset, const void *p, size_t n)
{
  AppAssert(n == 0 || (void*)((char*)pBuffer_ + offset + n) <= pOccupancy_);

  if (n > 0) {
    ::memcpy((void*)((char*)pBuffer_ + offset), p, n);
  }

  return n;
}
//------------------------------------------------------------------------------

size_t BinaryBuffer::getOccupancy() const
{
  std::ptrdiff_t n = 0;
  if (pOccupancy_ >= pBuffer_) {
    n = static_cast<char*>(pOccupancy_) - static_cast<char*>(pBuffer_);
  }

  return (size_t)n;
}
//------------------------------------------------------------------------------

size_t BinaryBuffer::getCursorPosition() const
{
  std::ptrdiff_t n = 0;
  if (pCursor_ >= pBuffer_) {
    n = static_cast<char*>(pCursor_) - static_cast<char*>(pBuffer_);
  }

  return (size_t)n;
}
//------------------------------------------------------------------------------

BinaryBuffer &BinaryBuffer::operator=(const BinaryBuffer& buffer)
{
  // Result is same sized buffer with same occupation.
  // b1 = b2; AppAssert(b1 == b2); will work
  if (this != &buffer) {
    set(buffer.getBuffer(), buffer.getSize());
    pOccupancy_ = static_cast<char*>(pBuffer_) + buffer.getOccupancy();
  }

  return *this;
}
//------------------------------------------------------------------------------

int BinaryBuffer::operator==(const BinaryBuffer& buffer) const
{
  return (getOccupancy() == buffer.getOccupancy() && ::memcmp(pBuffer_, buffer.pBuffer_, getOccupancy()) == 0);
}
//------------------------------------------------------------------------------

bool BinaryBuffer::operator<(const BinaryBuffer& buffer) const
{
	bool res;
  int i = ::memcmp(pBuffer_, buffer.pBuffer_,
                   getOccupancy() < buffer.getOccupancy() ?
                   getOccupancy() : buffer.getOccupancy());

  if (i == 0) {
    res = (getOccupancy() < buffer.getOccupancy());
  }
  else {
    res = (i < 0);
  }

	return res;
}
//------------------------------------------------------------------------------

static const unsigned long BUFFER_TOSTRING_BUFFERSIZE = 64;
std::string BinaryBuffer::asString(bool textOnly) const
{
	size_t displayBytes = (getOccupancy() < BUFFER_TOSTRING_BUFFERSIZE ? getOccupancy() : BUFFER_TOSTRING_BUFFERSIZE);
	char p2[BUFFER_TOSTRING_BUFFERSIZE * 2 + 4];
	char p3[BUFFER_TOSTRING_BUFFERSIZE + 4];
	unsigned char *b = static_cast<unsigned char*>(pBuffer_);
	char *p = p2;
	char *p1 = p3;
	for (size_t i = 0;i < displayBytes; ++i) {
		int hn = (*b & 0xF0) >> 4;
		int ln = *b & 0x0F;
		*p++ = (hn < 10 ? '0' + hn : 'a' + hn - 10);
		*p++ = (ln < 10 ? '0' + ln : 'a' + ln - 10);
		*p1++ = ((*b >= 0x80 || iscntrl(*b)) ? '.' : *b);
		++b;
	}
	if (displayBytes < getOccupancy()) {
		*p++ = '.';
		*p++ = '.';
		*p++ = '.';
		*p1++ = '.';
		*p1++ = '.';
		*p1++ = '.';
	}
	*p++ = '\0';
	*p1++ = '\0';
	std::ostringstream os1;
	if (!textOnly) {
		//os1 << "BufferSize=" << bufferSize_; // std::hex << bufferSize_
		os1 << " Size=" << getOccupancy(); // std::hex << getOccupancy()
		os1 << " Hex=" << p2;
		os1 << " Text=" << p3;
	} else
		os1 << p3;
	return os1.str();
}
//------------------------------------------------------------------------------

std::string BinaryBuffer::asStringBrief() const
{
	size_t displayBytes = (getOccupancy() < BUFFER_TOSTRING_BUFFERSIZE ? getOccupancy() : BUFFER_TOSTRING_BUFFERSIZE);
	char p2[BUFFER_TOSTRING_BUFFERSIZE * 2 + 4];
	unsigned char *b = static_cast<unsigned char*>(pBuffer_);
	char *p = p2;
	for (size_t i = 0;i < displayBytes; ++i) {
		int hn = (*b & 0xF0) >> 4;
		int ln = *b & 0x0F;
		*p++ = (hn < 10 ? '0' + hn : 'a' + hn - 10);
		*p++ = (ln < 10 ? '0' + ln : 'a' + ln - 10);
		++b;
	}
	if (displayBytes < getOccupancy()) {
		*p++ = '.';
		*p++ = '.';
		*p++ = '.';
	}
	*p++ = '\0';
	std::ostringstream os1;
	os1 << "Size=" << getOccupancy(); // std::hex << getOccupancy()
	os1 << " Hex=" << p2;
	return os1.str();
}
//------------------------------------------------------------------------------

void BinaryBuffer::Trace() const
{
	std::string str;

	str = asString(); // asStringBrief();
	::Trace("BinaryBuffer trace: %s", str.c_str());
}
//------------------------------------------------------------------------------

void BinaryBuffer::deleteBuffer()
{
  if (ownsMemory_) {
    ::free(pBuffer_);
    pBuffer_ = 0;
  }
}
//------------------------------------------------------------------------------

void BinaryBuffer::expandBuffer(size_t newSize)
{
  if (ownsMemory_) {
    size_t newBufferSize = bufferSize_ << 1;
    newBufferSize = (newSize > newBufferSize ? newSize : newBufferSize);

    // don't overwrite pBuffer_ yet -- it's used in
    // getOccupancy and getCursorPosition.
    // let realloc() do copy.  It may copy more bytes
    // than would be done if just using occupancy, but
    // it will also avoid the copy if possible, by
    // reallocating in place -- a trade-off
    void *newBuffer = ::realloc(pBuffer_, newBufferSize);
    if (!newBuffer) {
      DEF_Exception(AppException::NoMemoryError, "BinaryBuffer: failed to allocate memory");
    }
    // ::memcpy(newBuffer, pBuffer_, getOccupancy());
    pCursor_ = static_cast<char*>(newBuffer) + getCursorPosition();
    pOccupancy_ = static_cast<char*>(newBuffer) + getOccupancy();
    pBuffer_ = newBuffer;
    bufferSize_ = newBufferSize;
  }
  else {
    DEF_Exception(AppException::NoMemoryError, "BinaryBuffer: size tool small");
  }
}
//------------------------------------------------------------------------------
