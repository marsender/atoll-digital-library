/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XsltManager.cpp

*******************************************************************************/

#include "../../Portability.hpp"
#include "XsltManager.hpp"
#include "../../common/UnicodeUtil.hpp"
#include "../../common/XercesString.hpp"
#include <xercesc/util/PlatformUtils.hpp>
//#include <xercesc/util/OutOfMemoryException.hpp>
#include <xalanc/Include/PlatformDefinitions.hpp>
#include <xalanc/XalanTransformer/XalanTransformer.hpp>
//#include <xalanc/XalanTransformer/XercesDOMWrapperParsedSource.hpp>
//#include <xalanc/XercesParserLiaison/XercesDOMSupport.hpp>
//#include <xalanc/XercesParserLiaison/XercesParserLiaison.hpp>
#if defined(XALAN_CLASSIC_IOSTREAMS)
	//#include <iostream.h>
	#include <sstream.h>
	//#include <fstream.h>
#else
	//#include <iostream>
	#include <sstream>
	//#include <fstream>
#endif
//------------------------------------------------------------------------------

#if defined(WIN32)
	#define DEF_XslPath "c:/work/dev/atoll/xsl"
#else
	#define DEF_XslPath "/opt/dev/atoll/xsl"
#endif

using namespace Common;
using namespace AtollPluginXslt;
XERCES_CPP_NAMESPACE_USE
XALAN_CPP_NAMESPACE_USE
//------------------------------------------------------------------------------

XsltManager::XsltManager(Common::Logger &inLog)
	:mLog(inLog)
{
	try {
		// Call the static initializer for Xerces
		XMLPlatformUtils::Initialize();

		// Initialize Xalan
		XalanTransformer::initialize();
	}
	catch (const XMLException &e) {
		XercesString message(e.getMessage());
		mLog.log(eTypLogError, "Err > Xerces exception during xalan initialize: %s", message.localForm());
	}
	catch (std::exception &e) {
		mLog.log(eTypLogError, "Err > Unknown exception during xalan initialize: %s", e.what());
	}

	// Initialise the transformer
	mXalanTransformer = new XalanTransformer;
	//mXalanTransformer->setIndent(2); // Never force indentation or inline tags will be indented
	mXalanTransformer->setUseValidation(false);
	//mXalanTransformer->setOmitMETATag(XalanTransformer::eOmitMETATagYes);

	// Add all stylesheets at startup or later, as requested
	bool lazyLoad = true;
	if (!lazyLoad) {
		CreateStylesheet("identity");
		CreateStylesheet("highwords");
		CreateStylesheet("docbook-xsl");
		CreateStylesheet("docbook-xsl-ns");
		CreateStylesheet("tei-xsl");
	}
}
//------------------------------------------------------------------------------

XsltManager::~XsltManager()
{
	try {
		// Delete explicitely the compiled stylesheet
		const XalanCompiledStylesheet *compiledStylesheet;
		CompiledStylesheetMap::const_iterator itCol = mCompiledStylesheetMap.begin();
		CompiledStylesheetMap::const_iterator itColEnd = mCompiledStylesheetMap.end();
		for (; itCol != itColEnd; ++itCol) {
			compiledStylesheet = itCol->second;
			mXalanTransformer->destroyStylesheet(compiledStylesheet);
		}

		delete mXalanTransformer;
	}
	catch (const XMLException &e) {
		XercesString message(e.getMessage());
		mLog.log(eTypLogError, "Err > Xerces exception during xalan transformer delete: %s", message.localForm());
	}
	catch (std::exception &e) {
		mLog.log(eTypLogError, "Err > Unknown exception during xalan transformer delete: %s", e.what());
	}

	try {
		// Terminate Xalan
		XalanTransformer::terminate();

		// Terminate Xerces
		XMLPlatformUtils::Terminate();
	}
	catch (const XMLException &e) {
		XercesString message(e.getMessage());
		mLog.log(eTypLogError, "Err > Xerces exception during xalan terminate: %s", message.localForm());
	}
	catch (std::exception &e) {
		mLog.log(eTypLogError, "Err > Unknown exception during xalan terminate: %s", e.what());
	}

	// Clean up the ICU, if it's integrated
	// Warning: This clean up makes the engine collator crash
	// since the ICU will no longer be in a usable state
	//XalanTransformer::ICUCleanUp();
}
//------------------------------------------------------------------------------

bool XsltManager::AddStylesheet(const UnicodeString &inXsltName, const char *inXslt)
{
	//std::string xslFile(DEF_XslPath);
	//xslFile += "/docbook_html.xsl";
	//const XalanDOMString xslInputFile(xslFile.c_str()); // "d:\\_cvs\\dev\\atoll\\xsl\\docbook_html.xsl"
	//const XSLTInputSource	xslInputSource(xslInputFile);

	std::istringstream xslStream(inXslt); //, (std::streamsize)strlen(inXslt));
	XSLTInputSource xslSource(xslStream);

	const XalanCompiledStylesheet *compiledStylesheet = NULL;
	int res = mXalanTransformer->compileStylesheet(xslSource, compiledStylesheet); // xslSource or xslInputSource
	if (res != 0) {
		mLog.log(eTypLogError, "Err > Xslt compile stylesheet error: %s", mXalanTransformer->getLastError());
		return false;
	}

	mCompiledStylesheetMap[inXsltName] = (XalanCompiledStylesheet *)compiledStylesheet;

	return true;
}
//------------------------------------------------------------------------------

const XalanCompiledStylesheet *XsltManager::GetCompiledStylesheet(const UnicodeString &inXsltName)
{
	CompiledStylesheetMap::const_iterator it;
	const XalanCompiledStylesheet *compiledStylesheet;

	// Search in the compiled stylesheet map
	it = mCompiledStylesheetMap.find(inXsltName);
	if (it != mCompiledStylesheetMap.end()) {
		compiledStylesheet = it->second;
		return compiledStylesheet;
	}

	// Create the stylesheet
	CreateStylesheet(inXsltName);

	// Try a new search in the compiled stylesheet map
	it = mCompiledStylesheetMap.find(inXsltName);
	if (it != mCompiledStylesheetMap.end()) {
		compiledStylesheet = it->second;
		return compiledStylesheet;
	}

	std::string name = UnicodeString2String(inXsltName);
	mLog.log(eTypLogError, "Err > Cannot find a compiled stylesheet for: %s", name.c_str());

	return NULL;
}
//------------------------------------------------------------------------------

bool XsltManager::CreateStylesheet(const UnicodeString &inXsltName)
{
	const char *identityXsl = "<?xml version='1.0'?>\
<xsl:stylesheet\
	xmlns:xsl='http://www.w3.org/1999/XSL/Transform'\
	xmlns:html='http://www.w3.org/1999/xhtml'\
	exclude-result-prefixes='html'\
	version='1.0'>\
	<xsl:include href='%s/identity_param.xsl'/>\
</xsl:stylesheet>";

	const char *highwordsXsl = "<?xml version='1.0'?>\
<xsl:stylesheet\
	xmlns:xsl='http://www.w3.org/1999/XSL/Transform'\
	xmlns:html='http://www.w3.org/1999/xhtml'\
	exclude-result-prefixes='html'\
	version='1.0'>\
	<xsl:include href='%s/highwords_param.xsl'/>\
</xsl:stylesheet>";

	const char *docbookXsl = "<?xml version='1.0'?>\
<xsl:stylesheet\
	xmlns:xsl='http://www.w3.org/1999/XSL/Transform'\
	xmlns:exsl='http://exslt.org/common'\
	xmlns:d='http://docbook.org/ns/docbook'\
	xmlns:doc='http://nwalsh.com/xsl/documentation/1.0'\
	exclude-result-prefixes='exsl doc d'\
	version='1.0'>\
	<xsl:import href='%s/docbook-xsl/html/docbook.xsl'/>\
	<xsl:include href='%s/docbook_param.xsl'/>\
</xsl:stylesheet>";

	const char *docbookXslNs = "<?xml version='1.0'?>\
<xsl:stylesheet\
	xmlns:xsl='http://www.w3.org/1999/XSL/Transform'\
	xmlns:d='http://docbook.org/ns/docbook'\
	xmlns:doc='http://nwalsh.com/xsl/documentation/1.0'\
	exclude-result-prefixes='doc d'\
	version='1.0'>\
	<xsl:import href='%s/docbook-xsl-ns/html/docbook.xsl'/>\
	<xsl:include href='%s/docbook_param.xsl'/>\
</xsl:stylesheet>";

	const char *teiXsl = "<?xml version='1.0'?>\
<xsl:stylesheet\
	xmlns:xsl='http://www.w3.org/1999/XSL/Transform'\
	xmlns:xd='http://www.pnp-software.com/XSLTdoc'\
	xmlns:tei='http://www.tei-c.org/ns/1.0'\
	exclude-result-prefixes='tei xd'\
	version='1.0'>\
	<xsl:import href='%s/tei-xsl/xhtml2/tei.xsl'/>\
	<xsl:include href='%s/tei_param.xsl'/>\
</xsl:stylesheet>";

	// Construct the xslt
	char buf[500];
	const char *styleSheet = NULL;
	if (inXsltName == "identity") {
		styleSheet = identityXsl;
		sprintf(buf, styleSheet, DEF_XslPath);
	}
	else if (inXsltName == "highwords") {
		styleSheet = highwordsXsl;
		sprintf(buf, styleSheet, DEF_XslPath);
	}
	else if (inXsltName == "docbook-xsl") {
		styleSheet = docbookXsl;
		sprintf(buf, styleSheet, DEF_XslPath, DEF_XslPath);
	}
	else if (inXsltName == "docbook-xsl-ns") {
		styleSheet = docbookXslNs;
		sprintf(buf, styleSheet, DEF_XslPath, DEF_XslPath);
	}
	else if (inXsltName == "tei-xsl") {
		styleSheet = teiXsl;
		sprintf(buf, styleSheet, DEF_XslPath, DEF_XslPath);
	}

	// Add the stylesheet
	bool isOk = false;
	if (styleSheet)
		isOk = AddStylesheet(inXsltName, buf);

	return isOk;
}
//------------------------------------------------------------------------------
