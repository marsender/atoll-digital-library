/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XercesString.cpp

*******************************************************************************/

#include "XercesString.hpp"
#include "unicode/ustring.h" // For u_memcpy
//------------------------------------------------------------------------------

#define DEF_DefaultBufSize 100
//------------------------------------------------------------------------------

using namespace Common;
XERCES_CPP_NAMESPACE_USE

XercesString::XercesString()
{
	mUBuf = NULL;
	mLocalForm = NULL;
	mUBufSize = 0;

	// Unicode buffer default size allocation
	UBufAllocSize(DEF_DefaultBufSize);
}
//------------------------------------------------------------------------------

XercesString::XercesString(const XMLCh* const toTranscode)
{
	mUBuf = NULL;
	mLocalForm = NULL;
	mUBufSize = 0;

	// Call of the Xerces static transcodage function
	if (toTranscode)
		mLocalForm = XMLString::transcode(toTranscode);
}
//------------------------------------------------------------------------------

XercesString::~XercesString()
{
	if (mUBuf)
		::free(mUBuf);

	// Call of the Xerces static memory free function
	if (mLocalForm)
		XMLString::release(&mLocalForm);
}
//------------------------------------------------------------------------------

const UChar *XercesString::ConvertToUChar(const XMLCh* const inStr, int32_t inLength /*= -1*/)
{
	unsigned int len = inLength == -1 ? (unsigned int)XMLString::stringLen(inStr) : inLength;
	if (len > mUBufSize)
		UBufAllocSize(len + DEF_DefaultBufSize);

	if (sizeof(XMLCh) == sizeof(UChar)) {
		//res = (const UChar *)inStr;
		u_memcpy(mUBuf, inStr, len);
		mUBuf[len] = 0;
	}
	else {
		const XMLCh *src = inStr;
		UChar *dst = mUBuf;
		while (len--)
			*dst++ = UChar(*src++);
		*dst = 0;
		//res = mUBuf;
	}

	return mUBuf;
}
//------------------------------------------------------------------------------

void XercesString::UBufAllocSize(unsigned int inSize)
{
	mUBufSize = inSize;
	mUBuf = (UChar *)::realloc(mUBuf, sizeof(UChar) * (mUBufSize + 1));
}
//------------------------------------------------------------------------------
