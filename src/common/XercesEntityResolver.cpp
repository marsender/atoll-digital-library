/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XercesEntityResolver.cpp

*******************************************************************************/

#include "XercesEntityResolver.hpp"
#include "FileSystem.hpp" // in common directory
#include "Logger.hpp" // in common directory
#include "XercesCatalogResolver.hpp"
#include "XercesString.hpp"
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/URLInputSource.hpp>
//------------------------------------------------------------------------------

using namespace Common;
XERCES_CPP_NAMESPACE_USE
//------------------------------------------------------------------------------

XercesEntityResolver::XercesEntityResolver(SAX2XMLReader::ValSchemes inValScheme,
																					 const std::string &inCatalogFile)
	:EntityResolver(),
	XMLEntityResolver(),
	mCatalogFile(inCatalogFile),
	mValScheme(inValScheme)
{
	gLog.log(eTypLogDebug, "Deb > EntityResolver create");
}
//------------------------------------------------------------------------------

XercesEntityResolver::~XercesEntityResolver()
{
}
//------------------------------------------------------------------------------

InputSource *XercesEntityResolver::resolveEntity(const XMLCh * const inPublicId,
																							 const XMLCh * const inSystemId)
{
	// Returns the resolved entity or null to let the parser resolve
	// In order to disable the dtd: return an empty memory source
	// CatalogFile sample: "C:\\Program Files\\XML Copy Editor\\catalog\\catalog"

	gLog.log(eTypLogDebug, "Deb > EntityResolver resolve entity");

	if (!inPublicId || *inPublicId == 0 || mCatalogFile.empty()) {
		if (mValScheme != SAX2XMLReader::Val_Never)
			return NULL;
	}

	// Load the catalog
	std::string uri;
	if (inPublicId && *inPublicId && mValScheme != SAX2XMLReader::Val_Never && !mCatalogFile.empty()) {
		if (FileSystem::FileExists(mCatalogFile)) {
			XercesString systemId(inSystemId); // Remote dtd that won't be used
			XercesString publicId(inPublicId);
			XercesCatalogResolver catalog;
			std::string fileName(mCatalogFile.c_str()), strPublicId(publicId.localForm());
			// Load the catalog
			if (!catalog.LoadCatalog(fileName))
				gLog.log(eTypLogError, "Err > Unable to load catalog: %s", mCatalogFile.c_str());
			// Get the uri
			uri = catalog.LookupPublicId(strPublicId);
			if (uri.empty()) {
				//gLog.log(eTypLogWarning, "Wrn > Catalog file publicId doesn't exist: %s", strPublicId.c_str());
				// uri = systemId.localForm(); // Is ok to resolve
				return NULL; // To let the parser resolve
			}
		}
		else {
			gLog.log(eTypLogError, "Err > Catalog file doesn't exist: %s", mCatalogFile.c_str());
		}
	}

	InputSource *inputSource;
	if (!uri.empty()) {
		bool isUrl = (strncmp(uri.c_str(), "file:", 5) == 0) || (strncmp(uri.c_str(), "http:", 5) == 0);
		if (isUrl) {
			// uri: "file:///C:/test/dtd/test.dtd"
			XMLCh *fileName = XMLString::transcode(uri.c_str());
			inputSource = new URLInputSource(fileName);
			XMLString::release(&fileName);
		}
		else {
			// uri: "C:\\test\\dtd\\test.dtd"
			XMLCh *fileName = XMLString::transcode(uri.c_str());
			inputSource = new LocalFileInputSource(fileName);
			XMLString::release(&fileName);
		}
	}
	else {
		//if (mValScheme == SAX2XMLReader::Val_Never)
		//	gLog.log(eTypLogWarning, "Wrn > No entity resolver as validation scheme is Val_Never");
		XMLByte *srcDocBytes = NULL;
		unsigned int byteCount = 0;
		XMLCh *bufID = NULL;
		inputSource = new MemBufInputSource(srcDocBytes, byteCount, bufID);
	}

	return inputSource;

	/*
	if (mValScheme != SAX2XMLReader::Val_Never)
		return NULL;

	XercesString publicId(inPublicId);
	XercesString systemId(inSystemId);
	std::string strPublicId = publicId.localForm();
	std::string strSystemId = systemId.localForm();

	// Returns the resolved entity or null to let the parser do it
	// In order to avoid the entity scan, returns an empty memory source
	// Other possibilities:
  //   return new LocalFileInputSource(gRedirectToFile);
	//   return new URLInputSource(...);
	XMLByte *srcDocBytes = NULL;
	unsigned int byteCount = 0;
	XMLCh *bufID = NULL;
	InputSource *inputSource = new MemBufInputSource(srcDocBytes, byteCount, bufID);

	return inputSource;
	*/
}
//------------------------------------------------------------------------------

InputSource *XercesEntityResolver::resolveEntity(XMLResourceIdentifier *)
{
	// See Xalan sample: xalan\samples\EntityResolver\EntityResolver.cpp

	gLog.log(eTypLogDebug, "Deb > EntityResolver resolve entity for a resource identifier");

	return NULL;
}
//------------------------------------------------------------------------------
