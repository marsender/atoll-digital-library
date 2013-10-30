/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

SearchCrit.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "SearchCrit.hpp"
#include "../common/BinaryBuffer.hpp"
#include "../common/UnicodeUtil.hpp"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

std::ostream& Atoll::operator<<(std::ostream& inStream, const SearchCrit &inSearchCrit)
{
	inStream << inSearchCrit.ToString();
	return inStream;
}
//------------------------------------------------------------------------------

void Atoll::SearchCritToString(std::string &outStr, const SearchCrit &inSearchCrit)
{
	std::string str;

	ConvertUnicodeString2String(outStr, inSearchCrit.GetIdCrit());
	outStr += ' ';
	ConvertUnicodeString2String(str, inSearchCrit.mWrd);
	outStr += str;
}
//------------------------------------------------------------------------------

void Atoll::StringToSearchCrit(const std::string &inStr, SearchCrit &outSearchCrit)
{
	const char *s = inStr.c_str();
	char *pos = (char *)strchr(s, ' ');

	if (pos == NULL) {
		outSearchCrit.Clear();
		gLog.log(eTypLogError, "String to search crit: incorrect format");
		return;
	}

	*pos = 0;
	outSearchCrit.SetIdCrit(s);
	*pos = ' ';

	ConvertString2UnicodeString(outSearchCrit.mWrd, pos + 1);
}
//------------------------------------------------------------------------------

void Atoll::SearchCritVectorToString(std::string &outStr, const SearchCritVector &inSearchCritVector)
{
	outStr = "";
	SearchCritVector::const_iterator it = inSearchCritVector.begin();
	SearchCritVector::const_iterator itEnd = inSearchCritVector.end();
  for (; it != itEnd; ++it) {
		const SearchCrit &sc = *it;
		if (outStr.length())
			outStr += " - ";
		// Add the search criteria
		outStr += "[";
		outStr += sc.ToString();
		outStr += "]";
	}
}
//------------------------------------------------------------------------------

size_t Atoll::SearchCritVectorToBuf(BinaryBuffer &outBuffer, const SearchCritVector &inSearchCritVector)
{
	std::string str;
	unsigned int size, length;
	char buf[DEF_SizeOfInt];

	// Raz output binary buffer
	outBuffer.reset();

	// Number of search crit
	size = inSearchCritVector.size();
	SetShBuf(buf, size);
	outBuffer.write(buf, DEF_SizeOfInt);

	SearchCritVector::const_iterator it = inSearchCritVector.begin();
	SearchCritVector::const_iterator itEnd = inSearchCritVector.end();
  for (; it != itEnd; ++it) {
		const SearchCrit &sc = *it;
		// Convert the search criteria to a string
		SearchCritToString(str, sc);
		// Write the search criteria
		length = str.length();
		SetShBuf(buf, length);
		outBuffer.write(buf, DEF_SizeOfInt);
		outBuffer.write(str.c_str(), length);
	}

	//outBuffer.Trace();

	return size;
}
//------------------------------------------------------------------------------

size_t Atoll::BufToSearchCritVector(SearchCritVector &outSearchCritVector, const void *inData)
{
	std::string str;
	unsigned int size, length;

	outSearchCritVector.clear();

	// Nothing to do if no data is given
	if (inData == NULL)
		return 0;

	// Get the total number of search crit
	const char *pos = (const char *)inData;
	size = GetShBuf(pos);
	pos += DEF_SizeOfInt;

  // Read the search criteria and add them in the output vector
  SearchCrit sc;
  for (unsigned long i = 0; i < size; i++) {
		// Read the search criteria string
		length = GetShBuf(pos);
		pos += DEF_SizeOfInt;
		str.assign(pos, length);
		pos += length;
		// Convert the string to a search criteria
    StringToSearchCrit(str, sc);
    outSearchCritVector.push_back(sc);
  }

	return size;
}
//------------------------------------------------------------------------------

SearchCrit::SearchCrit()
{
}
//------------------------------------------------------------------------------

SearchCrit::SearchCrit(const SearchCrit &inSearchCrit)
{
	mIdCrit = inSearchCrit.mIdCrit;
	mWrd = inSearchCrit.mWrd;
}
//------------------------------------------------------------------------------

UnicodeString SearchCrit::GetIdCrit() const
{
	return mIdCrit;
}
//------------------------------------------------------------------------------

void SearchCrit::SetIdCrit(const char *inId)
{
	ConvertString2UnicodeString(mIdCrit, inId);
}
//------------------------------------------------------------------------------

void SearchCrit::SetIdCrit(const UnicodeString &inId)
{
	mIdCrit = inId;
}
//------------------------------------------------------------------------------

void SearchCrit::Clear()
{
	mIdCrit.remove();
	mWrd.remove();
}
//------------------------------------------------------------------------------

std::string SearchCrit::ToString() const
{
	std::string s("SearchCrit: ");

	s += UnicodeString2String(mIdCrit);
	s += " - ";
	s += UnicodeString2String(mWrd);

	return s;
}
//------------------------------------------------------------------------------
