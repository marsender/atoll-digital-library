/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Adorner.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "Adorner.hpp"
#include "AdornerConfig.hpp"
#include "../common/UnicodeUtil.hpp"
#include "../common/XercesString.hpp"
#include "../adorner/AdornerDecorator.hpp"
#include "../database/DbDoc.hpp"
#include "../database/DbManager.hpp"
#include "../recordbreaker/RecordBreaker.hpp"
#include "../recordbreaker/RecordBreakerConfig.hpp"
#include "../tokenizer/Token.hpp"
#include "../util/DocRecord.hpp"
#include "../util/Entry.hpp"
#include "unicode/unistr.h"
#include "unicode/ustring.h"
#include <sstream> // For std::ostringstream
//------------------------------------------------------------------------------

#define DEF_Log(x) { gLog.log(eTypLogError, "Err > Adorner: %s", x); }
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

Adorner::Adorner()
{
	mNbError = 0;
	mDbMgr = NULL;
	mDbDoc = NULL;
	mFout = NULL;

	// Unused actually
	mAdornerConfig = new AdornerConfig();

	// Record breaker configuration
	mRecordBreakerConfig = new RecordBreakerConfig();

	mDecoratorDefault = new AdornerDecorator();
	mDecoratorNote = new AdornerDecorator();
	mDecorator = NULL;

	mNumDoc = 0;

	mWantAdornerString = false;
	mWantDbDocStorage = true;
	mRecordBreaker = NULL;

	mXercesNodeVector = new XercesNodeVector;
}
//------------------------------------------------------------------------------

Adorner::~Adorner()
{
	if (mRecordBreaker)
		delete mRecordBreaker;

	delete mRecordBreakerConfig;
	delete mAdornerConfig;

	delete mDecoratorDefault;
	delete mDecoratorNote;

	delete mXercesNodeVector;

	// Check errors if the document has not been closed
  if (mNumDoc != 0)
		gLog.log(eTypLogError, "Err > Adorner: Document number not null (%u)", mNumDoc);
	if (mFout != NULL) {
		u_fclose(mFout);
		DEF_Log("Output file not closed");
	}
	if (mDbDoc != NULL) {
		DEF_Log("Output document database not closed");
	}
	if (!mStrOutput.isEmpty()) {
		//const UChar *sDebug = mStrOutput.getBuffer();
		// The string should have been used and cleared
		DEF_Log("Destructor called with output string not cleared");
	}
}
//------------------------------------------------------------------------------

void Adorner::SetDbManager(DbManager *inDbMgr)
{
	mDbMgr = inDbMgr;
}
//------------------------------------------------------------------------------

void Adorner::SetAdornerDocRef(unsigned int inNumDoc)
{
	mNumDoc = inNumDoc;
}
//------------------------------------------------------------------------------

void Adorner::SetAdornerDstFileName(const std::string &inFileName)
{
	mDstFileName = inFileName;
}
//------------------------------------------------------------------------------

void Adorner::EnableAdornerString(bool inEnable)
{
	mWantAdornerString = inEnable;
}
//------------------------------------------------------------------------------

void Adorner::EnableAdornerDbDocStorage(bool inEnable)
{
	// Document storage ?
	mWantDbDocStorage = inEnable;
	// Increment indexation position ?
	mDecoratorDefault->EnableDecoratorPosition(inEnable);
	mDecoratorNote->EnableDecoratorPosition(inEnable);
}
//------------------------------------------------------------------------------

void Adorner::EnableAdornerRecordBreaker(bool inEnable)
{
	if (inEnable) {
		// Create record breaker with the valid node vector
		if (!mRecordBreaker)
			mRecordBreaker = new RecordBreaker(mRecordBreakerConfig, mXercesNodeVector);
	}
	else {
		if (mRecordBreaker) {
			delete mRecordBreaker;
			mRecordBreaker = NULL;
		}
	}
	// Need adorner string to get the content
	EnableAdornerString(inEnable);
}
//------------------------------------------------------------------------------

void Adorner::LoadAdornerRecordBreakerConfig(const std::string &inFileName)
{
	mRecordBreakerConfig->LoadFromFile(inFileName);
}
//------------------------------------------------------------------------------

void Adorner::SetAdornerIndexer(Indexer *inIndexer)
{
	mDecoratorDefault->SetDecoratorIndexer(inIndexer);
	mDecoratorNote->SetDecoratorIndexer(inIndexer);
}
//------------------------------------------------------------------------------

void Adorner::StartDocument()
{
  AppAssert(mNumDoc != 0);
	AppAssert(mDbDoc == NULL);
  AppAssert(mFout == NULL);

	mNbError = 0;
	mStrOutput.remove();

	// Set the current decorator
	SetCurrentDecorator(mDecoratorDefault);

	// Init decorators: document, page and position
	Entry e(mNumDoc, 0, 0);
	mDecoratorDefault->Clear();
	mDecoratorDefault->SetDecoratorPosition(e);
	e.mUlEntPos = 100000;
	mDecoratorNote->Clear();
	mDecoratorNote->SetDecoratorPosition(e);

	// Init record breaker
	if (mRecordBreaker)
		mRecordBreaker->StartDocument();

	// Open the output file
	if (!mDstFileName.empty()) {
		if ((mFout = u_fopen(mDstFileName.c_str(), "w", NULL, "utf-8")) == NULL) {
			std::ostringstream oss;
			oss << "Open file for writing: " << mDstFileName;
			DEF_Exception(AppException::FileError, oss.str());
		}
	}

	// Init the document database storage
	if (mWantDbDocStorage && mDbMgr && mNumDoc != 0) {
		mDbDoc = mDbMgr->GetDbDoc(mNumDoc);
		mWantAdornerString = true;
	}

	mDecoratorDefault->EnableDecoratorString(mFout || mWantAdornerString);
	mDecoratorNote->EnableDecoratorString(mFout || mWantAdornerString);

	// Output the initial content
	if (mFout || mWantAdornerString) {
		UnicodeString str;
		str = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
		//str += "<AtollDocument>\n";
		AdornerOutput(str);
	}
}
//------------------------------------------------------------------------------

void Adorner::EndDocument(bool inIsException)
{
	// Decorate the last buffered content
	BreakDecorator();

	// Output the final content
	if (mFout || mWantAdornerString) {
		UnicodeString str;
		//str = "\n</AtollDocument>";
		str = "\n";
		AdornerOutput(str);
	}

	// End record breaker
	BreakRecord();
	if (mRecordBreaker) {
		mRecordBreaker->EndDocument();
	}

	// Close the output file
	if (mFout) {
		u_fclose(mFout);
		mFout = NULL;
	}

	// Close the output document database
	if (mDbDoc) {
		mDbMgr->CloseDbDoc(mNumDoc);
		mDbDoc = NULL;
	}

	if (!mXercesNodeVector->empty()) {
		if (!inIsException) {
			mNbError++;
			DEF_Log("EndDocument elem list not empty");
		}
		mXercesNodeVector->clear();
	}

	// Clear the document number
	mNumDoc = 0;
	mDstFileName = "";
	mDecoratorDefault->Clear();
	mDecoratorNote->Clear();
}
//------------------------------------------------------------------------------

void Adorner::StartNode(const std::string &inElem, const StringToUnicodeStringMap &inAttrMap)
{
	bool wantTrace = false;

	eTypToken typToken = Token::StaticGetTypToken(inElem);
	switch (typToken) {
	case eTypTokenNote:
		SetCurrentDecorator(mDecoratorNote);
		break;
	case eTypTokenNli:
		// Decorate the last buffered content
		BreakDecorator();
		break;
	default:
		break;
	}

	if (wantTrace)
		Trace("Adorner::StartNode: %s", inElem.c_str());

	// Use record breaker (before adding the node)
	if (mRecordBreaker) {
		eTypBreaker typBreaker = mRecordBreaker->ComputeNodeTypBreaker(inElem, inAttrMap, true);
		if (typBreaker == eTypBreakerOpen)
			BreakRecord();
	}

	XercesNode *xercesNode = new XercesNode(inElem, inAttrMap, typToken);
	UnicodeString str;
	xercesNode->ToString(str, true);
	mDecorator->AddNode(str);

	mXercesNodeVector->push_back(xercesNode);
}
//------------------------------------------------------------------------------

void Adorner::EndNode()
{
	XercesNode *xercesNode = mXercesNodeVector->back();
	mXercesNodeVector->pop_back();

	UnicodeString str;
	xercesNode->ToString(str, false);
	mDecorator->AddNode(str);

	switch (xercesNode->GetTypToken()) {
	case eTypTokenNote:
		SetCurrentDecorator(mDecoratorDefault);
		break;
	case eTypTokenNli:
		// Decorate the last buffered content
		BreakDecorator();
		break;
	default:
		break;
	}

	// Use record breaker
	if (mRecordBreaker) {
		const std::string &elem = xercesNode->GetElem();
		StringToUnicodeStringMap *attrMap = NULL;
		eTypBreaker typBreaker = mRecordBreaker->ComputeNodeTypBreaker(elem, *attrMap, false);
		if (typBreaker == eTypBreakerClose)
			BreakRecord();
	}

	delete xercesNode;
}
//------------------------------------------------------------------------------

void Adorner::AddCharacters(const UChar *inStr, unsigned int inLength)
{
	mDecorator->AddContent(inStr, inLength);

	// Use record breaker
	if (mRecordBreaker) {
		if (mRecordBreaker->mExtraContentLevel)
			mRecordBreaker->AddExtraContent(inStr, inLength);
	}

	bool wantTrace = false;
	if (wantTrace) {
		UTrace("Adorner::AddCharacters", inStr, inLength);
	}

	// Rarely used
	if (mDecorator->NeedDecorate()) {
		// Check if the characters are a line break
		// in order to decorate the current string buffer
		UChar *pos = u_strchr(inStr, 0x000A);
		if (pos) {
			// Decorate if there is other chars than space, tab or crlf
			if (!UnicodeStringIsOnlySpace(inStr))
				Decorate();
		}
	}
}
//------------------------------------------------------------------------------

void Adorner::ProcessingInstruction(const UChar *inPiStr)
{
	// Pi "linebreak"
	const UChar cLineBreak[] = { 'l', 'i', 'n', 'e', 'b', 'r', 'e', 'a', 'k', 0 };

	if (u_strcmp(inPiStr, cLineBreak) == 0) {
		mDecorator->AddProcessingInstruction(cLineBreak);
		return;
	}

	// Check record breaker pi
	if (mRecordBreaker) {
		std::string pi;
		UChar2String(pi, inPiStr);
		eTypBreaker typBreaker = mRecordBreaker->ComputeProcessingInstruction(inPiStr);
		if (typBreaker == eTypBreakerOpen)
			BreakRecord();
	}
}
//------------------------------------------------------------------------------

void Adorner::SetCurrentDecorator(AdornerDecorator *inDecorator)
{
	// Warning: initial decorator is empty
	if (mDecorator) {
		BreakDecorator();
	}

	mDecorator = inDecorator;
}
//------------------------------------------------------------------------------

void Adorner::BreakDecorator()
{
	Decorate();
	mDecorator->IndexWord();
}
//------------------------------------------------------------------------------

void Adorner::Decorate()
{
	// Decorate the current string buffer
	mDecorator->DecorateContent();

	// Get, print and clear the decorator content
	if (mFout || mWantAdornerString) {
		UnicodeString &str = mDecorator->GetDecoratorString();
		AdornerOutput(str);
		str.remove();
	}

	// If the max cache size is reached => store the word set in the database
	/*
	if (DEF_WordSetCacheSize != DEF_LongMax
		&& mDecorator->GetWordSetSize() > DEF_WordSetCacheSize) {
		StoreDecoratorToDb(mDecorator);
	}
	*/
}
//------------------------------------------------------------------------------

void Adorner::BreakRecord()
{
	Entry e;

	if (!mRecordBreaker) {
		if (mDbDoc) {
			DocRecord dr;
			UnicodeString key;
			mDecorator->GetDecoratorPosition(e);
			dr.mPge = e.mUlEntPge;
			dr.mPos = e.mUlEntPos;
			dr.mTexte = mStrOutput;
			dr.GetPageKey(key);
			mDbDoc->AddDocRecord(key, dr);
			mStrOutput.remove();
		}
		return;
	}

	// Get the record center
	BreakDecorator();

	// Get the record envelope
	UnicodeString recordHeader, recordFooter;
	mRecordBreaker->GetXmlEnvelope(recordHeader, recordFooter);

	bool wantTrace = false;
	if (wantTrace) {
		UTrace("Record header", recordHeader.getTerminatedBuffer());
		UTrace("Record content", mStrOutput.getTerminatedBuffer());
		UTrace("Record footer", recordFooter.getTerminatedBuffer());
	}

	if (mDbDoc) {
		DocRecord dr;
		UnicodeString key;
		mDecorator->GetDecoratorPosition(e);
		dr.mPge = e.mUlEntPge;
		dr.mPos = e.mUlEntPos;
		dr.mTexte = recordHeader;
		dr.mTexte += mStrOutput;
		dr.mTexte += recordFooter;
		dr.GetPageKey(key);
		mDbDoc->AddDocRecord(key, dr);
	}

	// Increment decorator pages
	mDecoratorDefault->GetDecoratorPosition(e);
	e.mUlEntPge++;
	mDecoratorDefault->SetDecoratorPosition(e);
	mDecoratorNote->GetDecoratorPosition(e);
	e.mUlEntPge++;
	mDecoratorNote->SetDecoratorPosition(e);

	mStrOutput.remove();
}
//------------------------------------------------------------------------------

void Adorner::AdornerOutput(const UnicodeString &inStr)
{
	if (mFout) {
		u_file_write(inStr.getBuffer(), inStr.length(), mFout);
	}

	if (mWantAdornerString) {
		mStrOutput += inStr;
	}
}
//------------------------------------------------------------------------------
