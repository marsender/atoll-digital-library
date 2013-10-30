/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

See the file LicenseBerkeleyDB.txt for redistribution information.
Copyright (c) 2002,2006 Oracle. All rights reserved.

BinaryBuffer.hpp

*******************************************************************************/

#ifndef __BinaryBuffer_HPP
#define	__BinaryBuffer_HPP
//------------------------------------------------------------------------------

#include <string> // For typedef unsigned long size_t;
#include <vector>
//------------------------------------------------------------------------------

namespace Common
{

//! General purpose binary buffer
class BinaryBuffer
{
public:
  //! Default constructor. Use Set to construct the buffer.
  BinaryBuffer();
  //! Construct from a buffer and size. BinaryBuffer copies this into its own memory.
  BinaryBuffer(const void *p, size_t size);
  //! Construct from a BinaryBuffer, with some extra space. BinaryBuffer copies this into its own memory.
  BinaryBuffer(const BinaryBuffer& buffer, size_t extra = 0);
  //! Construct from a BinaryBuffer and size.  BinaryBuffer points to this memory.
	/**
	 * If wrapper is true, buffer can't expand. BinaryBuffer won't deallocate this.
	 */
  BinaryBuffer(const void *p, size_t size, bool wrapper);
  //! Default destructor
  ~BinaryBuffer();

  //! Assignment operator
  BinaryBuffer &operator=(const BinaryBuffer& buffer);

  //! Get the size of the buffer in bytes.
  size_t getSize() const;
  //! Get the current cursor position as a bytes offset.
  size_t getCursorPosition() const;
  //! Set the buffer cursor to the start of the buffer.
  void resetCursorPosition();
  //! Returns the amount of buffer used. The highest position reached by the cursor.
  size_t getOccupancy() const;
  // Set the buffer occupancy to zero.
  void resetOccupancy();
  //! Return the buffer to its before use state. (Cursor==0 && Occupancy==0)
  void reset();
  //! Get a pointer to the internal buffer
  void *getBuffer(size_t offset = 0) const;
  //! Get a pointer to the internal buffer and donate it to the caller
  void *donateBuffer(size_t offset = 0);
  //! Get a pointer to the cursor position
  void *getCursor() const;

  //! How much data is remaining between the cursor and the occupancy.
  size_t getRemaining() const;

  //! Seek n bytes into buffer p from the current position.
  size_t seek(size_t n)
  {
    return readSeek(0, n);
  }
  //! Read n bytes into buffer p from the current position.
  size_t read(void *p, size_t n)
  {
    return readSeek(p, n);
  }
  //! Write n bytes from buffer p to the current position.
  size_t write(const void *p, size_t n);

  //! Reserve a space of size n in the buffer, returning the offset to it
  size_t reserve(size_t &offset, size_t n);
  //! Read n bytes into buffer p from the given offset
  size_t read(size_t offset, void* p, size_t n);
  //! Write n bytes from buffer p to the given offset
  size_t write(size_t offset, const void *p, size_t n);

  //! Comparison operator
  int operator==(const BinaryBuffer& buffer) const;
  int operator!=(const BinaryBuffer& buffer) const;
  bool operator<(const BinaryBuffer& buffer) const;

  //! Debug method to get a text representation of the buffer
  std::string asString(bool textOnly = false) const;
  std::string asStringBrief() const;
	void Trace() const;

private:
  void deleteBuffer();
  void expandBuffer(size_t amount);
  size_t readSeek(void *p, size_t n);

  size_t bufferSize_;
  void* pBuffer_; //!< Pointer to the begining of the buffer
  void* pCursor_; //!< Pointer to the cursor position = the next position to write (may be different as occupancy)
  void* pOccupancy_; //!< Pointer to the end of the buffer
  bool ownsMemory_;

  //! Set the buffer contents from a buffer and size. Used for delayed construction. Warning: this doesn't move the cursor position
  void set(const void *p, size_t nSize);
};
//------------------------------------------------------------------------------

inline size_t BinaryBuffer::getSize() const
{
  return bufferSize_;
}
//------------------------------------------------------------------------------

inline void BinaryBuffer::resetCursorPosition()
{
  pCursor_ = pBuffer_;
}
//------------------------------------------------------------------------------

inline void *BinaryBuffer::getBuffer(size_t offset) const
{
  return (void*)((char*)pBuffer_ + offset);
}
//------------------------------------------------------------------------------

// this will only work if the caller is using
// ::free to free the memory
inline void* BinaryBuffer::donateBuffer(size_t offset)
{
  //AppAssert(ownsMemory_ && (offset == 0));
  void *ret = (void*)((char*)pBuffer_ + offset);
  ownsMemory_ = false;
  return ret;
}
//------------------------------------------------------------------------------

inline void* BinaryBuffer::getCursor() const
{
  return pCursor_;
}
//------------------------------------------------------------------------------

inline int BinaryBuffer::operator!=(const BinaryBuffer& buffer) const
{
  return !operator==(buffer);
}
//------------------------------------------------------------------------------

inline void BinaryBuffer::resetOccupancy()
{
  pOccupancy_ = pBuffer_;
}
//------------------------------------------------------------------------------

inline void BinaryBuffer::reset()
{
  resetCursorPosition();
  resetOccupancy();
}
//------------------------------------------------------------------------------

inline size_t BinaryBuffer::getRemaining() const
{
  return static_cast<char*>(pOccupancy_) - static_cast<char*>(pCursor_);
}
//------------------------------------------------------------------------------

typedef std::vector <Common::BinaryBuffer *> BinaryBufferVector;
//------------------------------------------------------------------------------

} // namespace Common

//------------------------------------------------------------------------------
#endif
