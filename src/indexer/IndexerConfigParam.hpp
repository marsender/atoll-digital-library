/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

IndexerConfigParam.hpp

*******************************************************************************/

#ifndef __IndexerConfigParam_HPP
#define __IndexerConfigParam_HPP
//------------------------------------------------------------------------------

#include <string>
//------------------------------------------------------------------------------

namespace Atoll
{

enum eTypIndex {
	eTypIndexNone,
	eTypIndexWord,             //!< Index each word
	eTypIndexField,            //!< Index content as a field (multi-words)
	eTypIndexInterv,           //!< Index as an interval
	eTypIndexNothing,          //!< No index at all
	eTypIndexEnd
};

//! Indexer configuration parameter for an xml node
/**
	Logic:
		- Indicate how an elem, an attribute (or both) must be indexed
*/
class DEF_Export IndexerConfigParam
{
public:
	std::string mName;         //!< Index name (unique id)
	std::string mElem;         //!< Element to index
	std::string mAttr;         //!< Attribute to index
	eTypIndex mTypIndex;       //!< Index type
	bool mIsLowerCase;         //!< Lowercase index ?

	IndexerConfigParam();
	IndexerConfigParam(const IndexerConfigParam &inIndexerConfigParam);
	IndexerConfigParam(const std::string &inName, const std::string &inElem, const std::string &inAttr, eTypIndex inTypIndex, bool isLowerCase);

	void SetConfigParam(const char *inName, const char *inElem, const char *inAttr, eTypIndex inTypIndex, bool isLowerCase);
	void SetConfigParam(const std::string &inName, const std::string &inElem, const std::string &inAttr, eTypIndex inTypIndex, bool isLowerCase);

  void Clear(); //!< Clear the content
	bool IsValid() const; //!< Check validity

	//! Get the database index name for this index
	const std::string &GetIndexDbName() const;

	std::string ToString() const;
};
//------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream& inStream, const IndexerConfigParam &inConfig);

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
