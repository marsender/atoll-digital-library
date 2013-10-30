/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

RecordBreakerConfig.hpp

*******************************************************************************/

#ifndef __RecordBreakerConfig_HPP
#define __RecordBreakerConfig_HPP
//------------------------------------------------------------------------------

#include "RecordBreakerConfigParam.hpp"
#include "../common/XercesNode.hpp"
#include <map>
//------------------------------------------------------------------------------

namespace Atoll
{
//! Record breaker configuration
/**
	Logic:
		- Store the configuration of record breakers
		- Each node parsed in an xml source file calls this object to know it's record breaker properties
*/
class RecordBreakerConfig
{
private:
	typedef std::map <std::string, RecordBreakerConfigParam *> RecordBreakerConfigParamMap;

	bool mIsConfigParamAttr;	//!< Presence of a config param with an attribute
	bool mIsConfigParamValue;	//!< Presence of a config param with a value
	unsigned long mNbError;	//!< Number of errors in the document
	Common::StringToIntMap mTokenMap; //!< Map of valid xml tokens
	Common::XercesNodeVector *mXercesNodeVector; //!< Vector of xml nodes
	bool mIsActionAddConfigParam; //!< Action type indicator

	//! Map of specific nodes configurations
	RecordBreakerConfigParamMap mConfigParamMap;

	//! Get the map key from node info
	std::string GetMapKey(const std::string &inElem) const;
	//! Get the map key from node info
	std::string GetMapKey(const std::string &inElem, const std::string &inAttr, const std::string &inValue) const;

	//! Add a specific node configuration
	void AddConfigParam(RecordBreakerConfigParam &inConfigParam);
	//! Clear the map of nodes configurations
	void ClearConfigParamMap();

public:
	RecordBreakerConfig();
	~RecordBreakerConfig();

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

	//! Get the number of configurated items
	unsigned int GetRecordBreakerConfigSize();

	//! Get the breaker type of a node
	/**
		This method is called for each xml node parsed
	 */
	eTypBreaker GetNodeTypBreaker(const std::string &inElem, const Common::StringToUnicodeStringMap &inAttrMap) const;
};

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
