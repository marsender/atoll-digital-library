/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

RecordBreakerConfigParam.hpp

*******************************************************************************/

#ifndef __RecordBreakerConfigParam_HPP
#define __RecordBreakerConfigParam_HPP
//------------------------------------------------------------------------------

#include <string>
//------------------------------------------------------------------------------

namespace Atoll
{

enum eTypBreaker {
	eTypBreakerNone,
	eTypBreakerOpen,          //!< Break on xml node open
	eTypBreakerClose,         //!< Break after xml node close
	eTypBreakerStore,         //!< Store the xml content
	eTypBreakerEnd
};

//! Record breaker configuration parameter for an xml node
/**
	Logic:
		- Indicate how an elem, an attribute (or both) must break the document into records
*/
class DEF_Export RecordBreakerConfigParam
{
private:
	std::string GetRecordBreakerName() const;
	std::string GetRecordBreakerName(const std::string &inElem) const;

public:
	std::string mElem;
	std::string mAttr;
	std::string mValue;
	eTypBreaker mTypBreaker;

	RecordBreakerConfigParam();
	RecordBreakerConfigParam(const RecordBreakerConfigParam &inRecordBreakerConfigParam);
	RecordBreakerConfigParam(const std::string &inElem, const std::string &inAttr, const std::string &inValue, eTypBreaker inTypBreaker);

	void SetConfigParam(const char *inElem, const char *inAttr, const char *inValue, eTypBreaker inTypBreaker);
	void SetConfigParam(const std::string &inElem, const std::string &inAttr, const std::string &inValue, eTypBreaker inTypBreaker);
	std::string ToString() const;
};
//------------------------------------------------------------------------------

std::ostream &operator<<(std::ostream& inStream, const RecordBreakerConfigParam &inConfig);

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
