/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Adorner.hpp

*******************************************************************************/

#ifndef __Adorner_HPP
#define __Adorner_HPP
//------------------------------------------------------------------------------

#include "../common/XercesNode.hpp"
#include "unicode/ustdio.h"
//------------------------------------------------------------------------------

namespace Atoll
{
class DbDoc;
class Indexer;
class DbManager;
class AdornerConfig;
class AdornerDecorator;
class RecordBreaker;
class RecordBreakerConfig;

//! Xml content adorner
/**
	Logic:
		- Recieve notifications from the parser adorner handler
		- Send the parser notifications to the adorner decorator
		- The adorner decorator analyse and decorate the content (separate words and set their positions)
		- The decorated content may be indexed via the decorator if the indexer is set
		- The decorated content may be stored in an output string or in an output file
		- Alternatively, the decorated content can also be sent to database via a record breaker
*/
class Adorner
{
private:
	//! Adorner configuration
	AdornerConfig *mAdornerConfig;
	//! Record breaker configuration
	RecordBreakerConfig *mRecordBreakerConfig;

	DbManager *mDbMgr;
	unsigned long mNbError;	//!< Number of errors in the document
	AdornerDecorator *mDecorator;
	AdornerDecorator *mDecoratorDefault;
	AdornerDecorator *mDecoratorNote;
	unsigned int mNumDoc;
	bool mWantAdornerString; //!< Enable the document storage into string
	bool mWantDbDocStorage; //!< Enable document storage into database

	RecordBreaker *mRecordBreaker;

	DbDoc *mDbDoc; //!< Output document database
	UFILE *mFout; //!< Output file
	std::string mDstFileName; //!< Output file name
	UnicodeString mStrOutput; //!< Output string

	Common::XercesNodeVector *mXercesNodeVector;

	//! Set the current decorator
	void SetCurrentDecorator(AdornerDecorator *inDecorator);

	//! Decorate the current buffered content and add it to the output
	void Decorate();

	//! Decorate and purge decorator
	void BreakDecorator();

	//! Break and store a record
	void BreakRecord();

	//! Add content to the output
	void AdornerOutput(const UnicodeString &inStr);

public:
	Adorner();
	~Adorner();

	//! Init the db manager. Must be set before parsing
	void SetDbManager(DbManager *inDbMgr);

	void StartNode(const std::string &inElem, const Common::StringToUnicodeStringMap &inAttrMap);
	void EndNode();

	//! Receive notification of character data inside an element
	void AddCharacters(const UChar *inStr, unsigned int inLength);

	//! Receive notification of a processing instruction
	void ProcessingInstruction(const UChar *inPiStr);

	//! Receive notification of the beginning of the document
	void StartDocument();

	//! Receive notification of the end of the document
	void EndDocument(bool inIsException);

	//! Get the number of error in the current document
	unsigned long GetNbError() const { return mNbError; }

	//! Get the document number
	unsigned int GetNumDoc() const { return mNumDoc; }
	//! Init the document number. Must be set before parsing
	void SetAdornerDocRef(unsigned int inNumDoc);
	//! Init the destination file name
	void SetAdornerDstFileName(const std::string &inFileName);
	//! Tell if the adorner must output it's content in a string
	void EnableAdornerString(bool inEnable);
	//! Tell if the adorner must store the document into database
	void EnableAdornerDbDocStorage(bool inEnable);
	//! Tell if the adorner must output it's content in a record breaker
	void EnableAdornerRecordBreaker(bool inEnable);
	//! Load the record breaker configuration
	void LoadAdornerRecordBreakerConfig(const std::string &inFileName);
	//! Set the indexer for the adorner
	void SetAdornerIndexer(Indexer *inIndexer);

	//! Get the adorner output string
	UnicodeString &GetAdornerString() { return mStrOutput; }

	//! Force the adorner to decorate the actual content
	void ForceDecorate() { Decorate(); }
};

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
