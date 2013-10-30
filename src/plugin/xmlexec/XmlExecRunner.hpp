/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XmlExecRunner.hpp

*******************************************************************************/

#ifndef __XmlExecRunner_HPP
#define __XmlExecRunner_HPP
//------------------------------------------------------------------------------

#include "XmlExecCmd.hpp"
#include "../../common/XercesNode.hpp"
#include "unicode/ustdio.h"
#include <memory> // for std::auto_ptr
//------------------------------------------------------------------------------

namespace Common
{
class Logger;
class XercesNode;
}

namespace Atoll
{
class ColMeta;
class DocMeta;
class FieldMeta;
class EngineEnv;
class SearchCrit;
}

namespace AtollPlugin
{
class PluginMessage;
}

namespace AtollPluginXmlExec
{
class XmlReq;

//! XmlExec engine runner
/**
	Logic:
		- Recieve notification from the parser content handler
		- Execute the requests and send the responses in the plugin message
*/
class XmlExecRunner
{
public:
	XmlExecRunner(AtollPlugin::PluginMessage *inPluginMessage, Common::Logger &inLog);
	~XmlExecRunner();

	//! Init document to parse
	//bool RunnerInitDocument(const std::string &inFileName);

	//! Receive notification of the beginning of the document
	void StartDocument();

	//! Receive notification of the end of the document
	void EndDocument(bool inIsException);

	//! Receive notification of the beginig of an xml node
	void StartNode(const std::string &inElem, const Common::StringToUnicodeStringMap &inAttrMap);
	//! Receive notification of the end of an xml node
	void EndNode();

	//! Receive notification of character data inside an element
	void AddCharacters(const UChar *inStr, unsigned int inLength);

	//! Get the number of error in the current document
	unsigned long GetNbError() const { return mNbError; }

private:
	//! XmlExec engine runner info struct
	/**
		Logic:
			- Store the plugin message pointer
			- Store some usefull parameters for the execution
	*/
	struct Info {
		//std::string mInputFile;
		std::string mOutputFile;
		Atoll::EngineEnv *mEngineEnv;
		AtollPlugin::PluginMessage *mPluginMessage;
	};

	Common::Logger &mLog;
#if defined(WIN32) && !defined(_DEBUG)
	XmlExecCmd::AssocMap mTokenMap; // &mTokenMap crashes on windows release
#else
	XmlExecCmd::AssocMap &mTokenMap;
#endif
	Info *mInfo;
	bool mWantInputBuffer; //!< Store input characters ?
	UnicodeString mInputBuffer; //!< Input character buffer
	unsigned long mNbError;	//!< Number of errors in the document
	Common::XercesNodeVector *mXercesNodeVector;
	Atoll::ColMeta *mColMeta;
	Atoll::DocMeta *mDocMeta;
	Atoll::FieldMeta *mFieldMeta;
	Atoll::SearchCrit *mSearchCrit;
	std::auto_ptr<XmlReq> mXmlReq;

	//! Set the input file that is parsed
	//void SetInputFile(const std::string &inFileNeme);
	//! Set the output file
	void SetOutputFile(const std::string &inFileNeme);

	void OutputText(const char *inStr);
	void OutputText(UnicodeString &inStr);

	//! Tell if the current stack of nodes contains a token
	bool IsParentToken(int inTypToken);

	//! Raz of document infos
	void ClearDocumentInfo();

	//! Release and delete the xml request
	void ReleaseRequest();
};

} // namespace AtollPluginXmlExec

//------------------------------------------------------------------------------
#endif
