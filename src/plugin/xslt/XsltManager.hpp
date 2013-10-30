/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XsltManager.hpp

*******************************************************************************/

#ifndef __XsltManager_HPP
#define __XsltManager_HPP
//------------------------------------------------------------------------------

#include <map>
#include "unicode/unistr.h"
#include <xalanc/XalanTransformer/XalanTransformer.hpp>
//------------------------------------------------------------------------------

namespace AtollPluginXslt
{

//! Xslt manager
class XsltManager
{
private:
	typedef std::map <UnicodeString, XALAN_CPP_NAMESPACE::XalanCompiledStylesheet *> CompiledStylesheetMap;

	Common::Logger &mLog;
	XALAN_CPP_NAMESPACE::XalanTransformer *mXalanTransformer;
	CompiledStylesheetMap mCompiledStylesheetMap;

	bool CreateStylesheet(const UnicodeString &inXsltName);

public:
	XsltManager(Common::Logger &inLog);
	~XsltManager();

	//XALAN_CPP_NAMESPACE::XalanTransformer *GetXalanTransformer() { return mXalanTransformer; }
	bool AddStylesheet(const UnicodeString &inXsltName, const char *inXslt);
	const XALAN_CPP_NAMESPACE::XalanCompiledStylesheet *GetCompiledStylesheet(const UnicodeString &inXsltName);
};
//------------------------------------------------------------------------------

} // namespace AtollPluginXslt

//------------------------------------------------------------------------------
#endif
