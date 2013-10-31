/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XsltTransformer.cpp

*******************************************************************************/

#include "../../Portability.hpp"
#include "XsltTransformer.hpp"
#include "XsltManager.hpp"
#include "../../common/UnicodeUtil.hpp"
#include "../../common/XercesString.hpp"
#include "../../common/XercesEntityResolver.hpp"
#include "../../plugin/PluginMessage.hpp"
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xalanc/Include/PlatformDefinitions.hpp>
#include <xalanc/XalanTransformer/XalanTransformer.hpp>
#if defined(XALAN_CLASSIC_IOSTREAMS)
	#include <iostream.h>
	#include <sstream.h>
	#include <fstream.h>
#else
	//#include <iostream>
	#include <sstream>
	//#include <fstream>
#endif
//------------------------------------------------------------------------------

using namespace Common;
using namespace AtollPluginXslt;
XERCES_CPP_NAMESPACE_USE
XALAN_CPP_NAMESPACE_USE
//------------------------------------------------------------------------------

//! XalanTransformer callback
/**
	Logic:
		- Write in the string passed by reference in the constructor
*/
class CallbackHandler
{
private:
	std::string &mStr;

public:
	//! Callback constuctor: store the reference of the string
	CallbackHandler(std::string &inStr)
		:mStr(inStr)
	{
	}

	//! Write function for the transformer
	CallbackSizeType TransformerWrite(const char *inStr, CallbackSizeType inLength)
	{
		mStr.append(inStr, inLength);

		return inLength;
	}

	//! Flush function for the transformer
	void TransformerFlush() const
	{
		// Nothing to do
	}
};
//------------------------------------------------------------------------------

// These functions need to have C linkage
extern "C"
{
	//! XalanTransformer write callback function
	CallbackSizeType WriteCallback(const char * theData, CallbackSizeType theLength, void *theHandle)
	{
		return (reinterpret_cast<CallbackHandler *>(theHandle))->TransformerWrite(theData, theLength);
	}

	//! XalanTransformer flush callback function
	void FlushCallback(void *theHandle)
	{
		(reinterpret_cast<CallbackHandler *>(theHandle))->TransformerFlush();
	}
} // extern "C"
//------------------------------------------------------------------------------

XsltTransformer::XsltTransformer(SAX2XMLReader::ValSchemes inValScheme, const std::string &inCatalogFile,
																 AtollPlugin::PluginMessage *inPluginMessage, Common::Logger &inLog,
																 XsltManager *inXsltManager)
	:mLog(inLog),
	mPluginMessage(inPluginMessage),
	mXsltManager(inXsltManager)
{
	if (mXsltManager == NULL)
		DEF_Exception(AppException::NullPointer, "Xslt transformer called with a NULL xslt manager");

	mEntityResolver = NULL;
	if (!inCatalogFile.empty()) {
		mEntityResolver = new XercesEntityResolver(inValScheme, inCatalogFile);
	}
}
//------------------------------------------------------------------------------

XsltTransformer::~XsltTransformer()
{
	if (mEntityResolver)
		delete mEntityResolver;
}
//------------------------------------------------------------------------------

bool XsltTransformer::XslTransform(const UChar *inStr, int32_t inLength,
																	 const XalanCompiledStylesheet *inCompiledStylesheet,
																	 const StringToUnicodeStringMap &inStylesheetParamMap)
{
	bool isOk = true;
	unsigned long duration = 0;

	mLog.log(eTypLogAction, "Act > Xslt transform string");

	bool wantTrace = false;
	if (wantTrace) {
		UTrace("XslTransform: Xml source", inStr, inLength);
	}

	try {
		const unsigned long startMillis = XMLPlatformUtils::getCurrentMillis();
		// Create input source and output handler
		std::string xmlStr, callbackStr;
		ConvertUnicodeString2String(xmlStr, inStr, inLength);
		std::istringstream xmlStream(xmlStr); //.c_str(), (std::streamsize)xmlStr.length());
		XSLTInputSource xmlSource(xmlStream);
		CallbackHandler callbackHandler(callbackStr);

		// Create and initialise the transformer
		XalanTransformer *xalanTransformer = new XalanTransformer();
		//xalanTransformer->setIndent(2); // Never force indentation or inline tags will be indented
		xalanTransformer->setUseValidation(false);
		//xalanTransformer->setOmitMETATag(XalanTransformer::eOmitMETATagYes);

		// Set the stylesheet parameters
		if (inStylesheetParamMap.size()) {
			// Stylesheet parameters are "sticky"
			//xalanTransformer->clearStylesheetParams(); // Not available before xalan 1.11
			StringToUnicodeStringMap::const_iterator it = inStylesheetParamMap.begin();
			StringToUnicodeStringMap::const_iterator itEnd = inStylesheetParamMap.end();
			for (; it != itEnd; ++it) {
				const std::string &paramKey = it->first;
				std::string value;
				ConvertUnicodeString2String(value, it->second);
				// !!! Must quote string parameters !!!
				std::string paramValue("'");
				paramValue += value;
				paramValue += "'";
				// Sample: xalanTransformer->setStylesheetParam("atoll.highwords", "' 1 2 3 4 '");
				xalanTransformer->setStylesheetParam(paramKey.c_str(), paramValue.c_str());
			}
		}

		// Parse the source
		const XalanParsedSource *parsedSource = NULL;
		{
			int res = xalanTransformer->parseSource(xmlSource, parsedSource);
			if (res != 0) {
				mLog.log(eTypLogError, "Err > Xslt parse source error: %s", xalanTransformer->getLastError());
				isOk = false;
			}
		}

		// Do the transform
		if (isOk) {
			int res = xalanTransformer->transform(*parsedSource, inCompiledStylesheet,
				&callbackHandler, WriteCallback, FlushCallback);
			if (res != 0) {
				mLog.log(eTypLogError, "Err > Xslt transform error: %s", xalanTransformer->getLastError());
				isOk = false;
			}
		}

		// Delete explicitely the parsed source
		if (parsedSource)
			xalanTransformer->destroyParsedSource(parsedSource);

		delete xalanTransformer;

		if (isOk) {
			// Convert and store the result
			ConvertString2UnicodeString(mPluginMessage->mMessage, callbackStr);
		}

		const unsigned long endMillis = XMLPlatformUtils::getCurrentMillis();
		duration = endMillis - startMillis;
	}
	catch (const OutOfMemoryException &) {
		mLog.log(eTypLogError, "Err > Xerces out of memory exception during xslt");
		isOk = false;
	}
	catch (const XMLException &e) {
		XercesString message(e.getMessage());
		mLog.log(eTypLogError, "Err > Xerces exception during xslt: %s", message.localForm());
		isOk = false;
	}
	catch (std::exception &e) {
		mLog.log(eTypLogError, "Err > Unknown exception during xslt - %s", e.what());
		isOk = false;
	}

	if (wantTrace) {
		UTrace("XslTransform: Xml output", mPluginMessage->mMessage.getBuffer(), mPluginMessage->mMessage.length());
	}

	if (isOk)
		mLog.log(eTypLogAction, "Act > Xslt transform string in %lu ms", duration);

	return isOk;
}
//------------------------------------------------------------------------------

bool XsltTransformer::XslTransformWithBuffer(const UChar *inStr, int32_t inLength,
																						 const UChar *inXsl, int32_t inLengthXsl,
																						 const StringToUnicodeStringMap &inStylesheetParamMap)
{
	bool isOk = true;

	// Create and initialise the transformer
	XalanTransformer *xalanTransformer = new XalanTransformer();

	// Compile the stylesheet
	const XalanCompiledStylesheet *compiledStylesheet = NULL;
	try {
		std::string xslStr;
		ConvertUnicodeString2String(xslStr, inXsl, inLengthXsl);
		std::istringstream xslStream(xslStr); //.c_str(), (std::streamsize)xslStr.length());
		XSLTInputSource xslSource(xslStream);
		int res = xalanTransformer->compileStylesheet(xslSource, compiledStylesheet);
		if (res != 0) {
			mLog.log(eTypLogError, "Err > Xslt compile stylesheet error: %s", xalanTransformer->getLastError());
			isOk = false;
		}
	}
	catch (const OutOfMemoryException &) {
		mLog.log(eTypLogError, "Err > Xerces out of memory exception during compile stylesheet");
		isOk = false;
	}
	catch (const XMLException &e) {
		XercesString message(e.getMessage());
		mLog.log(eTypLogError, "Err > Xerces exception during compile stylesheet: %s", message.localForm());
		isOk = false;
	}
	catch (std::exception &e) {
		mLog.log(eTypLogError, "Err > Unknown exception during compile stylesheet - %s", e.what());
		isOk = false;
	}

	if (isOk)
		isOk = XslTransform(inStr, inLength, compiledStylesheet, inStylesheetParamMap);

	// Delete explicitely the compiled stylesheet
	if (compiledStylesheet)
		xalanTransformer->destroyStylesheet(compiledStylesheet);

	delete xalanTransformer;

	return isOk;
}
//------------------------------------------------------------------------------

bool XsltTransformer::XslTransformWithCompiled(const UChar *inStr, int32_t inLength,
																							 const UnicodeString &inXsltName,
																							 const StringToUnicodeStringMap &inStylesheetParamMap)
{
	bool isOk = true;

	// Get the stylesheet
	const XalanCompiledStylesheet *compiledStylesheet = mXsltManager->GetCompiledStylesheet(inXsltName);
	if (compiledStylesheet == NULL) {
		mLog.log(eTypLogError, "Err > XslTransform: No compiled stylesheet");
		isOk = false;
	}

	if (isOk)
		isOk = XslTransform(inStr, inLength, compiledStylesheet, inStylesheetParamMap);

	return isOk;
}
//------------------------------------------------------------------------------
