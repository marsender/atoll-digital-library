/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XsltTransformer.hpp

*******************************************************************************/

#ifndef __XsltTransformer_HPP
#define __XsltTransformer_HPP
//------------------------------------------------------------------------------

#include "unicode/unistr.h"
#include "unicode/ustring.h"
#include "../../common/Containers.hpp"
#include <xercesc/sax2/SAX2XMLReader.hpp> // SAX2XMLReader::ValSchemes
#include <xalanc/XalanTransformer/XalanTransformer.hpp>
XERCES_CPP_NAMESPACE_USE
//------------------------------------------------------------------------------

namespace Common
{
class XercesEntityResolver;
}

namespace AtollPlugin
{
class PluginMessage;
}

namespace AtollPluginXslt
{
class XsltManager;

//! Xslt transformer
class XsltTransformer
{
private:
	Common::Logger &mLog;
	AtollPlugin::PluginMessage *mPluginMessage;
	XsltManager *mXsltManager;
	Common::XercesEntityResolver *mEntityResolver;

	//! Transform an xml buffer with a compiled stylesheet
	bool XslTransform(const UChar *inStr, int32_t inLength, const XALAN_CPP_NAMESPACE::XalanCompiledStylesheet *inCompiledStylesheet, const Common::StringToUnicodeStringMap &inStylesheetParamMap);

public:
	XsltTransformer(SAX2XMLReader::ValSchemes inValScheme, const std::string &inCatalogFile,
		AtollPlugin::PluginMessage *inPluginMessage, Common::Logger &inLog, XsltManager *inXsltManager);
	virtual ~XsltTransformer();

	//! Transform an xml buffer with an xslt buffer
	virtual bool XslTransformWithBuffer(const UChar *inStr, int32_t inLength, const UChar *inXsl, int32_t inLengthXsl, const Common::StringToUnicodeStringMap &inStylesheetParamMap);
	//! Transform an xml buffer with a compiled stylesheet
	virtual bool XslTransformWithCompiled(const UChar *inStr, int32_t inLength, const UnicodeString &inXsltName, const Common::StringToUnicodeStringMap &inStylesheetParamMap);
};
//------------------------------------------------------------------------------

} // namespace AtollPluginXslt

//------------------------------------------------------------------------------
#endif
