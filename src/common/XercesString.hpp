/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XercesString.hpp

*******************************************************************************/

#ifndef __XercesString_HPP
#define	__XercesString_HPP
//------------------------------------------------------------------------------

#include "LibExport.hpp"
#include "unicode/unistr.h"
#include <xercesc/util/XMLString.hpp>
//------------------------------------------------------------------------------

namespace Common
{

//! Xerces string converter, from Xerces Xml string to C or Unicode string
/**
	For C to Xerces Xml string conversion, use XMLString::transcode
	XMLCh *str = XMLString::transcode("test");
	XMLString::release(&str);
 */
class XercesString
{
private :
  UChar *mUBuf;
	char *mLocalForm; //!< Local page encoded string
	unsigned int mUBufSize; //!< Unicode encoded string

	//! Reallocate buffer size to the new size
	void UBufAllocSize(unsigned int inSize);

public :
	DEF_Export XercesString();
	DEF_Export XercesString(const XMLCh* const toTranscode);
	DEF_Export ~XercesString();

	// Get the transcoded form
	DEF_Export const char* localForm() const { return mLocalForm; }
	DEF_Export const UChar *ConvertToUChar(const XMLCh* const inStr, int32_t inLength = -1);
};
//------------------------------------------------------------------------------

} // namespace Common

//------------------------------------------------------------------------------

#endif
