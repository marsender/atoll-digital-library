/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

RecordBreaker.hpp

*******************************************************************************/

#ifndef __RecordBreaker_HPP
#define __RecordBreaker_HPP
//------------------------------------------------------------------------------

#include "RecordBreakerConfigParam.hpp"
#include "../common/Containers.hpp"
#include "../common/XercesNode.hpp"
//------------------------------------------------------------------------------

namespace Atoll
{
class RecordBreakerConfig;

//! Xml content record breaker
/**
	Logic:
		- Recieve notifications from the parser adorner handler
		- Check each node for a record break
		- Produce the envelope (header an footer) for each record
*/
class RecordBreaker
{
private:
	UnicodeString mHeader;
	UnicodeString mExtraContent;
	//! Record breaker configuration
	RecordBreakerConfig *mRecordBreakerConfig;
	Common::XercesNodeVector *mXercesNodeVector;
	Common::IntToUnicodeStringMap mExtraContentMap;

public:
	size_t mExtraContentLevel;
  unsigned long mPge;

	RecordBreaker(RecordBreakerConfig *inRecordBreakerConfig, Common::XercesNodeVector *inXercesNodeVector);
	~RecordBreaker();

	//! Receive notification of the beginning of the document
	void StartDocument();

	//! Receive notification of the end of the document
	void EndDocument();

	//void StartNode(const Common::XercesNode *inXercesNode);
	//void EndNode(const Common::XercesNode *inXercesNode);

	//! Receive notification of character data inside an element
	//void AddCharacters(const UChar *inStr, unsigned int inLength);

	//! Compute node type from xerces node
	eTypBreaker ComputeNodeTypBreaker(const std::string &inElem, const Common::StringToUnicodeStringMap &inAttrMap, bool inIsOpen);
	//! Compute node type from processing instruction
	eTypBreaker ComputeProcessingInstruction(const UChar *inPiStr);

	//! Get xml header and footer strings
	void GetXmlEnvelope(UnicodeString &outHeader, UnicodeString &outFooter);

	//! Add content to the extra content string
	void AddExtraContent(const UChar *inStr, unsigned int inLength);

	std::string ToString() const;
};
//------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream& inStream, const RecordBreaker &inRecordBreaker);
//------------------------------------------------------------------------------

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
