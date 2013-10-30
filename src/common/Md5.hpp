/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Md5.hpp

*******************************************************************************/

#ifndef __Md5_HPP
#define __Md5_HPP
//------------------------------------------------------------------------------

#include <string>
//------------------------------------------------------------------------------

namespace Common
{
class BinaryBuffer;

//! Get the md5 sum of a string
void Md5String(std::string &outMd5, const std::string &inStr);
//! Get the md5 sum of a binary buffer
void Md5BinaryBuffer(std::string &outMd5, const BinaryBuffer &inBinaryBuffer);
//! Get the md5 sum of a file
void Md5File(std::string &outMd5, const std::string &inFullFileName);

} // namespace Common

//------------------------------------------------------------------------------
#endif
