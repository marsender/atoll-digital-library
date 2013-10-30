/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

RecordBreakerConfigParam.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "RecordBreakerConfigParam.hpp"
//#include "../common/UnicodeUtil.hpp"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

//#define DEF_Log(x) { gLog.log(eTypLogError, "Err > RecordBreakerConfigParam: %s", x); }
//------------------------------------------------------------------------------

std::ostream& Atoll::operator<<(std::ostream& inStream, const RecordBreakerConfigParam &inConfig)
{
	inStream << inConfig.ToString();
	return inStream;
}
//------------------------------------------------------------------------------

RecordBreakerConfigParam::RecordBreakerConfigParam()
{
	mTypBreaker = eTypBreakerNone;
}
//------------------------------------------------------------------------------

RecordBreakerConfigParam::RecordBreakerConfigParam(const RecordBreakerConfigParam &inRecordBreakerConfigParam)
{
	mElem = inRecordBreakerConfigParam.mElem;
	mTypBreaker = inRecordBreakerConfigParam.mTypBreaker;
}
//------------------------------------------------------------------------------

RecordBreakerConfigParam::RecordBreakerConfigParam(const std::string &inElem, const std::string &inAttr,
																									 const std::string &inValue, eTypBreaker inTypBreaker)
{
	mElem = inElem;
	mAttr = inAttr;
	mValue = inValue;
	mTypBreaker = inTypBreaker;
}
//------------------------------------------------------------------------------

void RecordBreakerConfigParam::SetConfigParam(const char *inElem, const char *inAttr,
																							const char *inValue, eTypBreaker inTypBreaker)
{
	mElem = inElem;
	mAttr = inAttr;
	mValue = inValue;
	mTypBreaker = inTypBreaker;
}
//------------------------------------------------------------------------------

void RecordBreakerConfigParam::SetConfigParam(const std::string &inElem, const std::string &inAttr,
																							const std::string &inValue, eTypBreaker inTypBreaker)
{
	mElem = inElem;
	mAttr = inAttr;
	mValue = inValue;
	mTypBreaker = inTypBreaker;
}
//------------------------------------------------------------------------------

std::string RecordBreakerConfigParam::GetRecordBreakerName() const
{
	return GetRecordBreakerName(mElem);
}
//------------------------------------------------------------------------------

std::string RecordBreakerConfigParam::GetRecordBreakerName(const std::string &inElem) const
{
	std::string str;
	char buf[50];

	AppAssert(inElem.length() != 0);

	// Record breaker name
	str += inElem;

	// Record breaker options
	sprintf(buf, " %d", mTypBreaker);
	str += buf;

	return str;
}
//------------------------------------------------------------------------------

std::string RecordBreakerConfigParam::ToString() const
{
	std::string s("RecordBreakerConfigParam: ");

	s += GetRecordBreakerName();

	return s;
}
//------------------------------------------------------------------------------
