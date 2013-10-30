/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Containers.hpp

*******************************************************************************/

#ifndef __Containers_HPP
#define __Containers_HPP
//------------------------------------------------------------------------------

#include <set>
#include <map>
#include <vector>
#include <string>
#include "unicode/unistr.h"
//------------------------------------------------------------------------------

namespace Common
{

//! UnicodeString Compare function
struct fCmpUnicodeString : std::binary_function<UnicodeString, UnicodeString, bool>
{
	// UnicodeString Compare function
  bool operator() (const UnicodeString &s1, const UnicodeString &s2) const;
};
//------------------------------------------------------------------------------

//! Standard string Set
typedef std::set < std::string > StdStringSet;

//! UnicodeString Set
typedef std::set < UnicodeString, fCmpUnicodeString, std::allocator<UnicodeString> > UnicodeStringSet;

//! UnicodeString Vector
typedef std::vector < UnicodeString > UnicodeStringVector;

//! Integer Vector
typedef std::vector < unsigned int > IntVector;

//! String to UnicodeString Map
typedef std::map < std::string, UnicodeString > StringToUnicodeStringMap;

//! Integer to UnicodeString Map
typedef std::map < unsigned int, UnicodeString > IntToUnicodeStringMap;

//! UnicodeString to int Map
typedef std::map < UnicodeString, unsigned int > UnicodeStringToIntMap;

//! Integer Map
typedef std::map < std::string, unsigned int > StringToIntMap;
//------------------------------------------------------------------------------

//! Compare two Unicode strings
int32_t CollatorStrcmp(const UChar *s1, const UChar *s2);
//! Compare two Unicode strings (at most n characters)
int32_t CollatorStrncmp(const UChar *s1, const UChar *s2, int32_t n);

//! Get the set of words in a string. Warning: the initial string is destroyed
size_t UnicodeStringGetWords(UnicodeStringSet &outStringSet, UnicodeString &inStr);

//! Get the set of sub-words in a single word. Warning: the initial string is destroyed
/**
	Cut the word in parts, separated by '-' or '.' etc...
	then clear each part from it's punctuation, parenthesis etc...
*/
size_t UnicodeStringGetSubWords(UnicodeStringSet &outStringSet, UnicodeString &inStr);

//! Delete functor, used to delete pointers. Can be used with any type
/**
	Example:
		std::list<int*> l;
		l.push_back(new int(5));
		std::for_each(l.begin(), l.end(), DeleteFunctor());
*/
struct DeleteFunctor
{
	template <class T> void operator()(T*& p) const
	{
		delete p;
		p = NULL;
	}
};
//------------------------------------------------------------------------------

} // namespace Common

//------------------------------------------------------------------------------
#endif
