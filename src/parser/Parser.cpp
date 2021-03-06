/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Parser.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "Parser.hpp"
#include "AdornerHandler.hpp"
#include "IndexerHandler.hpp"
#include "IndexerConfigHandler.hpp"
#include "RecordBreakerConfigHandler.hpp"
#include "../common/UnicodeUtil.hpp"
#include "../common/XercesString.hpp"
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
XERCES_CPP_NAMESPACE_USE

Parser::Parser(SAX2XMLReader::ValSchemes inValScheme, const std::string &inCatalogFile,
							 eTypHandler inTypHandler)
	:XercesParser(inValScheme, inCatalogFile),
	mTypHandler(inTypHandler)
{
	// Create and initialize the SAX handler
	switch (mTypHandler) {
	case eTypHandlerBase:
		mHandler = new XercesBaseHandler();
		break;
	case eTypHandlerIndexer:
		mHandler = new IndexerHandler();
		break;
	case eTypHandlerAdorner:
		mHandler = new AdornerHandler();
		break;
	case eTypHandlerIndexerConfig:
		mHandler = new IndexerConfigHandler();
		break;
	case eTypHandlerRecordBreakerConfig:
		mHandler = new RecordBreakerConfigHandler();
		break;
	default:
		DEF_Exception(AppException::ParserError, "Incorrect sax handler type");
	}

	XercesParser::InitXercesHandler();
}
//------------------------------------------------------------------------------

Parser::~Parser()
{
	// The handler is deleted by the parent class
}
//------------------------------------------------------------------------------

bool Parser::XercesInitHandler()
{
	bool isOk = true;

	switch (mTypHandler) {
	case eTypHandlerBase:
		break;
	case eTypHandlerIndexer:
		{
			IndexerHandler *handler = static_cast<IndexerHandler *>(mHandler);
			isOk = handler->InitHandler();
		}
		break;
	case eTypHandlerAdorner:
		{
			AdornerHandler *handler = static_cast<AdornerHandler *>(mHandler);
			isOk = handler->InitHandler();
		}
		break;
	case eTypHandlerIndexerConfig:
		{
			IndexerConfigHandler *handler = static_cast<IndexerConfigHandler *>(mHandler);
			isOk = handler->InitHandler();
		}
		break;
	case eTypHandlerRecordBreakerConfig:
		{
			RecordBreakerConfigHandler *handler = static_cast<RecordBreakerConfigHandler *>(mHandler);
			isOk = handler->InitHandler();
		}
		break;
	default:
		DEF_Exception(AppException::ParserError, "Incorrect sax handler type");
	}

	return isOk;
}
//------------------------------------------------------------------------------

bool Parser::XercesParse(const std::string &inFileName)
{
	bool isOk = true;
	unsigned long duration = 0;

	gLog.log(eTypLogDebug, "Deb > Parse file: %s", inFileName.c_str());

	// Initialize the handler
	if (!XercesInitHandler())
		return false;

	try {
		mHandler->BeginDocument();
		const unsigned long startMillis = XMLPlatformUtils::getCurrentMillis();
		mParser->parse(inFileName.c_str());
		const unsigned long endMillis = XMLPlatformUtils::getCurrentMillis();
		duration = endMillis - startMillis;
	}
	catch (const AppException &e) {
		gLog.log(eTypLogError, "Err > Application exception during parse file - %s", e.what());
		isOk = false;
	}
	catch (const OutOfMemoryException &) {
		gLog.log(eTypLogError, "Err > Xerces out of memory exception during parse file");
		isOk = false;
	}
	catch (const XMLException &e) {
		XercesString message(e.getMessage());
		gLog.log(eTypLogError, "Err > Xerces exception during parse file: %s", message.localForm());
		isOk = false;
	}
	catch (std::exception &e) {
		gLog.log(eTypLogError, "Err > Unknown exception during parse file - %s", e.what());
		isOk = false;
	}

	// Close the document
	mHandler->EndDocument(!isOk);

	if (isOk) {
		// Check handler errors
		unsigned long nbErr = mHandler->GetNbError();
		if (nbErr) {
			gLog.log(eTypLogError, "Err > Parse file handler got %lu error(s)", nbErr);
			isOk = false;
		}
		else {
			gLog.log(eTypLogDebug, "Deb > Parse file: %s in %lu ms", inFileName.c_str(), duration);
		}
	}
	else
		gLog.log(eTypLogError, "Err > Parse file error: %s", inFileName.c_str());

	return isOk;
}
//------------------------------------------------------------------------------

bool Parser::XercesParse(const UChar *inStr, int32_t inLength /*= -1*/)
{
	AppAssert(sizeof(XMLCh) == sizeof(UChar));

	bool isOk = true;
	unsigned long duration = 0;

	//UTrace("XercesParse buffer", inStr);
	gLog.log(eTypLogDebug, "Deb > Parse string");

	// Initialize the handler
	if (!XercesInitHandler())
		return false;

	const char *memBufId = "EngineParseAtollId"; // Unused id
  int32_t len = inLength == -1 ? u_strlen(inStr) : inLength;
	XMLCh encoding[] = { 'U', 'T', 'F', '-', '1', '6', 'L', 'E', 0 }; // "UTF-16LE"
	const XMLByte *xmlBytes = reinterpret_cast<const XMLByte *>(inStr);
	MemBufInputSource* memBufIS = new MemBufInputSource(xmlBytes, len * sizeof(XMLCh), memBufId, false);
	memBufIS->setEncoding(encoding);

	try {
		mHandler->BeginDocument();
		const unsigned long startMillis = XMLPlatformUtils::getCurrentMillis();
		mParser->parse(*memBufIS);
		const unsigned long endMillis = XMLPlatformUtils::getCurrentMillis();
		duration = endMillis - startMillis;
	}
	catch (const AppException &e) {
		gLog.log(eTypLogError, "Err > Application exception during parse string - %s", e.what());
		isOk = false;
	}
	catch (const OutOfMemoryException &) {
		gLog.log(eTypLogError, "Err > Xerces out of memory exception during parse string");
		isOk = false;
	}
	catch (const XMLException &e) {
		XercesString message(e.getMessage());
		gLog.log(eTypLogError, "Err > Xerces exception during parse string: %s", message.localForm());
		isOk = false;
	}
	catch (std::exception &e) {
		gLog.log(eTypLogError, "Err > Unknown exception during parse string - %s", e.what());
		isOk = false;
	}

	delete memBufIS;

	// Close the document
	mHandler->EndDocument(!isOk);

	if (isOk) {
		// Check handler errors
		unsigned long nbErr = mHandler->GetNbError();
		if (nbErr) {
			gLog.log(eTypLogError, "Err > Parse string handler got %lu error(s)", nbErr);
			isOk = false;
		}
		else {
			gLog.log(eTypLogDebug, "Deb > Parse string in %lu ms", duration);
		}
	}
	else
		gLog.log(eTypLogError, "Err > Parse string error");

	return isOk;
}
//------------------------------------------------------------------------------
