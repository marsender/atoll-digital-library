/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

EngineLib.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "EngineLib.hpp"
#include "EngineApi.hpp"
#include "InitStringDecl.hpp"
#include "../database/DbUtil.hpp"
#include "../plugin/PluginMessage.hpp"
#include "unicode/ucnv.h"
#include "unicode/ustring.h"
#include <fstream>
//------------------------------------------------------------------------------

#define DEF_LogFileName    "atoll_engine.log"
//------------------------------------------------------------------------------

using namespace Atoll;
using namespace Common;

namespace Atoll
{
//! Get the file name for the engine log file
void GetEngineLogFileName(std::string &outFileName);
}
//------------------------------------------------------------------------------

bool InitEngine()
{
	// App::InitApp() and InitEngine() should have the same initialisation

	bool isOk = true;

	// Init string declarations
	InitStringDecl();

	// Make sure we're loaded with the right version of the DB library
	CheckDbVersion();

	// Make sure we're loaded with the right version of zlib library
	CheckZlibVersion();

	// Create the global UnicodeString Collator
	isOk &= CreateCollator();

	// Create the global plugin kernel
	CreatePluginKernel();

	return isOk;
}
//------------------------------------------------------------------------------

void CloseEngine()
{
	// App::CloseApp() and CloseEngine() should have the same termination

	// Delete the global plugin kernel
	DeletePluginKernel();

	// Delete the global UnicodeString Collator
	DeleteCollator();

	// Delete the main database environment
	//DbUtil::StaticDeleteEnvironment(dbEnv);
}
//------------------------------------------------------------------------------

void Atoll::GetEngineLogFileName(std::string &outFileName)
{
	outFileName = GetTracePath();
	outFileName += "/";
	outFileName += DEF_LogFileName;
}
//------------------------------------------------------------------------------

#if defined(WIN32)
DEF_Export void AtollLoadLib()
#else
void DEF_Export AtollLoadLib()
#endif
{
	// Called after loading the library
	gLog.setVerbose(1); // Show only errors
	std::string logFileName;
	GetEngineLogFileName(logFileName);
	OpenLogFile(logFileName);
	if (!InitEngine()) {
		CloseEngine();
	}

	gLog.log(eTypLogAction, "Act > AtollLoadLib");
}
//------------------------------------------------------------------------------

#if defined(WIN32)
DEF_Export void AtollFreeLib()
#else
void DEF_Export AtollFreeLib()
#endif
{
	gLog.log(eTypLogAction, "Act > AtollFreeLib");

	// Called before unloading the library
	CloseEngine();
}
//------------------------------------------------------------------------------

#if defined(WIN32)
DEF_Export char *AtollXmlExec(const char *inStr)
#else
char *DEF_Export AtollXmlExec(const char *inStr)
#endif
{
	if (!inStr || *inStr == 0)
		return NULL;

	// Debug
	//gLog.log(eTypLogFatal, inStr);
	
	gLog.log(eTypLogAction, "Act > AtollXmlExec start");

	// Set up the converter
	UErrorCode status = U_ZERO_ERROR;
	UConverter *conv = ucnv_open("utf-8", &status);
	if (!U_SUCCESS(status)) {
		gLog.log(eTypLogError, "Err > Utf-8 open converter: %s", u_errorName(status));
		return NULL;
	}

	// Convert char to UChar, as in ConvertString2UnicodeString
	int32_t len;
	UChar *uTarget = NULL;
	{
		int32_t srcLength = (int32_t)strlen(inStr);
		// Create the target buffer
		int32_t targetLength = 2 * srcLength;
		uTarget = new UChar[targetLength];

		// Convert the string
		len = ucnv_toUChars(conv, uTarget, targetLength, inStr, srcLength, &status);
		if (!U_SUCCESS(status)) {
			gLog.log(eTypLogError, "Err > Char to UChar conversion: %s", u_errorName(status));
			ucnv_close(conv);
			delete [] uTarget;
			return NULL;
		}
	}

	// Execute the request
	std::unique_ptr<AtollPlugin::PluginMessage> pluginMessage;
	pluginMessage = XmlExecBuffer(uTarget, len);
	if (!pluginMessage->mIsOk) {
		pluginMessage->mMessage = pluginMessage->mError;
	}
	delete [] uTarget;

	// Convert UChar to char, as in ConvertUnicodeString2String
	const UnicodeString &str = pluginMessage->mMessage;
	char *target = NULL;
	{
		const UChar *src = str.getBuffer();
		int32_t srcLength = str.length();

		// Create the target buffer
		int32_t targetLength = UCNV_GET_MAX_BYTES_FOR_STRING(srcLength, ucnv_getMaxCharSize(conv));
		target = new char[targetLength];

		// Convert the string
		len = ucnv_fromUChars(conv, target, targetLength, src, srcLength, &status);
		if (!U_SUCCESS(status)) {
			gLog.log(eTypLogError, "Err > UChar to char conversion: %s", u_errorName(status));
			ucnv_close(conv);
			return NULL;
		}

	}

	// Close the converter
	ucnv_close(conv);

	// Memory must be released from the caller of the library with a call to AtollXmlFree
	//delete [] target;

	gLog.log(eTypLogAction, "Act > AtollXmlExec stop");

	return target;
}
//------------------------------------------------------------------------------

#if defined(WIN32)
DEF_Export void AtollXmlFree(char *inStr)
#else
void DEF_Export AtollXmlFree(char *inStr)
#endif
{
	// Release memory previously allocated from the caller of the library with AtollXmlExec
	if (inStr)
		delete [] inStr;
}
//------------------------------------------------------------------------------

#if defined(WIN32)
DEF_Export const char *AtollVersion()
#else
const char *DEF_Export AtollVersion()
#endif
{
	const char *version = GetFullVersion();

	return version;
};
//------------------------------------------------------------------------------
