/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

WordEntries.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "WordEntries.hpp"
#include "../common/Containers.hpp"
#include "../common/UnicodeUtil.hpp"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

std::ostream& Atoll::operator<<(std::ostream& inStream, const WordEntries &inWordEntries)
{
	inStream << inWordEntries.ToString();
	return inStream;
}
//------------------------------------------------------------------------------

bool fCmpWordEntries::operator() (const WordEntries& w1, const WordEntries& w2) const
{
	fCmpUnicodeString compStruct;

	return compStruct(w1.mWrd, w2.mWrd);
}
//------------------------------------------------------------------------------

WordEntries::WordEntries(const UChar *inStr)
{
	if (inStr)
		mWrd = inStr;
}
//------------------------------------------------------------------------------

void WordEntries::AddEntry(const Entry &inEntry)
{
	mEntrySet.insert(inEntry);
}
//------------------------------------------------------------------------------

void WordEntries::AddEntry_ConstHack(const Entry &inEntry) const
{
	EntrySet *es = (EntrySet *)&mEntrySet;
	es->insert(inEntry);
}
//------------------------------------------------------------------------------

void WordEntries::AddEntries_ConstHack(const EntrySet &inEntrySet) const
{
	EntrySet *es = (EntrySet *)&mEntrySet;

	EntrySet::const_iterator it = inEntrySet.begin();
	EntrySet::const_iterator itEnd = inEntrySet.end();
	for (; it != itEnd; ++it) {
		const Entry &e = *it;
		es->insert(e);
	}
}
//------------------------------------------------------------------------------

std::string WordEntries::ToString() const
{
	std::string s("WordEntries: ");

	s += UnicodeString2String(mWrd);
	s += " - ";
	s += EntrySet2String(mEntrySet);

	return s;
}
//------------------------------------------------------------------------------
