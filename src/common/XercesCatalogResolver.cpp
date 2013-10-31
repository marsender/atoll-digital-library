/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XercesCatalogResolver.cpp

*******************************************************************************/

#include "XercesCatalogResolver.hpp"
#include "Logger.hpp" // in common directory
#include "AppException.hpp"
#include "XercesString.hpp"
#include <sstream> // For std::ostringstream
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMElement.hpp>
//------------------------------------------------------------------------------

using namespace Common;
XERCES_CPP_NAMESPACE_USE

XercesCatalogResolver::XercesCatalogResolver(bool inInitializeXerces /*= false*/)
	:mInitializeXerces(inInitializeXerces)
{
	gLog.log(eTypLogDebug, "Deb > CatalogResolver create");

	InitXercesParser();
}
//------------------------------------------------------------------------------

XercesCatalogResolver::~XercesCatalogResolver()
{
	CloseXercesParser();
}
//------------------------------------------------------------------------------

void XercesCatalogResolver::InitXercesParser()
{
	char localeStr[64];

	// Init Xerces (XML4C2)
	if (mInitializeXerces) {
		// Xerces should be already initialized
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
	}

	mParser = new XercesDOMParser;

	mParser->setValidationScheme(XercesDOMParser::Val_Never);
	mParser->setDoNamespaces(false);
	mParser->setDoSchema(false);
	mParser->setLoadExternalDTD(false);
}
//------------------------------------------------------------------------------

void XercesCatalogResolver::CloseXercesParser()
{
	delete mParser;

	// Call Xerces shutdown method
	if (mInitializeXerces) {
		// Xerces should be already initialized
		XMLPlatformUtils::Terminate();
	}
}
//------------------------------------------------------------------------------

bool XercesCatalogResolver::LoadCatalog(const std::string &inFileName)
{
	bool isOk = true;
	unsigned long duration = 0, startMillis, endMillis;

	gLog.log(eTypLogDebug, "Deb > CatalogResolver load catalog file: %s", inFileName.c_str());

	mCatalogMap.clear();

	try {
		startMillis = XMLPlatformUtils::getCurrentMillis();
		mParser->parse(inFileName.c_str());
	}
	catch (const AppException &e) {
		gLog.log(eTypLogError, "Err > Application exception during catalog parsing - %s", e.what());
		isOk = false;
	}
	catch (const OutOfMemoryException &) {
		gLog.log(eTypLogError, "Err > Out of memory exception during catalog parsing");
		isOk = false;
	}
	catch (const XMLException &e) {
		XercesString message(e.getMessage());
		gLog.log(eTypLogError, "Err > Xerces exception during catalog parsing: %s", message.localForm());
		isOk = false;
	}
	catch (const DOMException &e) {
		const unsigned int maxChars = 2047;
		XMLCh errText[maxChars + 1];
		if (DOMImplementation::loadDOMExceptionMsg(e.code, errText, maxChars)) {
			XercesString message(errText);
			gLog.log(eTypLogError, "Err > Xerces dom exception code %d during catalog parsing: %s",
				e.code, message.localForm());
		}
		else
			gLog.log(eTypLogError, "Err > Xerces dom exception code %d during catalog parsing", e.code);
		isOk = false;
	}
	catch (std::exception &e) {
		gLog.log(eTypLogError, "Err > Unknown exception during catalog parsing - %s", e.what());
		isOk = false;
	}

	if (isOk) {
		isOk = ReadCatalog();
	}

	endMillis = XMLPlatformUtils::getCurrentMillis();
	duration = endMillis - startMillis;

	bool wantLog = false;
	if (wantLog && isOk)
		gLog.log(eTypLogDebug, "Deb > Parse catalog: %s in %lu ms", inFileName.c_str(), duration);

	if (!isOk)
		gLog.log(eTypLogError, "Err > Error catalog: %s", inFileName.c_str());

	return isOk;
}
//------------------------------------------------------------------------------

bool XercesCatalogResolver::ReadCatalog()
{
	bool isOk = true;

	gLog.log(eTypLogDebug, "Deb > CatalogResolver read catalog file");

	try {
		// no need to free this pointer - owned by the parent parser object
		DOMDocument *xmlDoc = mParser->getDocument();
		if (!xmlDoc)
			return false;

		// Get the top-level root element
		DOMElement *elementRoot = xmlDoc->getDocumentElement();
		if (!elementRoot) {
			gLog.log(eTypLogError, "Err > Empty XML document");
			return false;
		}

		// Look one level nested within root (child of root)
		DOMNodeList *children = elementRoot->getChildNodes();
		const XMLSize_t nodeCount = children->getLength();

		XMLCh *elemPublic = XMLString::transcode("public");
		//XMLCh *elemSystem = XMLString::transcode("system");
		XMLCh *attrPublicId = XMLString::transcode("publicId");
		XMLCh *attrUri = XMLString::transcode("uri");

		// Iterate nodes, children of "root" in the XML tree.
		short nodeType;
		std::string publicId, uri;
		for (XMLSize_t i = 0; i < nodeCount; ++i) {
			DOMNode *currentNode = children->item(i);
			// If node is an element
			nodeType = currentNode->getNodeType();
			if (nodeType == DOMNode::ELEMENT_NODE) { // is element
				// Re-cast node as element
				DOMElement *currentElement = static_cast<xercesc::DOMElement*>(currentNode); // dynamic_cast returns null under linux
				if (!currentElement) {
					gLog.log(eTypLogError, "Err > Null catalog node element");
					isOk = false;
					continue;
				}
				// Use public element
				if (XMLString::equals(currentElement->getTagName(), elemPublic)) {
					// Read attributes of element
					publicId = XMLString::transcode(currentElement->getAttribute(attrPublicId));
					uri = XMLString::transcode(currentElement->getAttribute(attrUri));
					mCatalogMap[publicId] = uri;
				}
				// Other elements: system rewriteURI rewriteSystem delegatePublic delegateSystem
				//else if (!XMLString::equals(currentElement->getTagName(), elemSystem)) {
				//	gLog.log(eTypLogError, "Err > Incorrect catalog node element");
				//	isOk = false;
				//}
			}
		}

		XMLString::release(&elemPublic);
		//XMLString::release(&elemSystem);
		XMLString::release(&attrPublicId);
		XMLString::release(&attrUri);
	}
	catch (const AppException &e) {
		gLog.log(eTypLogError, "Err > Application exception during catalog reading - %s", e.what());
		isOk = false;
	}
	catch (const OutOfMemoryException &) {
		gLog.log(eTypLogError, "Err > Out of memory exception during catalog reading");
		isOk = false;
	}
	catch (const XMLException &e) {
		XercesString message(e.getMessage());
		gLog.log(eTypLogError, "Err > Xerces exception during catalog reading: %s", message.localForm());
		isOk = false;
	}
	catch (std::exception &e) {
		gLog.log(eTypLogError, "Err > Unknown exception during catalog reading - %s", e.what());
		isOk = false;
	}

	return isOk;
}
//------------------------------------------------------------------------------

std::string XercesCatalogResolver::LookupPublicId(const std::string &inPublicId)
{
	std::string uri;

	CatalogMap::const_iterator it = mCatalogMap.find(inPublicId);
	if (it != mCatalogMap.end()) {
		uri = it->second;
	}

	return uri;
}
//------------------------------------------------------------------------------
