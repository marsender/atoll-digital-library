/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

EngineApi.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "EngineApi.hpp"
#include "EngineEnv.hpp"
#include "../collection/Collection.hpp"
#include "../collection/ColManager.hpp"
#include "../common/Containers.hpp"
#include "../common/StdString.hpp"
#include "../common/UnicodeUtil.hpp"
#include "../database/DbDoc.hpp"
#include "../database/DbManager.hpp"
#include "../database/DbNdx.hpp"
#include "../indexer/IndexerConfigParam.hpp"
#include "../plugin/PluginKernel.hpp"
#include "../plugin/PluginMessage.hpp"
#include "../searcher/QueryResolver.hpp"
#include "../searcher/Searcher.hpp"
#include "../util/DocMeta.hpp"
#include "../util/DocRecord.hpp"
#include "../util/SearchRecord.hpp"
#include "unicode/coll.h"
#include <algorithm>
#include <fstream>
#include <zlib.h>
#include <xercesc/util/XercesVersion.hpp>
#include <xalanc/Include/XalanVersion.hpp>
//------------------------------------------------------------------------------

// Library version
#define DEF_AtollVersion "1.6"
// 01/10/13 1.6 Add unique id to docmeta
// 01/08/13 1.5 Indexation optimisation
// 27/04/13 1.4 Doc metadata refactoring
// 29/06/08 1.3 Refactoring with engine library
// 22/05/08 1.2 Use common classes and new log system
// 14/03/08 1.1 Finished basic indexation implementation
// 01/07/07 1.0 Begining of implementation

#define DEF_LibName "Atoll engine library"
#define DEF_XsltParamHighwords "atoll.highwords"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

Common::Logger gLog; //!< Logger instantiation that can be used by all classes
std::ofstream gLogStream;
AtollPlugin::PluginKernel *gPluginKernel = NULL;
extern Collator *gCollator; //!< The global collator instantiation is in the container file
const bool cWantCollator = true; //!< Use icu collator ?
//------------------------------------------------------------------------------

const char *Atoll::GetFullVersion()
{
	static char version[255];

	sprintf(version, "%s %s - Icu %s - %s - Xerces %s - Xalan %s", GetLibName(), GetLibVersion(),
		GetIcuVersion(), GetDbVersion(), GetXercesVersion(), GetXalanVersion());

	return version;
}
//------------------------------------------------------------------------------

const char *Atoll::GetLibName()
{
	return DEF_LibName;
}
//------------------------------------------------------------------------------

const char *Atoll::GetLibVersion()
{
	return DEF_AtollVersion;
}
//------------------------------------------------------------------------------

const char *Atoll::GetIcuVersion()
{
	return U_ICU_VERSION;
}
//------------------------------------------------------------------------------

const char *Atoll::GetDbVersion()
{
	int vMajor, vMinor, vPatch;

	const char *str = DbEnv::version(&vMajor, &vMinor, &vPatch);

	return str;
}
//------------------------------------------------------------------------------

const char *Atoll::GetXercesVersion()
{
	return XERCES_FULLVERSIONDOT;
}
//------------------------------------------------------------------------------

const char *Atoll::GetXalanVersion()
{
	return XALAN_FULLVERSIONDOT;
}
//------------------------------------------------------------------------------

void Atoll::CheckDbVersion()
{
	int vMajor, vMinor, vPatch;

	DbEnv::version(&vMajor, &vMinor, &vPatch);
	if (vMajor != DB_VERSION_MAJOR || vMinor != DB_VERSION_MINOR) {
		char mess[255];
		sprintf(mess,
			"Database version %d.%d doesn't match library version %d.%d\n",
			DB_VERSION_MAJOR, DB_VERSION_MINOR, vMajor, vMinor);
		DEF_Exception(AppException::BerkeleyDbError, mess);
	}
}
//------------------------------------------------------------------------------

void Atoll::CheckZlibVersion()
{
	static const char* myVersion = ZLIB_VERSION;

	if (zlibVersion()[0] != myVersion[0]) {
		DEF_Exception(AppException::AssertionError, "Incompatible zlib version");
	}

	if (strcmp(zlibVersion(), ZLIB_VERSION) != 0) {
		DEF_Exception(AppException::AssertionError, "Different zlib version");
	}
}
//------------------------------------------------------------------------------

Common::Logger &Atoll::GetLogger()
{
	return gLog;
}
//------------------------------------------------------------------------------

const char *Atoll::GetLogFileName()
{
	return gLog.getOutputStream2FileName();
}
//------------------------------------------------------------------------------

const char *Atoll::GetTraceFileName()
{
	return Common::GetTraceFileName();
}
//------------------------------------------------------------------------------

bool Atoll::CreateCollator()
{
	if (gCollator != NULL) {
		gLog.log(eTypLogError, "Err > Multiple create collator instance");
		return false;
	}

	gLog.log(eTypLogDebug, "Deb > Collator create instance");

	if (!cWantCollator)
		return true;

	UErrorCode status = U_ZERO_ERROR;
	gCollator = Collator::createInstance(ULOC_FRENCH, status); // Locale("fr", "FR")
	if (!U_SUCCESS(status)) {
		gLog.log(eTypLogError, "Err > Collator create instance");
		return false;
	}

	uloc_setDefault(ULOC_FRENCH, &status);
	if (!U_SUCCESS(status)) {
		gLog.log(eTypLogError, "Err > Set default locale");
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------

void Atoll::DeleteCollator()
{
	if (!cWantCollator)
		return;

	if (gCollator == NULL) {
		//gLog.log(eTypLogError, "Err > Multiple delete collator instance");
		return;
	}

	gLog.log(eTypLogDebug, "Deb > Collator delete instance");

	delete gCollator;
	gCollator = NULL;
}
//------------------------------------------------------------------------------

bool Atoll::OpenLogFile(const std::string &inFileName)
{
	// Append logs or create a new log file ?
	bool wantAppend = true;
	bool isOldLog;

	// Quit if no log file is required
	if (inFileName.size() == 0)
		return false;

	// Check if log file already exist
	isOldLog = FileSystem::FileExists(inFileName.c_str());

#ifdef WIN32
	std::ios_base::open_mode mode;
#else
	std::_Ios_Openmode mode;
#endif
	mode  = wantAppend ? std::ios_base::app : std::ios_base::out;
	gLogStream.open(inFileName.c_str(), std::ios_base::binary | mode);
	if (gLogStream.fail()) {
		gLog.log(eTypLogError, "Err > Unable to open log file: %s", inFileName.c_str());
		return false;
	}

	if (wantAppend && isOldLog)
		gLogStream << "\n\n";
	gLogStream << "## =============================================================================\n";
	gLogStream << "## " << GetLibName() << " version " << GetLibVersion() << " - " << StdString::TimeToString() << "\n";
	gLogStream << "## =============================================================================\n";
	gLogStream << "\n";

	gLogStream.flush();

	gLog.setOutputStream2(gLogStream, inFileName.c_str());

	// Set trace file from log file
	std::string tracePath = FileSystem::GetPath(inFileName);
	SetTracePath(tracePath.c_str());
	std::string traceFileName = FileSystem::GetFileNameWithoutExt(inFileName);
	traceFileName += "_trace.log";
	SetTraceFileName(traceFileName.c_str());
	if (!wantAppend)
		RazTrace();

	return true;
}
//------------------------------------------------------------------------------

void Atoll::CreatePluginKernel()
{
	if (gPluginKernel != NULL) {
		gLog.log(eTypLogError, "Err > Multiple create plugin kernel instance");
		return;
	}

	gLog.log(eTypLogDebug, "Deb > Atoll::CreatePluginKernel");

	// Create the global plugin kernel instance
	gPluginKernel = new AtollPlugin::PluginKernel(gLog);

	// Load the plugin libraries
	gPluginKernel->LoadPlugin("atoll_plugin_xmlexec");
	gPluginKernel->LoadPlugin("atoll_plugin_xslt");
}
//------------------------------------------------------------------------------

void Atoll::DeletePluginKernel()
{
	if (gPluginKernel == NULL) {
		//gLog.log(eTypLogError, "Err > Multiple delete plugin kernel instance");
		return;
	}

	gLog.log(eTypLogDebug, "Deb > Atoll::DeletePluginKernel");

	// Release the plugin libraries
	gPluginKernel->ReleasePlugins();

	// Delete the global plugin kernel instance
	delete gPluginKernel;
	gPluginKernel = NULL;
}
//------------------------------------------------------------------------------

std::unique_ptr<AtollPlugin::PluginMessage> Atoll::XmlExecBuffer(const UChar *inStr, int32_t inLength)
{
	std::unique_ptr<AtollPlugin::PluginMessage> pluginMessage(new AtollPlugin::PluginMessage());
	pluginMessage->mIsOk = false;

	try {
		// Display execution start
		gLog.log(eTypLogAction, "Act > Start xmlexec buffer");

		AtollPlugin::PluginServer &pluginServer = gPluginKernel->GetPluginServer();

		// Get the plugin
		AtollPlugin::PluginServer::PluginXml *pluginXml;
		pluginXml = pluginServer.GetPluginXmlExecBuffer();
		if (pluginXml == NULL) {
			std::string error("No xmlexec buffer plugin available");
			gLog.log(eTypLogError, "Err > %s", error.c_str());
			pluginMessage->mError = error.c_str();
			return pluginMessage;
		}

		pluginMessage = pluginXml->PluginRunXmlExecBuffer(inStr, inLength);
		if (!pluginMessage->mIsOk) {
			std::string error("Xmlexec buffer plugin parsing error");
			gLog.log(eTypLogError, "Err > %s", error.c_str());
			if (pluginMessage->mError.isEmpty())
				pluginMessage->mError = error.c_str();
		}

		// Display execution stop
		gLog.log(eTypLogAction, "Act > Stop xmlexec buffer");
	}
	catch (AppException &e) {
		std::string error("Application exception during xmlexec buffer plugin execution");
		gLog.log(eTypLogError, "Err > %s: %s", error.c_str(), e.what());
		pluginMessage->mError = error.c_str();
		//returnValue = e.getExceptionCode();
	}
	catch (std::exception &e) {
		std::string error("Unknown exception during xmlexec buffer plugin execution");
		gLog.log(eTypLogError, "Err > %s: %s", error.c_str(), e.what());
		pluginMessage->mError = error.c_str();
		//returnValue = -1;
	}

	// Trace the request if there is an error
	if (!pluginMessage->mIsOk) {
		std::string error(UnicodeString2String(pluginMessage->mError));
		UTrace(error.c_str(), inStr, inLength);
	}

	return pluginMessage;
}
//------------------------------------------------------------------------------

std::unique_ptr<AtollPlugin::PluginMessage> Atoll::XmlExecBuffer(const UnicodeString &inStr)
{
	const UChar *str = inStr.getBuffer();
	int32_t length = inStr.length();

	return XmlExecBuffer(str, length);
}
//------------------------------------------------------------------------------

std::unique_ptr<AtollPlugin::PluginMessage> Atoll::XmlExecFile(const std::string &inFileName)
{
	std::unique_ptr<AtollPlugin::PluginMessage> pluginMessage(new AtollPlugin::PluginMessage());
	pluginMessage->mIsOk = false;

	try {
		// Display execution start
		gLog.log(eTypLogAction, "Act > Start xmlexec file");

		AtollPlugin::PluginServer &pluginServer = gPluginKernel->GetPluginServer();

		// Get the plugin
		AtollPlugin::PluginServer::PluginXml *pluginXml;
		pluginXml = pluginServer.GetPluginXmlExecFile(inFileName);
		if (pluginXml == NULL) {
			std::string error("No xmlexec file plugin available");
			gLog.log(eTypLogError, "Err > %s", error.c_str());
			pluginMessage->mError = error.c_str();
			return pluginMessage;
		}

		pluginMessage = pluginXml->PluginRunXmlExecFile(inFileName);
		if (!pluginMessage->mIsOk) {
			std::string error("Xmlexec file plugin parsing error: ");
			error += inFileName;
			gLog.log(eTypLogError, "Err > %s", error.c_str());
			if (pluginMessage->mError.isEmpty())
				pluginMessage->mError = error.c_str();
		}

		// Display execution stop
		gLog.log(eTypLogAction, "Act > Stop xmlexec file");
	}
	catch (AppException &e) {
		std::string error("Application exception during xmlexec file plugin execution");
		gLog.log(eTypLogError, "Err > %s: %s", error.c_str(), e.what());
		pluginMessage->mError = error.c_str();
		//returnValue = e.getExceptionCode();
	}
	catch (std::exception &e) {
		std::string error("Unknown exception during xmlexec file plugin execution");
		gLog.log(eTypLogError, "Err > %s: %s", error.c_str(), e.what());
		pluginMessage->mError = error.c_str();
		//returnValue = -1;
	}

	// Trace the request if there is an error
	if (!pluginMessage->mIsOk) {
		std::string error(UnicodeString2String(pluginMessage->mError));
		Trace("%s --- %s", error.c_str(), inFileName.c_str());
	}

	return pluginMessage;
}
//------------------------------------------------------------------------------

std::unique_ptr<AtollPlugin::PluginMessage> Atoll::XsltBuffer(const UChar *inStr, int32_t inLength,
	const UChar *inXsl, int32_t inLengthXsl, const StringToUnicodeStringMap &inStylesheetParamMap)
{
	std::unique_ptr<AtollPlugin::PluginMessage> pluginMessage(new AtollPlugin::PluginMessage());
	pluginMessage->mIsOk = false;

	try {
		// Display execution start
		gLog.log(eTypLogAction, "Act > Start xslt buffer");

		AtollPlugin::PluginServer &pluginServer = gPluginKernel->GetPluginServer();

		// Get the plugin
		AtollPlugin::PluginServer::PluginXml *pluginXml;
		pluginXml = pluginServer.GetPluginXsltBuffer();
		if (pluginXml == NULL) {
			std::string error("No xslt buffer plugin available");
			gLog.log(eTypLogError, "Err > %s", error.c_str());
			pluginMessage->mError = error.c_str();
			return pluginMessage;
		}

		pluginMessage = pluginXml->PluginRunXsltBuffer(inStr, inLength, inXsl, inLengthXsl, inStylesheetParamMap);
		if (!pluginMessage->mIsOk) {
			std::string error("Xslt buffer plugin parsing error");
			gLog.log(eTypLogError, "Err > %s", error.c_str());
			if (pluginMessage->mError.isEmpty())
				pluginMessage->mError = error.c_str();
		}

		// Display execution stop
		gLog.log(eTypLogAction, "Act > Stop xslt buffer");
	}
	catch (AppException &e) {
		std::string error("Application exception during xslt buffer plugin execution");
		gLog.log(eTypLogError, "Err > %s: %s", error.c_str(), e.what());
		pluginMessage->mError = error.c_str();
		//returnValue = e.getExceptionCode();
	}
	catch (std::exception &e) {
		std::string error("Unknown exception during xslt buffer plugin execution");
		gLog.log(eTypLogError, "Err > %s: %s", error.c_str(), e.what());
		pluginMessage->mError = error.c_str();
		//returnValue = -1;
	}

	// Trace the request if there is an error
	if (!pluginMessage->mIsOk) {
		std::string error(UnicodeString2String(pluginMessage->mError));
		UTrace(error.c_str(), inStr, inLength);
	}

	return pluginMessage;
}
//------------------------------------------------------------------------------

std::unique_ptr<AtollPlugin::PluginMessage> Atoll::XsltBuffer(const UnicodeString &inStr,
	const UnicodeString &inXsl, const StringToUnicodeStringMap &inStylesheetParamMap)
{
	const UChar *str = inStr.getBuffer();
	int32_t length = inStr.length();

	return XsltBuffer(str, length, inXsl.getBuffer(), inXsl.length(), inStylesheetParamMap);
}
//------------------------------------------------------------------------------

std::unique_ptr<AtollPlugin::PluginMessage> Atoll::XsltCompiled(const UnicodeString &inStr,
	const UnicodeString &inXsltName, const StringToUnicodeStringMap &inStylesheetParamMap)
{
	std::unique_ptr<AtollPlugin::PluginMessage> pluginMessage(new AtollPlugin::PluginMessage());
	pluginMessage->mIsOk = false;

	try {
		// Display execution start
		gLog.log(eTypLogAction, "Act > Start xslt buffer");

		AtollPlugin::PluginServer &pluginServer = gPluginKernel->GetPluginServer();

		// Get the plugin
		AtollPlugin::PluginServer::PluginXml *pluginXml;
		pluginXml = pluginServer.GetPluginXsltBuffer();
		if (pluginXml == NULL) {
			std::string error("No xslt buffer plugin available");
			gLog.log(eTypLogError, "Err > %s", error.c_str());
			pluginMessage->mError = error.c_str();
			return pluginMessage;
		}

		const UChar *str = inStr.getBuffer();
		int32_t length = inStr.length();
		pluginMessage = pluginXml->PluginRunXsltCompiled(str, length, inXsltName, inStylesheetParamMap);
		if (!pluginMessage->mIsOk) {
			std::string error("Xslt buffer plugin parsing error");
			gLog.log(eTypLogError, "Err > %s", error.c_str());
			if (pluginMessage->mError.isEmpty())
				pluginMessage->mError = error.c_str();
		}

		// Display execution stop
		gLog.log(eTypLogAction, "Act > Stop xslt buffer");
	}
	catch (AppException &e) {
		std::string error("Application exception during xslt buffer plugin execution");
		gLog.log(eTypLogError, "Err > %s: %s", error.c_str(), e.what());
		pluginMessage->mError = error.c_str();
		//returnValue = e.getExceptionCode();
	}
	catch (std::exception &e) {
		std::string error("Unknown exception during xslt buffer plugin execution");
		gLog.log(eTypLogError, "Err > %s: %s", error.c_str(), e.what());
		pluginMessage->mError = error.c_str();
		//returnValue = -1;
	}

	// Trace the request if there is an error
	if (!pluginMessage->mIsOk) {
		std::string error(UnicodeString2String(pluginMessage->mError));
		UTrace(error.c_str(), inStr.getBuffer(), inStr.length());
	}

	return pluginMessage;
}
//------------------------------------------------------------------------------

bool Atoll::EngineApiGetColMetaVector(const EngineEnv &inEngineEnv, ColMetaVector &outVector)
{
	bool isOk;

	// Create a collection manager
	ColManager *colManager = new ColManager(inEngineEnv.GetDbEnv());

	// Get the vector of all collection metadata records
	isOk = colManager->GetColMetaVector(outVector);

	delete colManager;

	return isOk;
}
//------------------------------------------------------------------------------

bool Atoll::EngineApiAddCollection(UnicodeString &outColId, const EngineEnv &inEngineEnv, const ColMeta &inColMeta)
{
	bool isOk;

	// Create a collection manager
	ColManager *colManager = new ColManager(inEngineEnv.GetDbEnv());

	// Add the collection
	isOk = colManager->AddCollection(outColId, inColMeta);

	delete colManager;

	return isOk;
}
//------------------------------------------------------------------------------

bool Atoll::EngineApiGetCollection(const EngineEnv &inEngineEnv, ColMeta &ioColMeta)
{
	bool isOk;

	// Create a collection manager
	ColManager *colManager = new ColManager(inEngineEnv.GetDbEnv());

	// Get the collection
	isOk = colManager->GetCollectionMeta(ioColMeta);

	delete colManager;

	return isOk;
}
//------------------------------------------------------------------------------

bool Atoll::EngineApiSetCollection(const EngineEnv &inEngineEnv, const ColMeta &inColMeta)
{
	bool isOk;

	// Create a collection manager
	ColManager *colManager = new ColManager(inEngineEnv.GetDbEnv());

	// Update the collection
	isOk = colManager->SetCollectionMeta(inColMeta);

	delete colManager;

	return isOk;
}
//------------------------------------------------------------------------------

bool Atoll::EngineApiDelCollection(const EngineEnv &inEngineEnv, const UnicodeString &inColId)
{
	bool isOk;

	// Create a collection manager
	ColManager *colManager = new ColManager(inEngineEnv.GetDbEnv());

	// Remove the collection
	isOk = colManager->RemoveCollection(inColId);

	delete colManager;

	return isOk;
}
//------------------------------------------------------------------------------

bool Atoll::EngineApiGetDocMetaVector(const EngineEnv &inEngineEnv, DocMetaVector &outVector)
{
	bool isOk;

	// Create a collection manager
	ColManager *colManager = new ColManager(inEngineEnv.GetDbEnv());

	// Open the collection
	Collection *collection = colManager->OpenCollection(inEngineEnv.GetColId());
	if (!collection) {
		delete colManager;
		return false;
	}

	// Get the vector of all documents metadata records for this collection
	isOk = collection->GetDocMetaVector(outVector);

	delete colManager;

	return isOk;
}
//------------------------------------------------------------------------------

bool Atoll::EngineApiAddDocument(unsigned int &outDocNum, const EngineEnv &inEngineEnv,
																 const DocMeta &inDocMeta, bool inWantIndex)
{
	bool isOk;

	// Create a collection manager
	ColManager *colManager = new ColManager(inEngineEnv.GetDbEnv());

	// Open the collection
	Collection *collection = colManager->OpenCollection(inEngineEnv.GetColId());
	if (!collection) {
		delete colManager;
		return false;
	}

	// Add the document
	isOk = collection->AddDocument(outDocNum, inDocMeta, inWantIndex);

	delete colManager;

	return isOk;
}
//------------------------------------------------------------------------------

DEF_Export unsigned int Atoll::EngineApiGetDocumentIdByUuid(const EngineEnv &inEngineEnv, const UnicodeString &inUuid)
{
	unsigned int docNum = 0;

	DocMeta docMeta;
	docMeta.mUuid = inUuid;
	// Get the document number from uuid
	if (EngineApiGetDocument(inEngineEnv, docMeta))
		docNum = docMeta.mDocNum;

	return docNum;
}
//------------------------------------------------------------------------------

bool Atoll::EngineApiGetDocument(const EngineEnv &inEngineEnv, DocMeta &ioDocMeta)
{
	bool isOk;

	// Create a collection manager
	ColManager *colManager = new ColManager(inEngineEnv.GetDbEnv());

	// Open the collection
	Collection *collection = colManager->OpenCollection(inEngineEnv.GetColId());
	if (!collection) {
		delete colManager;
		return false;
	}

	// Get the document
	isOk = collection->GetDocumentMeta(ioDocMeta);

	delete colManager;

	return isOk;
}
//------------------------------------------------------------------------------

bool Atoll::EngineApiSetDocument(const EngineEnv &inEngineEnv, const DocMeta &inDocMeta)
{
	bool isOk;

	// Create a collection manager
	ColManager *colManager = new ColManager(inEngineEnv.GetDbEnv());

	// Open the collection
	Collection *collection = colManager->OpenCollection(inEngineEnv.GetColId());
	if (!collection) {
		delete colManager;
		return false;
	}

	// Set the document
	isOk = collection->SetDocumentMeta(inDocMeta);

	delete colManager;

	return isOk;
}
//------------------------------------------------------------------------------

bool Atoll::EngineApiDelDocument(const EngineEnv &inEngineEnv, unsigned int inDocNum)
{
	bool isOk;

	// Create a collection manager
	ColManager *colManager = new ColManager(inEngineEnv.GetDbEnv());

	// Open the collection
	Collection *collection = colManager->OpenCollection(inEngineEnv.GetColId());
	if (!collection) {
		delete colManager;
		return false;
	}

	// Remove the document
	isOk = collection->RemoveDocument(inDocNum);

	delete colManager;

	return isOk;
}
//------------------------------------------------------------------------------

bool Atoll::EngineApiIndexDocument(const EngineEnv &inEngineEnv, unsigned int inDocNum)
{
	bool isOk;
	Timing timer;

	gLog.log(eTypLogAction, "Act > Start document index %u", inDocNum);

	timer.restartTime();

	// Create a collection manager
	ColManager *colManager = new ColManager(inEngineEnv.GetDbEnv());

	// Open the collection
	Collection *collection = colManager->OpenCollection(inEngineEnv.GetColId());
	if (!collection) {
		delete colManager;
		return false;
	}

	// Index the document
	isOk = collection->IndexDocument(inDocNum);

	delete colManager;

	gLog.log(eTypLogAction, "Act > Stop document index %u: %2.2f ms", inDocNum, timer.getElapsedTime());

	return isOk;
}
//------------------------------------------------------------------------------

bool Atoll::EngineApiDelDocumentIndex(const EngineEnv &inEngineEnv, unsigned int inDocNum)
{
	bool isOk;

	gLog.log(eTypLogAction, "Act > Start document index remove %u", inDocNum);

	// Create a collection manager
	ColManager *colManager = new ColManager(inEngineEnv.GetDbEnv());

	// Open the collection
	Collection *collection = colManager->OpenCollection(inEngineEnv.GetColId());
	if (!collection) {
		delete colManager;
		return false;
	}

	// Remove the document index
	isOk = collection->RemoveDocumentIndex(inDocNum);

	delete colManager;

	gLog.log(eTypLogAction, "Act > Stop document index remove %u", inDocNum);

	return isOk;
}
//------------------------------------------------------------------------------

bool Atoll::EngineApiGetDocumentContent(const EngineEnv &inEngineEnv, UnicodeString &outContent,
																				const Entry &inEntry, const IntVector &inHilightVector)
{
	bool isOk;

	outContent.remove();

	// Create a collection manager
	ColManager *colManager = new ColManager(inEngineEnv.GetDbEnv());

	// Open the collection
	Collection *collection = colManager->OpenCollection(inEngineEnv.GetColId());
	if (!collection) {
		delete colManager;
		return false;
	}

	DocMeta docMeta(inEntry.mUiEntDoc);
	isOk = collection->GetDocumentMeta(docMeta);
	if (!isOk) {
		gLog.log(eTypLogError, "Err > XmlReqDoc: Incorrect document - %u", inEntry.mUiEntDoc);
		delete colManager;
		return false;
	}
	/*
	if (inEntry.mUlEntPge => docMeta.mCountPge) {
		gLog.log(eTypLogError, "Err > XmlReqDoc: Incorrect document page - %u-%lu", inEntry.mUiEntDoc, inEntry.mUlEntPge);
		delete colManager;
		return false;
	}
	*/

	// Get the document database
	DbDoc *dbDoc = collection->GetDocumentDatabase(inEntry.mUiEntDoc);
	if (!dbDoc) {
		delete colManager;
		return false;
	}

	UnicodeString key;
	DocRecord dr;
	dr.mPge = inEntry.mUlEntPge;
	dr.mPos = inEntry.mUlEntPos; // Unused
	dr.GetPageKey(key);
	isOk = dbDoc->GetDocRecord(key, dr);
	if (!isOk) {
		std::string str = UnicodeString2String(key);
		gLog.log(eTypLogError, "Err > XmlReqDoc: Incorrect document record key - %s", str.c_str());
		delete colManager;
		return false;
	}

	// Set the stylesheet parameters with hilight positions
	// Example: "atoll.highwords", " 1 2 3 4 "
	StringToUnicodeStringMap stylesheetParamMap;
	if (inHilightVector.size()) {
		unsigned int pos;
		UnicodeString paramValue(" "), strPos;
		IntVector::const_iterator it = inHilightVector.begin();
		IntVector::const_iterator itEnd = inHilightVector.end();
		for (; it != itEnd; ++it) {
			pos = *it;
			UnicodeStringSetInt(strPos, pos);
			paramValue += strPos;
			paramValue += " ";
		}
		std::string paramKey(DEF_XsltParamHighwords);
		stylesheetParamMap[paramKey] = paramValue;
		//UTrace(paramKey.c_str(), paramValue.getTerminatedBuffer());
	}

	// Get the xslt name
	UnicodeString xsltName;
	if (docMeta.mXsltCode.isEmpty()) {
		if (inHilightVector.size())
			xsltName = "highwords"; // Default xslt: display highwords and clear word positions
		else
			xsltName = "identity"; // Default xslt: clear word positions
	}
	else
		xsltName = docMeta.mXsltCode;

	// Do the transformation if necessary
	if (xsltName.isEmpty()) {
		outContent = dr.mTexte;
	}
	else {
		// Xslt plugin execution
		std::unique_ptr<AtollPlugin::PluginMessage> pluginMessage;
		pluginMessage = XsltCompiled(dr.mTexte, xsltName, stylesheetParamMap);
		isOk = pluginMessage->mIsOk;
		if (isOk) {
			// If the result is html, get the content of the body part only
			UnicodeString begin("<body");
			const UChar *p1 = pluginMessage->mMessage.getTerminatedBuffer();
			if ((p1 = u_strstr(p1, begin.getTerminatedBuffer())) != NULL && (p1 = u_strchr(p1, '>')) != NULL) {
				UChar *p2;
				UnicodeString end("</body>");
				if ((p2 = u_strstr(++p1, end.getTerminatedBuffer())) != NULL)
					*p2 = 0;
				outContent = p1;
			}
			else
				outContent = pluginMessage->mMessage;
		}
	}

	delete colManager;

	return isOk;
}
//------------------------------------------------------------------------------

bool Atoll::EngineApiGetFieldMetaVector(const EngineEnv &inEngineEnv, FieldMetaVector &outVector, eTypField inTypField)
{
	bool isOk;

	// Create a collection manager
	ColManager *colManager = new ColManager(inEngineEnv.GetDbEnv());

	// Open the collection
	Collection *collection = colManager->OpenCollection(inEngineEnv.GetColId());
	if (!collection) {
		delete colManager;
		return false;
	}

	// Get the vector of all field metadata records for this collection
	isOk = collection->GetFieldMetaVector(outVector, inTypField);

	delete colManager;

	return isOk;
}
//------------------------------------------------------------------------------

bool Atoll::EngineApiAddField(const EngineEnv &inEngineEnv, const FieldMeta &inFieldMeta)
{
	bool isOk;

	// Create a collection manager
	ColManager *colManager = new ColManager(inEngineEnv.GetDbEnv());

	// Open the collection
	Collection *collection = colManager->OpenCollection(inEngineEnv.GetColId());
	if (!collection) {
		delete colManager;
		return false;
	}

	// Add the field
	isOk = collection->AddFieldMeta(inFieldMeta);

	delete colManager;

	return isOk;
}
//------------------------------------------------------------------------------

bool Atoll::EngineApiGetField(const EngineEnv &inEngineEnv, FieldMeta &ioFieldMeta)
{
	bool isOk;

	// Create a collection manager
	ColManager *colManager = new ColManager(inEngineEnv.GetDbEnv());

	// Open the collection
	Collection *collection = colManager->OpenCollection(inEngineEnv.GetColId());
	if (!collection) {
		delete colManager;
		return false;
	}

	// Get the field
	isOk = collection->GetFieldMeta(ioFieldMeta);

	delete colManager;

	return isOk;
}
//------------------------------------------------------------------------------

bool Atoll::EngineApiSetField(const EngineEnv &inEngineEnv, const FieldMeta &inFieldMeta)
{
	bool isOk;

	// Create a collection manager
	ColManager *colManager = new ColManager(inEngineEnv.GetDbEnv());

	// Open the collection
	Collection *collection = colManager->OpenCollection(inEngineEnv.GetColId());
	if (!collection) {
		delete colManager;
		return false;
	}

	// Set the field
	isOk = collection->SetFieldMeta(inFieldMeta);

	delete colManager;

	return isOk;
}
//------------------------------------------------------------------------------

bool Atoll::EngineApiDelField(const EngineEnv &inEngineEnv, const FieldMeta &inFieldMeta)
{
	bool isOk;

	// Create a collection manager
	ColManager *colManager = new ColManager(inEngineEnv.GetDbEnv());

	// Open the collection
	Collection *collection = colManager->OpenCollection(inEngineEnv.GetColId());
	if (!collection) {
		delete colManager;
		return false;
	}

	// Remove the field
	isOk = collection->RemoveFieldMeta(inFieldMeta);

	delete colManager;

	return isOk;
}
//------------------------------------------------------------------------------

bool Atoll::EngineApiComputeCrit(const EngineEnv &inEngineEnv, SearchRecord &ioSearchRecord)
{
	bool isOk = true;
	bool wantTrace = false;

	// Clear the search result set
	ioSearchRecord.mEntrySet.clear();

	if (ioSearchRecord.mSearchCritVector.size() == 0 && ioSearchRecord.mSearchId.empty()) {
		gLog.log(eTypLogError, "Err > EngineApi: Compute empty search record");
		return false;
	}

	// Create a collection manager
	ColManager *colManager = new ColManager(inEngineEnv.GetDbEnv());

	// Open the collection
	Collection *collection = colManager->OpenCollection(inEngineEnv.GetColId());
	if (!collection) {
		delete colManager;
		return false;
	}

	ioSearchRecord.mTypSearchRecord = eTypSearchRecordNormal;
	if (ioSearchRecord.mSearchCritVector.size() == 0) {
		// Load the search record in database
		if (!collection->GetSearchRecord(ioSearchRecord)) {
			gLog.log(eTypLogError, "Err > EngineApi: Incorrect search id");
			delete colManager;
			return false;
		}
		delete colManager;
		// Return success
		return isOk;
	}

	// Compute search
	//size_t size;
	bool isDocField, isLowerField;
	const UChar *buf;
	unsigned int len;
	std::vector<EntrySet *> docList, posList, *destList;
	EntrySet *tmpEntrySet;
	const EntrySet *entrySet;
	UnicodeString wrd, field, strDocMetaField("metadata_"), strDocMetaDescriptionField("metadata_dc_description"), strTextField("text");
	Searcher *searcher = new Searcher(collection);
	EntrySet::const_iterator itEntry, itEntryEnd;
	SearchCritVector::const_iterator it = ioSearchRecord.mSearchCritVector.begin();
	SearchCritVector::const_iterator itEnd = ioSearchRecord.mSearchCritVector.end();
	for (; it != itEnd; ++it) {
		const SearchCrit &searchCrit = (*it);
		field = searchCrit.GetIdCrit();
		isDocField = field.startsWith(strDocMetaField);
		isLowerField = (field == strTextField || field == strDocMetaDescriptionField);
		//destEntrySet = isDocField ? &docEntrySet : &ioSearchRecord.mEntrySet;
		destList = isDocField ? &docList : &posList;
		// Lower case the word to search
		wrd = searchCrit.mWrd;
		if (isLowerField) {
			wrd.toLower();
		}
		len = wrd.length(); // searchCrit.mWrd.length();
		buf = wrd.getBuffer();
		searcher->SetQuery(field.getTerminatedBuffer(), buf, len);
		searcher->Compute();
		entrySet = searcher->GetEntrySet();
		// Transfert the entry set in the right set
		tmpEntrySet = new EntrySet;
		itEntry = entrySet->begin();
		itEntryEnd = entrySet->end();
		for (; itEntry != itEntryEnd; ++itEntry) {
			tmpEntrySet->insert(*itEntry);
		}
		destList->push_back(tmpEntrySet);
		//size = tmpEntrySet->size();
		/*
		size = entrySet->size();
		if (size) {
			itEntry = entrySet->begin();
			itEntryEnd = entrySet->end();
			for (; itEntry != itEntryEnd; ++itEntry) {
				destEntrySet->insert(*itEntry);
			}
			// Form union in place
			//EntrySet tempSet;
			//std::set_union(ioSearchRecord.mEntrySet.begin(), ioSearchRecord.mEntrySet.end(),
			//							 entrySet->begin(), entrySet->end(),
			//							 std::inserter(tempSet, tempSet.begin()));
			//ioSearchRecord.mEntrySet.swap(tempSet); // tempSet will be deleted
		}
		*/
		// Trace
		if (wantTrace) {
			std::string strCrit(searchCrit.ToString());
			std::string strEntry(EntrySet2String(*tmpEntrySet));
			Trace("Atoll::EngineApi ComputeCrit SearchCrit - %s - %s", strCrit.c_str(), strEntry.c_str());
		}
	}
	delete searcher;

	// Filter the position set with the document set
	EntrySet *docEntrySet = NULL, *posEntrySet = NULL;
	if (docList.size()) {
		docEntrySet = QueryResolver::ReduceAndList(docList);
	}
	if (posList.size()) {
		posEntrySet = QueryResolver::ReduceAndList(posList);
	}
	// If there is doc and pos in the search: filter the position set with the document set
	if (docEntrySet && posEntrySet) {
		QueryResolver::FilterDocSetIntersection(posEntrySet, docEntrySet);
		ioSearchRecord.mEntrySet = *posEntrySet;
	}
	else if (docEntrySet) {
		ioSearchRecord.mEntrySet = *docEntrySet;
	}
	else if (posEntrySet) {
		ioSearchRecord.mEntrySet = *posEntrySet;
	}
	// Cleanup
	if (docEntrySet)
		delete docEntrySet;
	if (posEntrySet)
		delete posEntrySet;
	/*if (docEntrySet.size()) {
		if (ioSearchRecord.mEntrySet.size())
			QueryResolver::FilterDocSetIntersection(&ioSearchRecord.mEntrySet, &docEntrySet);
		else
			ioSearchRecord.mEntrySet = docEntrySet;
	}*/

	// Store the search record in database
	if (!collection->SetSearchRecord(ioSearchRecord)) {
		gLog.log(eTypLogError, "Err > EngineApi: Store search record");
		isOk = false;
	}

	// Trace
	if (wantTrace) {
		std::string strCritVector;
		SearchCritVectorToString(strCritVector, ioSearchRecord.mSearchCritVector);
		std::string strEntry(EntrySet2String(ioSearchRecord.mEntrySet));
		Trace("Atoll::EngineApi ComputeCrit SearchCritVector - %s - %s", strCritVector.c_str(), strEntry.c_str());
	}

	delete colManager;

	return isOk;
}
//------------------------------------------------------------------------------

void Atoll::EngineApiGetSearchResult(EntrySet &outEntrySet, const EntrySet &inEntrySet, unsigned long inMin, unsigned long &ioNb, unsigned long &outCount, bool inDocOnly, bool inDocPageOnly)
{
	outCount = 0;
	outEntrySet.clear();

	// Consecutives entries must be counted as one search result only
	Entry lastEntry;
	//bool wantAllEntries = false;
	//unsigned long pos;
	unsigned long nb = 0;
	EntrySet::const_iterator it, itEnd;
	it = inEntrySet.begin();
	itEnd = inEntrySet.end();
	while (it != itEnd) {
		const Entry &e = *it;
		if (inDocOnly) {
			if (lastEntry.mUiEntDoc == e.mUiEntDoc) {
				it++;
				continue;
			}
			lastEntry.mUiEntDoc = e.mUiEntDoc;
		}
		else if (inDocPageOnly) {
			if (lastEntry.mUiEntDoc == e.mUiEntDoc && lastEntry.mUlEntPge == e.mUlEntPge) {
				it++;
				continue;
			}
			lastEntry.mUiEntDoc = e.mUiEntDoc;
			lastEntry.mUlEntPge = e.mUlEntPge;
		}
		else {
			if (lastEntry.mUiEntDoc == e.mUiEntDoc && lastEntry.mUlEntPos + 1 == e.mUlEntPos) {
				it++;
				continue;
			}
			lastEntry.mUiEntDoc = e.mUiEntDoc;
			lastEntry.mUlEntPos = e.mUlEntPos;
		}
		if (outCount >= inMin && outCount < inMin + ioNb) {
			outEntrySet.insert(e);
			nb++;
		}
		outCount++; // Total number of search results
		it++;
	}
	/*
	while (it != itEnd) {
		const Entry &e = *it;
		if (outCount >= inMin && outCount < inMin + ioNb) {
			outEntrySet.insert(e);
			nb++;
		}
		// Add the consecutives entries
		pos = e.mUlEntPos;
		while (true) {
			it++;
			if (it == itEnd)
				break;
			const Entry &eNext = *it;
			if (eNext.mUiEntDoc != e.mUiEntDoc || eNext.mUlEntPos != ++pos)
				break;
			if (wantAllEntries && outCount >= inMin && outCount < inMin + ioNb)
				outEntrySet.insert(eNext);
		}
		outCount++; // Total number of search results
	}
	*/
	ioNb = nb;

	//ioNb = outEntrySet.size();
	if (inMin + ioNb > outCount)
		gLog.log(eTypLogError, "Incorrect search result size: Min=%lu Nb=%lu Max=%lu", inMin, ioNb, outCount);

	// Set the search result size
	//outCount = searchRecord.mEntrySet.size();
	// Set nb according to max
	//if (inMin + ioNb > outCount)
	//	ioNb = outCount - inMin;
	//if (ioNb != outEntrySet.size()) {
	//	gLog.log(eTypLogError, "Incorrect search result size: Min=%lu Nb=%lu Max=%lu", inMin, ioNb, outCount);
	//}

}
//------------------------------------------------------------------------------

bool Atoll::EngineApiComputeIndex(const EngineEnv &inEngineEnv, WordIntVector &outWordIntVector,
																	const SearchRecord &inSearchRecord,
																	const UnicodeString &inNdxName,
																	unsigned long &ioMin, const UnicodeString &inWord,
																	unsigned long &ioNb, unsigned long &outCount)
{
	bool isOk = true;

	outWordIntVector.clear();

	// Create a collection manager
	ColManager *colManager = new ColManager(inEngineEnv.GetDbEnv());

	// Open the collection
	Collection *collection = colManager->OpenCollection(inEngineEnv.GetColId());
	if (!collection) {
		delete colManager;
		return false;
	}

	// Get the right index
	std::string dbName;
	ConvertUnicodeString2String(dbName, inNdxName);
	DbNdx *dbNdx = collection->GetIndexDatabase(dbName); // Index name example: text
	if (!dbNdx) {
		delete colManager;
		return false;
	}

	// If a word is given, get it's position
	if (!inWord.isEmpty()) {
		UnicodeString word = inWord;
		word.toLower();
		ioMin = dbNdx->GetWordRecNum(word);
		// Clear min if error
		if (ioMin == DEF_LongMax)
			ioMin = 0;
	}

	// Get the index content (words and occurrences)
	isOk = dbNdx->GetWordIntList(outWordIntVector, ioMin, ioNb);

	// Set the index size
	outCount = dbNdx->Count();
	// Reduce min to max
	if (ioMin >= outCount) {
		ioMin = outCount - 1;
	}
	// Set nb according to max
	if (ioMin + ioNb > outCount)
		ioNb = outCount - ioMin;
	if (ioNb != outWordIntVector.size()) {
		gLog.log(eTypLogError, "Incorrect index size: Min=%lu Nb=%lu Max=%lu", ioMin, ioNb, outCount);
	}

	delete colManager;

	return isOk;
}
//------------------------------------------------------------------------------

bool Atoll::EngineApiGetIndexMap(const EngineEnv &inEngineEnv, UnicodeStringToIntMap &outMap)
{
	bool isOk;

	// Create a collection manager
	ColManager *colManager = new ColManager(inEngineEnv.GetDbEnv());

	// Open the collection
	Collection *collection = colManager->OpenCollection(inEngineEnv.GetColId());
	if (!collection) {
		delete colManager;
		return false;
	}

	// Get the map of all index for this collection
	isOk = collection->GetIndexMap(outMap);

	delete colManager;

	return isOk;
}
//------------------------------------------------------------------------------
