/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

AdornerDecorator.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "AdornerDecorator.hpp"
#include "../common/UnicodeUtil.hpp"
#include "../indexer/Indexer.hpp"
#include "../util/Entry.hpp"
#include "../util/WordEntry.hpp"
#include "unicode/ustring.h"
#include "unicode/utext.h"
#include "unicode/ubrk.h"
#include "unicode/ustdio.h"
//------------------------------------------------------------------------------

#define DEF_DefaultBufSize      0xFFFu	// 4095
#define DEF_NeedDecorateSize    0xFFFFu	// 65535

#define DEF_Log(x) { gLog.log(eTypLogError, "Err > AdornerDecorator: %s", x); }
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

AdornerDecorator::AdornerDecorator()
{
	// Size allocation for the buffer
	mUBuf = NULL;
	UBufAllocSize(DEF_DefaultBufSize);
	mEntry = new Entry();
	mWe = new WordEntry();
	Clear();

	mDecoratorIndexer = NULL;
	mWantDecoratorString = false;
	mWantDecoratorPosition = true;
}
//------------------------------------------------------------------------------

AdornerDecorator::~AdornerDecorator()
{
	// Check for errors (or mistakes)
	if (!mStrOutput.isEmpty()) {
		// The string should have been used and cleared
		std::string str = UnicodeString2String(mStrOutput);
		gLog.log(eTypLogError, "Err > AdornerDecorator: Output string not empty: %s", str.c_str());
		//DEF_Log("Output string not empty");
	}
	if (!mWe->mWrd.isEmpty()) {
		//const UChar *sDebug = mWe->mWrd.getBuffer();
		// The word entry should have been used and cleared
		DEF_Log("Word entry string not empty");
	}

	::free(mUBuf);
	delete mEntry;
	delete mWe;
}
//------------------------------------------------------------------------------

void AdornerDecorator::Clear()
{
	mUBufPos = 0;
	*mUBuf = 0;
	mIsBreak = true;
	mStrOutput.remove();
	mEntry->Set(0, 0, 0);
	mWe->mEntry.Set(0, 0, 0);
	mWe->mWrd.remove();
}
//------------------------------------------------------------------------------

void AdornerDecorator::GetDecoratorPosition(Entry &outEntry) const
{
	outEntry = *mEntry;
}
//------------------------------------------------------------------------------

void AdornerDecorator::SetDecoratorPosition(const Entry &inEntry)
{
	*mEntry = inEntry;
}
//------------------------------------------------------------------------------

void AdornerDecorator::UBufAllocSize(unsigned int inSize)
{
	mUBufSize = inSize;
	mUBuf = (UChar *)::realloc(mUBuf, sizeof(UChar) * (mUBufSize + 1));
}
//------------------------------------------------------------------------------

void AdornerDecorator::EnableDecoratorPosition(bool inEnable)
{
	mWantDecoratorPosition = inEnable;
}
//------------------------------------------------------------------------------

void AdornerDecorator::EnableDecoratorString(bool inEnable)
{
	mWantDecoratorString = inEnable;
}
//------------------------------------------------------------------------------

void AdornerDecorator::IndexWord()
{
	// Add currently indexed nodes
	if (mDecoratorIndexer) {
		//const UChar *sDebug = mWe->mWrd.getTerminatedBuffer();
		if (!mWe->mWrd.isEmpty()) {
			if (mWantDecoratorPosition)
				mDecoratorIndexer->IndexerAddWordEntry(*mWe);
			else {
				unsigned long bakPos = mWe->mEntry.mUlEntPos;
				mWe->mEntry.mUlEntPos = 1;
				mDecoratorIndexer->IndexerAddWordEntry(*mWe);
				mWe->mEntry.mUlEntPos = bakPos;
			}
			mWe->mWrd.remove();
		}
		mDecoratorIndexer->StoreNodeListToDb(false);
	}
}
//------------------------------------------------------------------------------

void AdornerDecorator::AddSpace(const UChar *inStr)
{
	if (mWantDecoratorString) {
		UnicodeStringAddXmlEntities(mStrOutput, inStr);
	}
}
//------------------------------------------------------------------------------

void AdornerDecorator::AddText(const UChar *inStr)
{
	if (*inStr == 0) {
		DEF_Log("AddText with empty word");
		return;
	}

	if (mWantDecoratorString) {
		UChar buf[50];
		// Add the position
		u_sprintf(buf, "<w pos=\"%lu\">", mEntry->mUlEntPos);
		mStrOutput += buf;
		// Add the text
		mStrOutput += inStr;
		// Close the position
		mStrOutput += "</w>";
	}

	/*
	UnicodeString strDebug(inStr);
	if (strDebug == "fruits")
		inStr = inStr;
	*/

	if (mDecoratorIndexer) {
		// Add word entry only if the position has changed
		if (mEntry->mUlEntPos > mWe->mEntry.mUlEntPos) {
			if (mWe->mEntry.mUlEntPos) {
				//const UChar *sDebug = mWe->mWrd.getTerminatedBuffer();
				IndexWord();
			}
			mWe->mEntry = *mEntry;
		}
		mDecoratorIndexer->IndexerActivateNodes();
		mWe->mWrd += inStr;
	}

	bool wantTrace = false;
	if (wantTrace) {
		std::string strTmp = mWe->ToString();
		Trace("AdornerDecorator::AddText [word] %s", strTmp.c_str());
	}
}
//------------------------------------------------------------------------------

void AdornerDecorator::AddContent(const UChar *inStr, int32_t inLength /*= -1*/)
{
	// Check the buffer size
	int32_t len = inLength == -1 ? u_strlen(inStr) : inLength;
	if (len + mUBufPos > mUBufSize)
		UBufAllocSize(len + mUBufPos + DEF_DefaultBufSize);

	// Append the content to the end of the string buffer
	u_memcpy(mUBuf + mUBufPos, inStr, len);
	mUBufPos += len;
	mUBuf[mUBufPos] = 0;
}
//------------------------------------------------------------------------------

void AdornerDecorator::AddContent(const UnicodeString &inStr)
{
	AddContent(inStr.getBuffer(), inStr.length());
}
//------------------------------------------------------------------------------

void AdornerDecorator::AddNode(const UnicodeString &inStr)
{
	// Decorate before anything
	DecorateContent();

	// Add the xml node to the output without change
	if (mWantDecoratorString) {
		//const UChar *sDebug = inStr.getBuffer();
		mStrOutput += inStr;
	}
}
//------------------------------------------------------------------------------

void AdornerDecorator::AddProcessingInstruction(const UChar *inPiStr)
{
	// Decorate before anything
	DecorateContent();

	// Add the xml node to the output without change
	if (mWantDecoratorString) {
		mStrOutput += "<?";
		mStrOutput += inPiStr;
		mStrOutput += "?>";
	}
}
//------------------------------------------------------------------------------

bool AdornerDecorator::NeedDecorate()
{
	if (mUBufPos > DEF_NeedDecorateSize)
		return true;

	return false;
}
//------------------------------------------------------------------------------

void AdornerDecorator::SetDecoratorIndexer(Indexer *inIndexer)
{
	if (mDecoratorIndexer) {
		IndexWord();
	}
	mDecoratorIndexer = inIndexer;
}
//------------------------------------------------------------------------------

void AdornerDecorator::DecorateContent()
{
	if (mUBufPos == 0) {
		//if (mDecoratorIndexer && mIsBreak)
		//	mDecoratorIndexer->StoreNodeListToDb(true);
		return;
	}

	UText *ut = NULL;
	UBreakIterator *bi = NULL;
	UErrorCode status = U_ZERO_ERROR;

	ut = utext_openUChars(ut, mUBuf, mUBufPos, &status);
	bi = ubrk_open(UBRK_WORD, "fr_fr", NULL, 0, &status);

	int32_t deb = 0, next;
	UChar cBak;
	unsigned int nbWord = 0;
	ubrk_setUText(bi, ut, &status);
	while (true) {
		next = ubrk_next(bi);
		if (next == UBRK_DONE)
			break;
		// Add spaces
		if (ubrk_getRuleStatus(bi) == UBRK_WORD_NONE) {
			if (!mIsBreak) {
				mIsBreak = true;
				//if (nbWord == 0)
				//	IndexWord();
			}
			cBak = *(mUBuf + next);
			*(mUBuf + next) = 0;
			AddSpace(mUBuf + deb);
			*(mUBuf + next) = cBak;
			deb = next;
			continue;
		}
		// If there is a space waiting: increment the word pos
		if (mIsBreak) {
			mIsBreak = false;
			mEntry->mUlEntPos++;
			nbWord++;
		}
		// Add the word in the word list
		cBak = *(mUBuf + next);
		*(mUBuf + next) = 0;
		AddText(mUBuf + deb);
		*(mUBuf + next) = cBak;
		deb = next;
	}
	utext_close(ut);
	ubrk_close(bi);
	AppAssert(U_SUCCESS(status));

	// Clear the buffer content
	mUBufPos = 0;
	*mUBuf = 0;
}
//------------------------------------------------------------------------------
