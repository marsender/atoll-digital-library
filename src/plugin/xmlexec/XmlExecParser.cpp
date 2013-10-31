/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XmlExecParser.cpp

*******************************************************************************/

#include "../../Portability.hpp"
#include "XmlExecParser.hpp"
#include "XmlExecHandler.hpp"
//#include "../../common/UnicodeUtil.hpp" // For UTrace
#include "../../common/XercesString.hpp"
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
//------------------------------------------------------------------------------

using namespace AtollPluginXmlExec;
using namespace Common;
XERCES_CPP_NAMESPACE_USE

XmlExecParser::XmlExecParser(SAX2XMLReader::ValSchemes inValScheme, const std::string &inCatalogFile,
														 AtollPlugin::PluginMessage *inPluginMessage, eTypHandlerXmlExec inTypHandler, Common::Logger &inLog)
	:XercesParser(inValScheme, inCatalogFile),
	mTypHandler(inTypHandler),
	mLog(inLog)
{
	// Create and initialize the SAX handler
	switch (mTypHandler) {
	//case eTypHandlerXmlExecBase:
	//	mHandler = new XercesBaseHandler();
	//	break;
	case eTypHandlerXmlExecReq:
		mHandler = new XmlExecHandler(inPluginMessage, inLog);
		break;
	default:
		DEF_Exception(AppException::ParserError, "Incorrect sax handler type");
	}

	XercesParser::InitXercesHandler();
}
//------------------------------------------------------------------------------

XmlExecParser::~XmlExecParser()
{
	// The handler is deleted by the parent class
}
//------------------------------------------------------------------------------

bool XmlExecParser::XercesInitHandler()
{
	bool isOk = true;

	if (!mHandler) {
		mLog.log(eTypLogError, "Err > No XmlExecParser handler");
		return true;
	}

	switch (mTypHandler) {
	case eTypHandlerXmlExecBase:
		break;
	case eTypHandlerXmlExecReq:
		{
			XmlExecHandler *handler = static_cast<XmlExecHandler *>(mHandler);
			isOk = handler->InitHandler();
		}
		break;
	default:
		DEF_Exception(AppException::ParserError, "Incorrect sax handler type");
	}

	return isOk;
}
//------------------------------------------------------------------------------

bool XmlExecParser::XercesParse(const std::string &inFileName)
{
	bool isOk = true;
	unsigned long duration = 0;

	mLog.log(eTypLogAction, "Act > XmlExec parse: %s", inFileName.c_str());

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
		mLog.log(eTypLogError, "Err > Application exception during parsing - %s", e.what());
		isOk = false;
	}
	catch (const OutOfMemoryException &) {
		mLog.log(eTypLogError, "Err > Xerces out of memory exception during parsing");
		isOk = false;
	}
	catch (const XMLException &e) {
		XercesString message(e.getMessage());
		mLog.log(eTypLogError, "Err > Xerces exception during parsing: %s", message.localForm());
		isOk = false;
	}
	catch (std::exception &e) {
		mLog.log(eTypLogError, "Err > Unknown exception during parsing - %s", e.what());
		isOk = false;
	}

	// Close the document
	mHandler->EndDocument(!isOk);

	// Check if there are errors in this file
	unsigned long nbErrFile = mHandler->GetNbError();
	if (nbErrFile)
		isOk = false;

	if (isOk)
		mLog.log(eTypLogAction, "Act > XmlExec parse: %s in %lu ms (%.2f h)", inFileName.c_str(), duration, (double)duration / 3600000.0);
	else
		mLog.log(eTypLogError, "Err > XmlExec parse error: %s", inFileName.c_str());

	return isOk;
}
//------------------------------------------------------------------------------

bool XmlExecParser::XercesParse(const UChar *inStr, int32_t inLength /*= -1*/)
{
	AppAssert(sizeof(XMLCh) == sizeof(UChar));

	bool isOk = true;
	unsigned long duration = 0;

	//UTrace("XercesParse buffer", inStr);
	mLog.log(eTypLogAction, "Act > XmlExec parse string");

	// Initialize the handler
	if (!XercesInitHandler())
		return false;

	const char *memBufId = "XmlExecParseAtollId"; // Unused id
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
		mLog.log(eTypLogError, "Err > Application exception during parsing - %s", e.what());
		isOk = false;
	}
	catch (const OutOfMemoryException &) {
		mLog.log(eTypLogError, "Err > Xerces out of memory exception during parsing");
		isOk = false;
	}
	catch (const XMLException &e) {
		XercesString message(e.getMessage());
		mLog.log(eTypLogError, "Err > Xerces exception during parsing: %s", message.localForm());
		isOk = false;
	}
	catch (std::exception &e) {
		mLog.log(eTypLogError, "Err > Unknown exception during parsing - %s", e.what());
		isOk = false;
	}

	delete memBufIS;

	// Close the document
	mHandler->EndDocument(!isOk);

	// Check if there are parsing errors
	unsigned long nbErrFile = mHandler->GetNbError();
	if (nbErrFile)
		isOk = false;

	if (isOk)
		mLog.log(eTypLogAction, "Act > XmlExec parse string in %lu ms (%.2f h)", duration, (double)duration / 3600000.0);
	else
		mLog.log(eTypLogAction, "Act > XmlExec parse error");

	return isOk;
}
//------------------------------------------------------------------------------
