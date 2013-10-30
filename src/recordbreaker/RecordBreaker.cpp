/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

RecordBreaker.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "RecordBreaker.hpp"
#include "RecordBreakerConfig.hpp"
#include "../common/UnicodeUtil.hpp"
#include "unicode/ustring.h"
//------------------------------------------------------------------------------

#define DEF_Log(x) { gLog.log(eTypLogError, "Err > RecordBreaker: %s", x); }
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

std::ostream& Atoll::operator<<(std::ostream& inStream, const RecordBreaker &inRecordBreaker)
{
	inStream << inRecordBreaker.ToString();
	return inStream;
}
//------------------------------------------------------------------------------

RecordBreaker::RecordBreaker(RecordBreakerConfig *inRecordBreakerConfig,
														 Common::XercesNodeVector *inXercesNodeVector)
{
	AppAssert(inRecordBreakerConfig != NULL);
	AppAssert(inXercesNodeVector != NULL);

	mPge = 0;
	mExtraContentLevel = 0;
	mRecordBreakerConfig = inRecordBreakerConfig;
	mXercesNodeVector = inXercesNodeVector;
}
//------------------------------------------------------------------------------

RecordBreaker::~RecordBreaker()
{
}
//------------------------------------------------------------------------------

void RecordBreaker::StartDocument()
{
  AppAssert(mXercesNodeVector != NULL);

	if (mRecordBreakerConfig->GetRecordBreakerConfigSize() == 0) {
		DEF_Log("StartDocument with empty config");
	}

	mPge = 0;
	mExtraContentLevel = 0;
	mExtraContent.remove();
}
//------------------------------------------------------------------------------

void RecordBreaker::EndDocument()
{
	if (mExtraContentLevel) {
		DEF_Log("EndDocument extra content level is not null");
	}
}
//------------------------------------------------------------------------------

eTypBreaker RecordBreaker::ComputeNodeTypBreaker(const std::string &inElem,
																								 const StringToUnicodeStringMap &inAttrMap, bool inIsOpen)
{
	bool wantTrace = false;

	eTypBreaker typBreakerConfig = mRecordBreakerConfig->GetNodeTypBreaker(inElem, inAttrMap);
	if (typBreakerConfig == eTypBreakerNone)
		return eTypBreakerNone;

	eTypBreaker typBreaker = eTypBreakerNone;
	const std::string &elem = inElem;
	if (inIsOpen) {
		if (typBreakerConfig == eTypBreakerOpen) {
			if (mPge != 0) {
				typBreaker = eTypBreakerOpen;
			}
			mPge++;
		}
	}
	else {
		if (typBreakerConfig == eTypBreakerClose) {
			typBreaker = eTypBreakerClose;
			mPge++;
		}
	}

	if (typBreakerConfig == eTypBreakerStore) {
		bool addNodeContent = false;
		size_t contentLevel = mXercesNodeVector->size();
		if (inIsOpen) {
			if (mExtraContentLevel == 0) {
				mExtraContentLevel = contentLevel;
				addNodeContent = true;
			}
		}
		else {
			if (contentLevel == mExtraContentLevel) {
				mExtraContentLevel = 0;
				addNodeContent = true;
			}
		}
		if (addNodeContent) {
			UnicodeString str;
			{
				XercesNode *xercesNode = new XercesNode(inElem, inAttrMap, -1);
				xercesNode->ToString(str, inIsOpen);
				delete xercesNode;
			}
			AddExtraContent(str.getTerminatedBuffer(), str.length());
			// If node close: store the extra content of the current level
			if (!inIsOpen) {
				// Remove higher levels
				IntToUnicodeStringMap::const_iterator it = mExtraContentMap.begin();
				IntToUnicodeStringMap::const_iterator itEnd = mExtraContentMap.end();
				for (; it != itEnd; ++it) {
					size_t level = it->first;
					if (level >= contentLevel) {
						mExtraContentMap.erase(level);
						it = mExtraContentMap.begin();
						itEnd = mExtraContentMap.end();
					}
				}
				mExtraContentMap[contentLevel] = mExtraContent;
				mExtraContent.remove();
				/*
				IntToUnicodeStringMap::const_iterator itMap = mExtraContentMap.find(contentLevel);
				if (itMap == mExtraContentMap.end()) {
					mExtraContentMap[contentLevel] = mExtraContent;
					mExtraContent.remove();
				}
				*/
			}
		}
	}

	if (wantTrace) {
		if (typBreaker != eTypBreakerNone)
			Trace("RecordBreaker::ComputeNodeTypBreaker: %s %s", elem.c_str(), inIsOpen ? "open" : "close");
	}

	return typBreaker;
}
//------------------------------------------------------------------------------

eTypBreaker RecordBreaker::ComputeProcessingInstruction(const UChar *inPiStr)
{
	// Pi "pagebreak"
	const UChar cPageBreak[] = { 'p', 'a', 'g', 'e', 'b', 'r', 'e', 'a', 'k', 0 };
	eTypBreaker typBreaker = eTypBreakerNone;

	if (u_strcmp(inPiStr, cPageBreak) == 0) {
		/*if (mPge != 1) {
			typBreaker = eTypBreakerOpen;
		}*/
		typBreaker = eTypBreakerOpen;
		mPge++;
	}

	return typBreaker;
}
//------------------------------------------------------------------------------

void RecordBreaker::GetXmlEnvelope(UnicodeString &outHeader, UnicodeString &outFooter)
{
	// Get the header
	outHeader = mHeader;

	// Compute the next header
	mHeader.remove();
	mHeader += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	int level = 0;
	UnicodeString str;
	XercesNodeVector::const_iterator it, itEnd;
	it = mXercesNodeVector->begin();
	itEnd = mXercesNodeVector->end();
	IntToUnicodeStringMap::const_iterator itMap, itMapEnd;
	itMapEnd = mExtraContentMap.end();
	for (; it != itEnd; ++it) {
		XercesNode *node = *it;
		node->ToString(str, true);
		mHeader += str;
		itMap = mExtraContentMap.find(++level);
		if (itMap != itMapEnd) {
			mHeader += itMap->second;
		}
	}

	// Get the footer
	outFooter.remove();
	XercesNodeVector::const_reverse_iterator rit, ritEnd;
	rit = mXercesNodeVector->rbegin();
	ritEnd = mXercesNodeVector->rend();
	for (; rit != ritEnd; ++rit) {
		XercesNode *node = *rit;
		node->ToString(str, false);
		outFooter += str;
	}
}
//------------------------------------------------------------------------------

void RecordBreaker::AddExtraContent(const UChar *inStr, unsigned int inLength)
{
	mExtraContent.append(inStr, inLength);
}
//------------------------------------------------------------------------------

/*
void RecordBreaker::StartNode(const Common::XercesNode *inXercesNode)
{
	bool wantTrace = false;

	const std::string &elem = inXercesNode->GetElem();
	mTypBreaker = GetTypBreaker(elem);
	switch (mTypBreaker) {
	case eTypBreakerOpen:
		break;
	case eTypBreakerClose:
		break;
	default:
		break;
	}

	if (wantTrace)
		Trace("RecordBreaker::StartNode: %s", elem.c_str());

	//XercesNode *xercesNode = new XercesNode(inElem, inAttrMap, typBreaker);
	//mXercesNodeVector->push_back(xercesNode);

	//UnicodeString str = xercesNode->ToString(true);
	//mDecorator->AddNode(str);
}
//------------------------------------------------------------------------------

void RecordBreaker::EndNode(const Common::XercesNode *inXercesNode)
{
	//XercesNode *xercesNode = mXercesNodeVector->back();
	//mXercesNodeVector->pop_back();

	UnicodeString str = inXercesNode->ToString(false);
	//mDecorator->AddNode(str);

	const std::string &elem = inXercesNode->GetElem();
	mTypBreaker = GetTypBreaker(elem);
	switch (mTypBreaker) {
	case eTypBreakerOpen:
		break;
	case eTypBreakerClose:
		break;
	default:
		break;
	}
}
//------------------------------------------------------------------------------

void RecordBreaker::AddCharacters(const UChar *inStr, unsigned int inLength)
{
	//mDecorator->AddContent(inStr, inLength);

	bool wantTrace = false;
	if (wantTrace) {
		UTrace("RecordBreaker::AddCharacters", inStr, inLength);
	}
}
//------------------------------------------------------------------------------
*/

/*
eTypBreaker RecordBreaker::GetTypBreaker(const std::string &inStr)
{
	if (inStr == "pg") {
		return eTypBreakerOpen;
	}
	else if (inStr == "head") {
		return eTypBreakerClose;
	}

	return eTypBreakerNone;
}*/
//------------------------------------------------------------------------------

std::string RecordBreaker::ToString() const
{
	std::string s("RecordBreaker: ");

	//s += UnicodeString2String(mTexte);
	s += " [";
	//s += mEntry.ToString(); // Todo
	s += "]";

	return s;
}
//------------------------------------------------------------------------------
