/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

BufUtil.hpp

*******************************************************************************/

#ifndef __BufUtil_HPP
#define __BufUtil_HPP
//------------------------------------------------------------------------------

// Sizes:
//   2 bytes for unsigned int 0xFFFF
//   4 bytes for unsigned long 0xFFFFFFFF
#define DEF_SizeOfInt    2
#define DEF_SizeOfLong   4
#define DEF_IntMax       0xffff     // 65535 != UINT_MAX
#define DEF_LongMax      0xffffffff // 4294967295 != ULONG_MAX

namespace Common
{

extern "C" {

// Long integer between 0 and 0xFFFFFFFF = DEF_LongMax = 4 294 967 295
inline unsigned long GetLgBuf(const char *inBuf)
{
  return
		((((unsigned char)inBuf[0] << 24)) & 0xFF000000) |
		((((unsigned char)inBuf[1] << 16)) & 0x00FF0000) |
		((((unsigned char)inBuf[2] << 8))  & 0x0000FF00) |
		((((unsigned char)inBuf[3]))       & 0x000000FF);
}
//------------------------------------------------------------------------------

// Long integer between 0 and 0xFFFFFFFF = DEF_LongMax = 4 294 967 295
inline void SetLgBuf(char *outBuf, unsigned long inUl)
{
  outBuf[0] = (unsigned char)(inUl >> 24) & 0xFF;
  outBuf[1] = (unsigned char)(inUl >> 16) & 0xFF;
  outBuf[2] = (unsigned char)(inUl >> 8) & 0xFF;
  outBuf[3] = (unsigned char)inUl & 0xFF;
}
//------------------------------------------------------------------------------

// Integer between 0 and 0xFFFF = 65535
inline unsigned int GetShBuf(const char *inBuf)
{
  return
		((((unsigned char)inBuf[0] << 8))  & 0x0000FF00) |
		((((unsigned char)inBuf[1]))       & 0x000000FF);
}
//------------------------------------------------------------------------------

// Integer between 0 and 0xFFFF = 65535
inline void SetShBuf(char *outBuf, unsigned int inUi)
{
  outBuf[0] = (unsigned char)(inUi >> 8) & 0xFF;
  outBuf[1] = (unsigned char)inUi & 0xFF;
}
//------------------------------------------------------------------------------

}; // extern "C"

} // namespace Common

//------------------------------------------------------------------------------
#endif
