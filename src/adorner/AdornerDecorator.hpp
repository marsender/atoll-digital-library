/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

AdornerDecorator.hpp

*******************************************************************************/

#ifndef __AdornerDecorator_HPP
#define __AdornerDecorator_HPP
//------------------------------------------------------------------------------

#include "unicode/unistr.h"
//------------------------------------------------------------------------------

namespace Atoll
{
class Entry;
class Indexer;
class WordEntry;

//! Xml content word position decorator
/**
	Add word position to an xml document

	Logic:
		- Recieve xml nodes and text content from a parsed document
		- Parse the text content between nodes, separate words
		- Add xml nodes with word position around each word
*/
class AdornerDecorator
{
private:
	bool mIsBreak;
  UChar *mUBuf; //!< Content buffer
	unsigned int mUBufSize; //!< Content buffer size
	unsigned int mUBufPos; //!< Content buffer position
	Entry *mEntry; //!< Current word position
	UnicodeString mStrOutput; //!< Output string
	bool mWantDecoratorString; //!< Write the content in a string
	bool mWantDecoratorPosition; //!< Increment the position

	WordEntry *mWe;
	//WordEntryVector mWordEntryVector;

	Indexer *mDecoratorIndexer;

	//! Reallocate buffer size to the new size
	void UBufAllocSize(unsigned int inSize);

	//! Insert spaces in the output buffer
	void AddSpace(const UChar *inStr);
	//! Insert a text with it's position in the output buffer
	/**
		The text may not be a full word
	 */
	void AddText(const UChar *inStr);

public:
	//! Constructor
	AdornerDecorator();
	~AdornerDecorator();

	//! Get the decorator position
	void GetDecoratorPosition(Entry &outEntry) const;
	//! Init the decorator position
	void SetDecoratorPosition(const Entry &inEntry);

	//! Add text content to the decorator
	void AddContent(const UChar *inStr, int32_t inLength = -1);
	//! Add text content to the decorator
	void AddContent(const UnicodeString &inStr);
	//! Add an xml node to the decorator
	void AddNode(const UnicodeString &inStr);
	//! Add a processing instruction
	void AddProcessingInstruction(const UChar *inPiStr);

	//! Tell if the size is big enough to be good to decorate
	bool NeedDecorate();

	//! Decorate the content of the string buffer, and clears the buffer
	/**
		The buffer is splitted in words, separated by spaces or line feeds
		Each new word gets a new incremented position, and is added to
		the output string. Then the content buffer is cleared
	*/
	void DecorateContent();

	//! Tell if the decorator must increment it's position
	void EnableDecoratorPosition(bool inEnable);

	//! Tell if the decorator must output it's content in a string
	void EnableDecoratorString(bool inEnable);

	//! Get a reference on the decorator output string
	UnicodeString &GetDecoratorString() { return mStrOutput; }

	//! Get the word entry vector
	//WordEntryVector *GetWordEntryVector() { return &mWordEntryVector; }

	//! Set the indexer for the adorner
	void SetDecoratorIndexer(Indexer *inIndexer);
	//! Index a full word
	void IndexWord();

	//! Clear the decorator content
	void Clear();
};

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
