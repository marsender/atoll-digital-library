/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

IndexerConfigNode.hpp

*******************************************************************************/

#ifndef __IndexerConfigNode_HPP
#define __IndexerConfigNode_HPP
//------------------------------------------------------------------------------

#include "IndexerItem.hpp"
//------------------------------------------------------------------------------

namespace Atoll
{
class IndexerConfigParam;

//! Indexing configuration directives for elements and attributes
/**
	Logic:
		- Store the configuration of everything that must be indexed for elements and attributes
		- This object is used to create the vector of indexer items on each xml node parsed
		- The configuration is stored in a multi-map as there may be multiple indexation for a same xml node
		  (as a field, and as a word for example)
		- If the elem property is empty, this is a configuration for attributes only
		- If the elem property is given (not empty), this is a configuration for this element
*/
class IndexerConfigNode
{
private:
	typedef std::multimap <std::string, IndexerConfigParam *> IndexerConfigParamMultiMap;

	IndexerConfigParamMultiMap mConfigMultiMap;

	//! Get the map key from node info
	std::string GetMapKey(const std::string &inElem, const std::string &inAttr) const;
	//! Add indexer items for a node if any. The indexer item vector is created if there is items to add
	void AddIndexerItems(IndexerItemVector *&ioIndexerItemVector, const std::string &inElem, const std::string &inAttr) const;

public:
	std::string mElem;

	IndexerConfigNode(const std::string &inElem);
	~IndexerConfigNode();

	//! Add a configuration directive
	void AddNodeConfigParam(const IndexerConfigParam &inConfigParam);

	//! Create the vector of indexer items for an xml node
	/**
		This method is called for each xml node parsed.
		Returns a vector of indexer items if there is a config for this node, or NULL if there is nothing to index
	 */
	IndexerItemVector *CreateIndexerItemVector(const std::string &inElem, const Common::StringToUnicodeStringMap &inAttrMap) const;
};

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
