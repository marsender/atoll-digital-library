/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XmlReq.hpp

*******************************************************************************/

#ifndef __XmlReq_HPP
#define __XmlReq_HPP
//------------------------------------------------------------------------------

#include "../XmlExecCmd.hpp"
#include "unicode/unistr.h"
#include <memory> // for std::unique_ptr
//------------------------------------------------------------------------------

namespace Atoll
{
class EngineEnv;
class SearchCrit;
class SearchRecord;
}

namespace AtollPlugin
{
class PluginMessage;
}

namespace AtollPluginXmlExec
{

// Types de resultats de recherche
enum eTypResRech {
  eResRechNone,
  eResRechDoc,
  eResRechPge,
  eResRechPos,
  eResRechEnd
};

// Styles de resultats de recherche
enum eStyleResRech {
	eStyleResRechNone,
	eStyleResRechNormal,
	eStyleResRechCompact,
	eStyleResRechEnd
};

//! XmlExec base class for any xml query
class XmlReq
{
public:
	unsigned int mDoc;
	unsigned long mPge, mPos;
	unsigned long mMin, mNb, mMax;
	Atoll::SearchRecord *mSearchRecord;

	XmlReq(Common::Logger &inLog, AtollPlugin::PluginMessage &inPluginMessage,
		Atoll::EngineEnv &inEngineEnv);
	virtual ~XmlReq();

	// Don't call a virtual function in a class constructor
	virtual void Clear();
	virtual bool Execute() = 0;

	//! Get the xml query command type: eReqDoc, eReqSearch
	eXmlCmd GetXmlCmdReq() const { return mCmdReq; }

	void ClearSearchCrit();
	void AddSearchCrit(const Atoll::SearchCrit &inSearchCrit);
	void SetSearchId(const std::string &inSearchId);
	//! Set the result type (Document, page or position)
	void SetTypResRech(short inTypResRech);
	//! Set the result style (Normal or compact)
	void SetStyleResRech(short inStyleResRech);

	void Printf(const char *inStr, ...);
	void OutputStr(const char *inStr);
	void OutputStr(const UChar *inStr);
	void OutputStr(const UnicodeString &inStr);
	void OutputXml(const char *inStr);
	void OutputXml(const UChar *inStr);
	void OutputXml(const UnicodeString &inStr);
	void OutputSearchId();
	void OutputPlage();
	void OutputHeader();
	void OutputFooter();
	/*
	void OutputTeteTexte();
	void OutputTexte(const char *inStr);
	void OutputPiedTexte();
	void OutputAnchor(eTypAnchor inTypAnchor, Entry *inEntry = NULL, unsigned long inNumAnchor = 0);
	void OutputCloseJmp();
	void OutputOpenJmp(Token *inTok);
	void OutputUserColIdReq(bool inIdReq = true);
	void OutputSearchCrit();
	*/

protected:
	Common::Logger &mLog;
	Atoll::EngineEnv &mEngineEnv;
	eXmlCmd mCmdReq, mCmdRes;
	short mTypResRech; //!< Result type (Document, page or position)
	short mStyleResRech; //!< Result style (Normal or compact)

private:
	char *mBuf; //!< Printf output buffer
	AtollPlugin::PluginMessage &mPluginMessage;
};
//------------------------------------------------------------------------------

} // namespace AtollPluginXmlExec

//------------------------------------------------------------------------------
#endif
