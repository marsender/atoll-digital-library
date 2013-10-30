/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

IndexerItem.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "IndexerItem.hpp"
#include "../common/StdString.hpp"
#include "../common/UnicodeUtil.hpp"
#include "unicode/ustring.h"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

#define DEF_Log(x) { gLog.log(eTypLogError, "Err > IndexerItem: %s", x); }

IndexerItem::IndexerItem(const std::string &inIndexDbName, eTypIndex inTypIndex, bool isLowerCase)
	:mIndexDbName(inIndexDbName),
	mTypIndex(inTypIndex),
	mIsLowerCase(isLowerCase)
{
	AppAssert(mTypIndex > eTypIndexNone && mTypIndex < eTypIndexEnd && mTypIndex != eTypIndexNothing);
}
//------------------------------------------------------------------------------

IndexerItem::~IndexerItem()
{
	// Check for errors (or mistakes)
	if (!mWordEntryVector.empty()) {
		// The list should have been stored to the set
		DEF_Log("Destructor: list not empty");
	}
}
//------------------------------------------------------------------------------

WordEntriesSet *IndexerItem::GetWordEntriesSetToIndex()
{
	mWordEntriesSet.clear();

	size_t count = mWordEntryVector.size();
	if (count) {
		// Store individual words of the list in a set
		WordEntryVector::iterator it = mWordEntryVector.begin();
		WordEntryVector::iterator itEnd = mWordEntryVector.end();
		for (; it != itEnd; ++it) {
			WordEntry *we = &(*it);
//#ifdef DEF_EnableAsserts
			if (we->mEntry.mUlEntPos == 0) {
				DEF_Log("Compute a word entry with an empty pos");
				continue;
			}
//#endif
			WordEntryToSet(*we);
		}
		// Clear the list
		mWordEntryVector.clear();
	}

	return &mWordEntriesSet;
}
//------------------------------------------------------------------------------

bool IndexerItem::GetFieldToIndex(UnicodeString &outStr)
{
	outStr = mField;

	// Clear line feeds and trims the field
	UnicodeStringClearCrLf(outStr);

	if (mIsLowerCase)
		outStr.toLower();

	if (outStr.isEmpty())
		return false;

	// If any position is empty, the field must also be empty
	if (mEntryStart.mUlEntPos == 0 || mEntryEnd.mUlEntPos == 0) {
		bool isOnlySpace = true;
		UnicodeStringSet tmpSet;
		UnicodeString tmpStr = outStr;
		// Warning: the word will be destroyed
		size_t count = Common::UnicodeStringGetSubWords(tmpSet, tmpStr);
		if (count) {
			UnicodeStringSet::const_iterator it = tmpSet.begin();
			UnicodeStringSet::const_iterator itEnd = tmpSet.end();
			for (; it != itEnd; ++it) {
				tmpStr = (*it);
				if (!UnicodeStringIsOnlySpace(tmpStr.getTerminatedBuffer())) {
					UTrace("Incorrect field", tmpStr.getTerminatedBuffer());
					isOnlySpace = false;
					break;
				}
			}
		}
		if (!isOnlySpace) {
			std::string str;
			ConvertUnicodeString2String(str, outStr);
			if (mEntryStart.mUlEntPos == 0)
				gLog.log(eTypLogError, "Err > IndexerItem: Compute a field with an empty start pos '%s'", str.c_str());
			else
				gLog.log(eTypLogError, "Err > IndexerItem: Compute a field with an empty end pos '%s'", str.c_str());
		}
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------

void IndexerItem::WordEntryToSet(WordEntry &inWordEntry)
{
	bool wantTrace = false;
	bool wantSubWords = true;
	size_t count;
	
	if (wantSubWords) {
		// Warning: the word will be destroyed
		count = UnicodeStringGetSubWords(mTmpSet, inWordEntry.mWrd);
	}
	else {
		mTmpSet.clear();
		mTmpSet.insert(inWordEntry.mWrd);
		count = mTmpSet.size();
	}
	if (count == 0)
		return;

	WordEntries wordEntries;
	WordEntriesSet::const_iterator itWes;
	UnicodeStringSet::const_iterator it = mTmpSet.begin();
	UnicodeStringSet::const_iterator itEnd = mTmpSet.end();
	for (; it != itEnd; ++it) {
		wordEntries.mWrd = (*it);
		if (mIsLowerCase)
			wordEntries.mWrd.toLower();
		// Add the word in the set
		// If the word already exists, add only it's new position
//gLog.log(eTypLogDebug, "crash 1 %u", mWordEntriesSet.size());
		itWes = mWordEntriesSet.find(wordEntries);
//gLog.log(eTypLogDebug, "crash 2");
		if (itWes == mWordEntriesSet.end()) {
			wordEntries.AddEntry(inWordEntry.mEntry);
			mWordEntriesSet.insert(wordEntries);
		}
		else {
			itWes->AddEntry_ConstHack(inWordEntry.mEntry);
		}
		if (wantTrace) {
			WordEntry wordEntry(wordEntries.mWrd, inWordEntry.mEntry);
			std::string strTmp = wordEntry.ToString();
			Trace("IndexerItem::WordEntryToSet - IndexDbName: %s - %s", mIndexDbName.c_str(), strTmp.c_str());
		}
	}
}
//------------------------------------------------------------------------------

void IndexerItem::IiAddWordEntry(WordEntry &inWordEntry)
{
//#ifdef DEF_EnableAsserts
	if (inWordEntry.mWrd.isEmpty()) {
		DEF_Log("Try to add an empty word");
		return;
	}
	if (inWordEntry.mEntry.mUlEntPos == 0) {
		DEF_Log("Try to add a word with an empty pos");
		return;
	}
	if (UnicodeStringHasSpace(inWordEntry.mWrd)) {
		DEF_Log("Try to add a word with space");
		return;
	}
//#endif

	switch (mTypIndex) {
	case eTypIndexWord:
		/*{
			const UChar *sDebug = inWordEntry.mWrd.getBuffer();
			sDebug = sDebug;
		}*/
		mWordEntryVector.push_back(inWordEntry);
		break;
	case eTypIndexField:
	case eTypIndexInterv:
		if (mEntryStart.IsEmpty()) {
			mEntryStart = inWordEntry.mEntry;
		}
		mEntryEnd = inWordEntry.mEntry;
		break;
	default:
		DEF_Log("Undefined index type");
		break;
	}
}
//------------------------------------------------------------------------------

void IndexerItem::IiAddFieldContent(const UChar *inStr, unsigned int inLength)
{
	if (mTypIndex != eTypIndexField)
		return;

	mField.append(inStr, inLength);
}
//------------------------------------------------------------------------------

std::string IndexerItem::ToString() const
{
	std::string s("IndexerItem:");

	s += " - Index db name: ";
	s += mIndexDbName;
	s += " - ";
	switch (mTypIndex) {
	case eTypIndexWord:
		s += "IndexWord";
		break;
	case eTypIndexField:
		s += "IndexField";
		break;
	case eTypIndexInterv:
		s += "IndexInterv";
		break;
	default:
		DEF_Exception(AppException::InvalidValue, "IndexerItem type of index");
	}
	s += " - Lower case: ";
	StdString::AppendBool(s, mIsLowerCase);
	if (mField.length()) {
		s += " - Field: ";
		s += UnicodeString2String(mField);
	}
	if (!mEntryStart.IsEmpty()) {
		s += " - Entry start: ";
		s += mEntryStart.ToString();
		s += " - Entry end: ";
		s += mEntryEnd.ToString();
	}

	return s;
}
//------------------------------------------------------------------------------
