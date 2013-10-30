/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Zlib.hpp

*******************************************************************************/

#ifndef __Zlib_HPP
#define __Zlib_HPP
//------------------------------------------------------------------------------

#include <string> // For size_t;
//------------------------------------------------------------------------------

namespace Common
{
class BinaryBuffer;

//! Compress a string into a binary buffer, at it's current position
bool ZlibCompress(BinaryBuffer &outBinaryBuffer, const char *inSrc, size_t inSrcSize);
//! Deflate a compressed buffer into a string, alocating destination buffer
bool ZlibDeflate(char *&outDest, unsigned int inDestSize, void *inData, size_t inDataSize);

} // namespace Common

//------------------------------------------------------------------------------
#endif
