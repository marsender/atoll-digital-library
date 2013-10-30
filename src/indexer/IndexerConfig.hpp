/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

IndexerConfig.hpp

*******************************************************************************/

#ifndef __IndexerConfig_HPP
#define __IndexerConfig_HPP
//------------------------------------------------------------------------------

#include "IndexerConfigParam.hpp"
#include "IndexerItem.hpp"
#include "../common/XercesNode.hpp"
#include <map>
//------------------------------------------------------------------------------

namespace Atoll
{
class IndexerConfigNode;

//! Indexer configuration
/**
	Logic:
		- Store the configuration of everything that must be indexed
		- Each node parsed in an xml source file calls this object to know what it has to index
*/
class IndexerConfig
{
private:
	typedef std::map <std::string, IndexerConfigParam * > IndexerConfigParamMap;
	typedef std::map <std::string, IndexerConfigNode *> IndexerConfigNodeMap;
	typedef std::map <std::string, IndexerItemVector *> IndexerItemVectorMap;

	unsigned long mNbError;	//!< Number of errors in the document
	Common::StringToIntMap mTokenMap; //!< Map of valid xml tokens
	Common::XercesNodeVector *mXercesNodeVector; //!< Vector of xml nodes
	bool mIsActionAddConfigParam; //!< Action type indicator

	//! Map of configuration parameters
	IndexerConfigParamMap mConfigParamMap;
	//! Configuration for nodes with attributes only (empty elem)
	IndexerConfigNode *mAttrConfigNode;
	//! Map of specific nodes configurations
	IndexerConfigNodeMap mConfigNodeMap;

	//! Optional cache
	IndexerItemVectorMap *mIndexerItemVectorMapCache;

	// Tell if the current stack of nodes contains a token
	//bool IsParentToken(int inTypToken);

	//! Clear the map of configuration parmeters
	void ClearConfigParamMap();
	//! Add a specific node configuration
	void AddConfigParam(IndexerConfigParam &inConfigParam);
	//! Clear the map of nodes configurations
	void ClearConfigNodeMap();

public:
	IndexerConfig();
	~IndexerConfig();

	void LoadFromFile(const std::string &inFileName);

	void StartNode(const std::string &inElem, const Common::StringToUnicodeStringMap &inAttrMap);
	void EndNode();

	//! Receive notification of character data inside an element
	void AddCharacters(const UChar *inStr, unsigned int inLength) const;

	//! Receive notification of the beginning of the document
	void StartDocument();

	//! Receive notification of the end of the document
	void EndDocument(bool inIsException);

	//! Get the number of error in the current document
	unsigned long GetNbError() const { return mNbError; }

	//! Get an indexer config parameter from it's name
	IndexerConfigParam *GetConfigParam(const std::string &inIndexName);

	//! Get the vector of nodes to index, or NULL if there is nothing to index
	/**
		This method is called for each xml node parsed
	 */
	IndexerItemVector *GetNodeIndexerVector(const std::string &inElem, const Common::StringToUnicodeStringMap &inAttrMap) const;
};

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
