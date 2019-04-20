/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XercesParser.cpp

*******************************************************************************/

#include "XercesParser.hpp"
#include "AppException.hpp" // in common directory
#include "FileSystem.hpp" // in common directory
#include "Logger.hpp" // in common directory
#include "XercesBaseHandler.hpp" // in common directory
#include "XercesEntityResolver.hpp"
#include "XercesString.hpp" // in common directory
#include <sstream> // For std::ostringstream
#include <xercesc/util/PlatformUtils.hpp>
//#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
//------------------------------------------------------------------------------

#if defined(WIN32)
	#define DEF_DefaultCatalogFile  "C:\\Program Files\\XML Copy Editor\\catalog\\catalog"
	#define DEF_DefaultCatalogFile2 "C:\\Program Files (x86)\\XML Copy Editor\\catalog\\catalog"
#else
	#define DEF_DefaultCatalogFile  "/usr/share/xml/schema/xml-core/catalog.xml"
#endif
//------------------------------------------------------------------------------

using namespace Common;
XERCES_CPP_NAMESPACE_USE

std::string XercesParser::mStaticDefaultCatalogFile;

XercesParser::XercesParser(SAX2XMLReader::ValSchemes inValScheme,
													 const std::string &inCatalogFile) :
	mCatalogFile(inCatalogFile)
{
	mValScheme = inValScheme;
	mParser = NULL;
	mHandler = NULL;
	mEntityResolver = NULL;

	// No xml validation for auto validation if the catalog file is not set
	if (mValScheme == SAX2XMLReader::Val_Auto && mCatalogFile.empty()) {
		mValScheme = SAX2XMLReader::Val_Never;
	}
	
	gLog.log(eTypLogDebug, "Deb > XercesParser create - CatalogFile [%s] - ValSheme %d", inCatalogFile.c_str(), mValScheme);

	InitXercesParser();
}
//------------------------------------------------------------------------------

XercesParser::~XercesParser()
{
	CloseXercesParser();
}
//------------------------------------------------------------------------------

const std::string &XercesParser::StaticGetDefaultCatalogFile()
{
	if (mStaticDefaultCatalogFile.empty()) {
		if (FileSystem::FileExists(DEF_DefaultCatalogFile))
			mStaticDefaultCatalogFile = DEF_DefaultCatalogFile;
#if defined(WIN32)
		if (FileSystem::FileExists(DEF_DefaultCatalogFile2))
			mStaticDefaultCatalogFile = DEF_DefaultCatalogFile2;
#endif
	}

	if (mStaticDefaultCatalogFile.empty()) {
		gLog.log(eTypLogDebug, "Deb > XercesParser has no default catalog file");
	}

	return mStaticDefaultCatalogFile;
}
//------------------------------------------------------------------------------

void XercesParser::InitXercesParser()
{
	char localeStr[64];
	bool doNamespaces = true;
	bool doSchema = true;
	bool schemaFullChecking = false;
	bool identityConstraintChecking = true;
	bool namespacePrefixes = false;

	// Init du type de validation
	switch (mValScheme) {
	case SAX2XMLReader::Val_Never:
	case SAX2XMLReader::Val_Auto:
	case SAX2XMLReader::Val_Always:
		break;
	default:
		std::ostringstream oss;
		oss << "Parser init Xml validation sheme not managed: "
			<< mValScheme;
		//gLog.log(eTypLogError, "Err > %s", oss.str().c_str());
		DEF_Exception(AppException::ParserError, oss.str());
	}

	// Init Xerces (XML4C2)
  memset(localeStr, 0, sizeof localeStr);
	try {
		if (strlen(localeStr)) {
			XMLPlatformUtils::Initialize(localeStr);
		}
		else {
			XMLPlatformUtils::Initialize();
		}
	}
	catch (const XMLException &e) {
		XercesString message(e.getMessage());
		std::ostringstream oss;
		oss << "Xerces exception during parser initialization: " << message.localForm();
		//gLog.log(eTypLogError, "Err > %s", oss.str().c_str());
		DEF_Exception(AppException::ParserError, oss.str());
	}

	// Create and initialize the parser
	// Xerces Api:
	//   http://xerces.apache.org/xerces-c/apiDocs-3/classSAX2XMLReader.html
	mParser = XMLReaderFactory::createXMLReader();
	mParser->setFeature(XMLUni::fgSAX2CoreNameSpaces, doNamespaces);
	mParser->setFeature(XMLUni::fgXercesSchema, doSchema);
	mParser->setFeature(XMLUni::fgXercesSchemaFullChecking, schemaFullChecking);
	mParser->setFeature(XMLUni::fgXercesIdentityConstraintChecking, identityConstraintChecking);
	mParser->setFeature(XMLUni::fgSAX2CoreNameSpacePrefixes, namespacePrefixes);

	//mValScheme = SAX2XMLReader::Val_Never;
	//mParser->setFeature(XMLUni::fgXercesLoadExternalDTD, false);
	switch (mValScheme) {
	case SAX2XMLReader::Val_Never:
		mParser->setFeature(XMLUni::fgSAX2CoreValidation, false);
		break;
	case SAX2XMLReader::Val_Auto:
		mParser->setFeature(XMLUni::fgSAX2CoreValidation, true);
		mParser->setFeature(XMLUni::fgXercesDynamic, true);
		break;
	case SAX2XMLReader::Val_Always:
		mParser->setFeature(XMLUni::fgSAX2CoreValidation, true);
		mParser->setFeature(XMLUni::fgXercesDynamic, false);
		break;
	}

	// Install the entity resolver
	if (!mCatalogFile.empty()) {
		mEntityResolver = new XercesEntityResolver(mValScheme, mCatalogFile);
		mParser->setEntityResolver(mEntityResolver);
		gLog.log(eTypLogDebug, "Deb > XercesParser set EntityResolver for catalog file [%s]", mCatalogFile.c_str());
	}
	else {
		gLog.log(eTypLogDebug, "Deb > XercesParser init without catalog file");
	}

	// At this point, the SAX handler must be created and initialised
	// by the derived class.
	// Example:
	//   mHandler = new XercesBaseHandler();
	// Then, the derived class must call the folowing method:
	//   XercesParser::InitXercesHandler()
}
//------------------------------------------------------------------------------

void XercesParser::InitXercesHandler()
{
	if (mHandler) {
		mParser->setContentHandler(mHandler);
		mParser->setErrorHandler(mHandler);
	}
}
//------------------------------------------------------------------------------

void XercesParser::CloseXercesParser()
{
	delete mParser;
	if (mEntityResolver)
		delete mEntityResolver;
	if (mHandler)
		delete mHandler;

	// Call Xerces shutdown method
	XMLPlatformUtils::Terminate();
}
//------------------------------------------------------------------------------
