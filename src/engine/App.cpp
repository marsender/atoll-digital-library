/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

App.cpp

Project settings | Debug
	Command:
		$(OutDir)\atoll_console.exe
	Program arguments:
		UnitTest:
			-v -j test -l ./data_unittest/_atoll.log
		Xml plugin debug execution:
			-vv -l ./data_unittest/_atoll.log -x ./data_unittest/xmlexec_req.xml
			-v -l ./data_unittest/_atoll.log -x ./data_unittest/xmlexec_test.xml
		Xml plugin release execution:
		  sudo rm /opt/trace/atoll_res.xml /opt/dev/atoll/data_xmlexec/_atoll_trace.log
			sudo rm -f /opt/atoll/data_db/demo/__db.* /opt/atoll/data_db/demo/db*
			-vv -l ./data_xmlexec/_atoll.log -x ./data_xmlexec/ReqCreateDb_demo.xml
			-vv -l ./data_xmlexec/_atoll.log -x ./data_xmlexec/ReqAdmin.xml
		Adorner:
			-v -j adorner -l ./data/_atoll.log -h ./data_db/ -s ./data/ -d ./data_dst/
		Indexer:
			-v -j indexer -l ./data/_atoll.log -h ./data_db/ -s ./data/ -d ./data_dst/
	Working directory:
		$(ATOLLROOT)

Project settings | post-build step
	$(SolutionDir)\make UnitTest $(ConfigurationName)

ICU config:
	Environment:
		set ICUROOT=C:\Util\xml\icu
	Include directory:
		/I "$(ICUROOT)/include"
	Linked libraries:
		icuuc.lib icuin.lib icuio.lib (icudt.lib not used yet)
	Lib path:
		/libpath:"$(ICUROOT)/lib"

Berkeley DB config:
	Environment:
		set BERKELEYDBROOT=C:\Work\Prog\xml\berkeleydb
	Include directory:
		/I "$(BERKELEYDBROOT)/build_windows"
	Linked libraries:
		libdb47d.lib
	Lib path release:
		/libpath:"$(BERKELEYDBROOT)/build_windows/Release"
	Lib path debug:
		/libpath:"$(BERKELEYDBROOT)/build_windows/Debug"

db_stat utility:
	http://www.oracle.com/technology/documentation/berkeley-db/db/utility/db_stat.html
	Display Berkeley DB database information about the database environment,
	including all configured subsystems of the database environment:
		db_stat -h %ATOLLROOT%/data_db -e
	Display Berkeley DB database cache statistics:
		db_stat -h %ATOLLROOT%/data_db -m

Sample directories:
	%BERKELEYDBROOT%\examples_c
	%BERKELEYDBROOT%\examples_cxx

*******************************************************************************/

#include "../Portability.hpp"
#include "App.hpp"
#include "EngineApi.hpp"
#include "EngineEnv.hpp"
#include "InitStringDecl.hpp"
//#include "../collection/Collection.hpp"
#include "../common/Containers.hpp"
#include "../common/UnicodeUtil.hpp"
#include "../common/UnitTest.hpp"
#include "../database/DbManager.hpp"
//#include "../database/DbUtil.hpp"
#include "../parser/AdornerHandler.hpp"
#include "../parser/IndexerHandler.hpp"
#include "../parser/Parser.hpp"
//#include "../plugin/PluginKernel.hpp"
#include "../plugin/PluginMessage.hpp"
#include <db_cxx.h>
#include <fstream>
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <shellapi.h>  // For ShellExecute
#endif 
#ifdef _WIN32
	#include "vld.h" // Visual Leak Detector (Memory leak detection)
#endif 
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;
//------------------------------------------------------------------------------

App::App()
{
	mIsValid = true;
}
//------------------------------------------------------------------------------

App::~App()
{
	CloseApp();

	// Display log info
	unsigned int nbLog = gLog.getNbLog();
	unsigned int nbLogError = gLog.getNbLogError();
	unsigned int nbLogWarning = gLog.getNbLogWarning();
	if (nbLogError)
		gLog.log(eTypLogAction, "Act > Total: %u error(s)", nbLogError);
	if (nbLogWarning)
		gLog.log(eTypLogAction, "Act > Total: %u warning(s)", nbLogWarning);
	if (nbLog) {
		if (!mLogFileName.empty()) {
			gLog.log(eTypLogAction, "Act > *** See trace file: %s ***", mLogFileName.c_str());
#if defined(WIN32) && !defined(_DEBUG)
			// Open the trace file
			std::string fileName = FileSystem::SetPlatformDirectorySeparator(mLogFileName);
			HINSTANCE hinst = ShellExecuteA(NULL, "open", fileName.c_str(), NULL, NULL, SW_SHOWNORMAL);
#endif
		}
#ifdef WIN32
		std::cout << "Press any key to continue...";
		getc(stdin);
#endif
	}
}
//------------------------------------------------------------------------------

std::string App::GetEngineVersion()
{
	const char *version = GetFullVersion();

	return version;
}
//------------------------------------------------------------------------------

Common::Logger &App::GetEngineLogger()
{
	return gLog;
}
//------------------------------------------------------------------------------

void App::SetAppLogFile(const std::string &inFileName)
{
	// Quit if no log file is required
	if (inFileName.size() == 0)
		return;

	if (!OpenLogFile(inFileName))
		mIsValid = false;
}
//------------------------------------------------------------------------------

void App::SetAppLogVerbose(int inVerbose)
{
	gLog.setVerbose(inVerbose);
}
//------------------------------------------------------------------------------

void App::SetAppSrcDir(const std::string &inPath)
{
	if (!FileSystem::DirectoryExists(inPath)) {
		mIsValid = false;
		gLog.log(eTypLogError, "Err > Incorrect application source directory: %s", inPath.c_str());
		return;
	}

	mAppSrcDir = FileSystem::SetDirectorySeparator(inPath);
}
//------------------------------------------------------------------------------

void App::SetAppDstDir(const std::string &inPath)
{
	if (!FileSystem::DirectoryExists(inPath)) {
		mIsValid = false;
		gLog.log(eTypLogError, "Err > Incorrect application destination directory: %s", inPath.c_str());
		return;
	}

	mAppDstDir = FileSystem::SetDirectorySeparator(inPath);
}
//------------------------------------------------------------------------------

void App::SetAppDbDir(const std::string &inPath)
{
	if (!FileSystem::DirectoryExists(inPath)) {
		mIsValid = false;
		gLog.log(eTypLogError, "Err > Incorrect application database directory: %s", inPath.c_str());
		return;
	}

	mAppDbDir = FileSystem::SetDirectorySeparator(inPath);
}
//------------------------------------------------------------------------------

bool App::InitApp()
{
	// App::InitApp() and InitEngine() should have the same initialisation

	// Quit if already invalid
	if (!mIsValid)
		return false;

	// Init string declarations
	InitStringDecl();

	// Make sure we're loaded with the right version of the DB library
	CheckDbVersion();

	// Make sure we're loaded with the right version of zlib library
	CheckZlibVersion();

	// Create the global UnicodeString Collator
	mIsValid &= CreateCollator();

	// Create the global plugin kernel
	CreatePluginKernel();

	return mIsValid;
}
//------------------------------------------------------------------------------

void App::CloseApp()
{
	// App::CloseApp() and CloseEngine() should have the same termination

	// Delete the global plugin kernel
	DeletePluginKernel();

	// Delete the global UnicodeString Collator
	DeleteCollator();

	// Get the list of all databases from the memory pool statistics
	/*
	if (dbEnv != NULL) {
		DbUtil::DbList dbList;
		DbUtil::StaticGetDbList(*dbEnv, dbList);
		//DbUtil::StaticRemoveDbList(*dbEnv, dbList);
		//char **list = NULL;
		//dbEnv->log_archive(&list, DB_ARCH_REMOVE);
	}
	*/
}
//------------------------------------------------------------------------------

bool App::ParseDataAdorner()
{
	bool isOk = true;

	if (mAppSrcDir.empty()) {
		gLog.log(eTypLogError, "Err > Empty application source directory");
		isOk = false;
	}
	if (mAppDstDir.empty()) {
		gLog.log(eTypLogError, "Err > Empty application destination directory");
		isOk = false;
	}
	if (!mAppSrcDir.empty() && mAppSrcDir == mAppDstDir) {
		gLog.log(eTypLogError, "Err > Application directories source and destination must be different");
		isOk = false;
	}
	if (!isOk) {
		return false;
	}

	// Create the parser
	const std::string &catalogFile = XercesParser::StaticGetDefaultCatalogFile();
	std::auto_ptr<Parser> xercesParser(new Parser(SAX2XMLReader::Val_Auto, catalogFile, eTypHandlerAdorner));
	// Get the handler
	AdornerHandler *adornerHandler = static_cast<AdornerHandler *>(xercesParser->GetHandler());

	FileSystem::FileSet fileSet;
	FileSystem::GetDirectoryFiles(fileSet, mAppSrcDir.c_str(), "*.xml");
	if (!fileSet.size())
		gLog.log(eTypLogError, "Err > No xml files to parse in directory: %s", mAppSrcDir.c_str());

	// Test file only ?
	bool wantTest = false;
	if (wantTest) {
		fileSet.clear();
		// Test files: utf8_test AaeNC01 LacAD01_01 GlgAD01_03
		std::string inputFile("./data/utf8_test.xml");
		fileSet.insert(inputFile);
	}

	// Parse the files
	unsigned int numDoc = 0;
	UnicodeString docName; // Empty => no database document storage
	std::string dstFileName, dstFullFileName;
	FileSystem::FileSet::const_iterator it;
	FileSystem::FileSet::const_iterator itEnd;
	for (it = fileSet.begin(), itEnd = fileSet.end(); it != itEnd; ++it) {
		const std::string &fullFileName = (*it);
		adornerHandler->SetDocRef(++numDoc);
		dstFileName = FileSystem::GetFileName(fullFileName);
		dstFullFileName = FileSystem::MakeFullFileName(mAppDstDir, dstFileName);
		adornerHandler->SetDstFileName(dstFullFileName);
		isOk &= xercesParser->XercesParse(fullFileName);
	}

	return isOk;
}
//------------------------------------------------------------------------------

bool App::ParseDataIndexer()
{
	bool isOk = true;

	if (mAppSrcDir.empty()) {
		gLog.log(eTypLogError, "Err > Empty application source directory");
		isOk = false;
	}
	if (mAppDstDir.empty()) {
		gLog.log(eTypLogError, "Err > Empty application destination directory");
		isOk = false;
	}
	if (!mAppSrcDir.empty() && mAppSrcDir == mAppDstDir) {
		gLog.log(eTypLogError, "Err > Application directories source and destination must be different");
		isOk = false;
	}
	if (mAppDbDir.empty()) {
		gLog.log(eTypLogError, "Err > Empty application database directory");
		isOk = false;
	}
	if (!isOk) {
		return false;
	}

	// Create the main dababase environment
	//FileSystem::ClearDirectoryContent(mAppDbDir, "__db.*");
	std::auto_ptr<EngineEnv> engineEnv(new EngineEnv());
	if (!engineEnv->CreateDbEnv(mAppDbDir))
		return false;
	engineEnv->SetColId("ColApp");

	// Create the database manager
	std::auto_ptr<DbManager> dbMgr(new DbManager(engineEnv->GetDbEnv(), engineEnv->GetColId()));
	//dbMgr->SetClearOnOpen();
	//dbMgr->SetRemoveOnClose(true);

	// Create the parser
	const std::string &catalogFile = XercesParser::StaticGetDefaultCatalogFile();
	std::auto_ptr<Parser> xercesParser(new Parser(SAX2XMLReader::Val_Auto, catalogFile, eTypHandlerIndexer));
	// Get the handler
	IndexerHandler *indexerHandler = static_cast<IndexerHandler *>(xercesParser->GetHandler());
	
	// Initialize the handler database manager
	indexerHandler->SetDbManager(dbMgr.get());

	// Init the configuration directory
	std::string appConfigDir("./config/");

	// Initialize the handler indexation configuration
	std::string indexerConfigFileName, indexerConfigFullFileName;
	indexerConfigFileName = "indexer_config_default.xml";
	indexerConfigFullFileName = FileSystem::MakeFullFileName(appConfigDir, indexerConfigFileName);
	indexerHandler->LoadIndexerConfig(indexerConfigFullFileName);

	// Initialize the record breaker configuration
	std::string recordBreakerConfigFileName, recordBreakerConfigFullFileName;
	recordBreakerConfigFileName = "recordbreaker_config_default.xml";
	recordBreakerConfigFullFileName = FileSystem::MakeFullFileName(appConfigDir, recordBreakerConfigFileName);
	indexerHandler->LoadRecordBreakerConfig(recordBreakerConfigFullFileName);
	indexerHandler->EnableRecordBreaker(true);

	FileSystem::FileSet fileSet;
	FileSystem::GetDirectoryFiles(fileSet, mAppSrcDir.c_str(), "*.xml");
	if (!fileSet.size())
		gLog.log(eTypLogError, "Err > No xml files to parse in directory: %s", mAppSrcDir.c_str());

	// Test file only ?
	bool wantTest = false;
	if (wantTest) {
		fileSet.clear();
		// Test files: utf8_test AaeNC01 LacAD01_01 GlgAD01_03
		std::string inputFile("./data/utf8_test.xml");
		fileSet.insert(inputFile);
	}

	// Parse the files
	unsigned int numDoc = 0;
	bool wantAdornerOutput = true;
	UnicodeString docName; // Empty => no database document storage
	std::string dstFileName, dstFullFileName;
	FileSystem::FileSet::const_iterator it;
	FileSystem::FileSet::const_iterator itEnd;
	for (it = fileSet.begin(), itEnd = fileSet.end(); it != itEnd; ++it) {
		const std::string &fullFileName = (*it);
		indexerHandler->SetDocRef(++numDoc);
		if (wantAdornerOutput) {
			dstFileName = FileSystem::GetFileName(fullFileName);
			dstFullFileName = FileSystem::MakeFullFileName(mAppDstDir, dstFileName);
			indexerHandler->SetDstFileName(dstFullFileName);
		}
		isOk &= xercesParser->XercesParse(fullFileName);
	}

	return isOk;
}
//------------------------------------------------------------------------------

int App::RunUnitTest()
{
	AppAssert(mIsValid);
	
	std::set<int> logLevelsBak = gLog.getLogLevels();
	gLog.clearLogLevels(); // Raz log levels
	gLog.setVerbose(1); // Show only errors

	int returnValue = Common::RunUnitTest();

	// Remove unit test collection content
	/* Must be done in each unit test
	EngineEnv *engineEnv = new EngineEnv;
	if (!engineEnv->CreateDbEnv("./data_unittest/"))
		return false;
	engineEnv->SetColId("ColUnitTest");
	Collection *collection = new Collection(engineEnv->GetDbEnv(), engineEnv->GetColId());
	collection->ClearCollection();
	delete collection;
	delete engineEnv;
	*/

	if (returnValue)
		gLog.log(eTypLogError, "Err > UnitTest failure");

	gLog.setLogLevels(logLevelsBak);

	return returnValue;
}
//------------------------------------------------------------------------------

int App::RunAdorner()
{
	AppAssert(mIsValid);

	int returnValue = 0;
	try {
		// Display application start
		gLog.log(eTypLogAction, "Act > Start application adorner");

		if (!ParseDataAdorner())
			returnValue = -1;

		// Display application stop
		gLog.log(eTypLogAction, "Act > Stop application adorner");
	}
	catch (AppException &e) {
		gLog.log(eTypLogError, "Err > Application exception during application adorner execution: %s", e.what());
		returnValue = e.getExceptionCode();
	}
	catch (DbException &e) {
		gLog.log(eTypLogError, "Err > DB exception during application adorner execution: %s", e.what());
		returnValue = e.get_errno();
	}
	catch (std::exception &e) {
		gLog.log(eTypLogError, "Err > Unknown exception during application adorner execution: %s", e.what());
		returnValue = -1;
	}

	return returnValue;
}
//------------------------------------------------------------------------------

int App::RunIndexer()
{
	AppAssert(mIsValid);

	int returnValue = 0;
	try {
		// Display application start
		gLog.log(eTypLogAction, "Act > Start application indexer");

		// Doesn't work:
		//int err = dbEnv->dbremove(NULL /* no transaction */, "__db.001", 0, 0);

		// Delete the whole database
		//dbEnv->dbremove(NULL, ".\\data\\__db.001", 0, 0);
		//Db db(dbEnv, 0);
		//int err = db.remove(home, 0, 0 /* flags */);

		if (!ParseDataIndexer())
			returnValue = -1;

		// Display application stop
		gLog.log(eTypLogAction, "Act > Stop application indexer");
	}
	catch (AppException &e) {
		gLog.log(eTypLogError, "Err > Application exception during application indexer execution: %s", e.what());
		returnValue = e.getExceptionCode();
	}
	catch (DbException &e) {
		gLog.log(eTypLogError, "Err > DB exception during application indexer execution: %s", e.what());
		returnValue = e.get_errno();
	}
	catch (std::exception &e) {
		gLog.log(eTypLogError, "Err > Unknown exception during application indexer execution: %s", e.what());
		returnValue = -1;
	}

	return returnValue;
}
//------------------------------------------------------------------------------

int App::RunXmlFile(const std::string &inFileName)
{
	AppAssert(mIsValid);

	int returnValue = 0;
	try {
		// Display execution start
		gLog.log(eTypLogAction, "Act > Start application xmlexec");

		std::auto_ptr<AtollPlugin::PluginMessage> pluginMessage;
		pluginMessage = XmlExecFile(inFileName);

		// Display execution stop
		gLog.log(eTypLogAction, "Act > Stop application xmlexec");
	}
	catch (AppException &e) {
		gLog.log(eTypLogError, "Err > Application exception during execution: %s", e.what());
		returnValue = e.getExceptionCode();
	}
	catch (DbException &e) {
		gLog.log(eTypLogError, "Err > DB exception during execution: %s", e.what());
		returnValue = e.get_errno();
	}
	catch (std::exception &e) {
		gLog.log(eTypLogError, "Err > Unknown exception during execution: %s", e.what());
		returnValue = -1;
	}

	return returnValue;
}
//------------------------------------------------------------------------------
