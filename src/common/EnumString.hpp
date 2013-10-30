/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

EnumString.hpp

A utility to provide stringizing support for C++ enums
Author: Francis Xavier Joseph Pulikotil
Site: http://www.gamedev.net/reference/snippets/features/cppstringizing/

Usage example:

namespace Common
{
	// Enumeration
	enum eWeekEnd {
		Saturday = 6,
		Sunday = 7,
		eTest = 100
	};

	// String support for enumeration
	EnumStringBegin(eWeekEnd) {
		EnumStringAdd(Saturday); // or RegisterEnumerator(Saturday, "Saturday");
		EnumStringAdd(Sunday); // RegisterEnumerator(Sunday, "Sunday");
		RegisterEnumerator(eTest, "The test");
	}
	EnumStringEnd;
} // namespace Common

{
	// Convert from enum to string
	const std::string &str = EnumString<eWeekEnd>::From(Saturday);
	// str should now be "Saturday"
	assert(str.compare("Saturday") == 0);

	// Convert from string to enum
	eWeekEnd w;
	const bool isOk = EnumString<eWeekEnd>::To(w, "Sunday"); // or To(w, str);
	// w should now be Sunday
	assert(isOk);
	assert(w == Sunday);
}

*******************************************************************************/

#ifndef __EnumString_HPP
#define __EnumString_HPP
//------------------------------------------------------------------------------

#include <map>
#include <string>
#include <cassert>
//------------------------------------------------------------------------------

// Helper macros
#define EnumStringBegin(EnumerationName) template <> \
	struct EnumString<EnumerationName> : \
		public EnumStringBase< EnumString<EnumerationName>, EnumerationName > { \
			static void RegisterEnumerators()
#define EnumStringAdd(EnumeratorName) RegisterEnumerator(EnumeratorName, #EnumeratorName);
#define EnumStringEnd }

namespace Common
{

//! EnumString base class
template <class DerivedType, class EnumType>
class EnumStringBase
{
public:
	// Converts from an enumerator to a string.
	// Returns an empty string if the enumerator was not registered.
	static const std::string &From(const EnumType e);

	// Converts from a string to an enumerator.
	// Returns true if the conversion is successful; false otherwise.
	static const bool To(EnumType &e, const std::string &str);

protected:
	typedef std::map<std::string, EnumType> AssocMap;

	// Constructor / Destructor
	explicit EnumStringBase();
	~EnumStringBase();

	// Use this helper function to register each enumerator
	// and its string representation.
	static void RegisterEnumerator(const EnumType e, const std::string &eStr);

private:
	// Copy Constructor / Assignment Operator
	EnumStringBase(const EnumStringBase &);
	const EnumStringBase &operator=(const EnumStringBase &);

	static AssocMap &GetMap();
};

//! EnumString class: Convert a string to an enumerator and vice versa
/**
	Specialize this class for each enumeration, and implement the RegisterEnumerators() function.
*/
template <class EnumType>
struct EnumString : public EnumStringBase< EnumString<EnumType>, EnumType > {
	static void RegisterEnumerators();
};

// Function definitions

template <class D, class E>
typename EnumStringBase<D, E>::AssocMap &EnumStringBase<D, E>::GetMap()
{
	// A static map of associations from strings to enumerators
	static AssocMap assocMap;
	static bool bFirstAccess = true;

	// If this is the first time we're accessing the map, then populate it.
	if (bFirstAccess) {
		bFirstAccess = false;
		D::RegisterEnumerators();
		assert(!assocMap.empty());
	}

	return assocMap;
}

template <class D, class E>
void EnumStringBase<D, E>::RegisterEnumerator(const E e, const std::string &eStr)
{
	const bool bRegistered = GetMap().insert(typename AssocMap::value_type(eStr, e)).second;
	assert(bRegistered);
	(void)sizeof(bRegistered); // This is to avoid the pesky 'unused variable' warning in Release Builds.
}

template <class D, class E>
const std::string &EnumStringBase<D, E>::From(const E e)
{
	for (;;) { // Code block
		// Search for the enumerator in our map
		typename AssocMap::const_iterator i;
		for (i = GetMap().begin(); i != GetMap().end(); ++i)
			if ((*i).second == e)
				break;

		// If we didn't find it, we can't do this conversion
		if (i == GetMap().end())
			break;

		// Keep searching and see if we find another one with the same value
		typename AssocMap::const_iterator j(i);
		for (++j; j != GetMap().end(); ++j)
			if ((*j).second == e)
				break;

		// If we found another one with the same value, we can't do this conversion
		if (j != GetMap().end())
			break;

		// We found exactly one string which matches the required enumerator
		return (*i).first;
	}

	// We couldn't do this conversion; return an empty string.
	static const std::string dummy;
	return dummy;
}

template <class D, class E>
const bool EnumStringBase<D, E>::To(E &e, const std::string &str)
{
	// Search for the string in our map.
	const typename AssocMap::const_iterator itr(GetMap().find(str));

	// If we have it, then return the associated enumerator.
	if (itr != GetMap().end()) {
		e = (*itr).second;
		return true;
	}

	// We don't have it; the conversion failed.
	return false;
}

} // namespace Common

//------------------------------------------------------------------------------
#endif
