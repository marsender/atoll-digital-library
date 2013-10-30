/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Main.cpp

Main program entry point.

Logic:
	- Parse the program arguments
	- Create an application object, initialize it an run the application

*******************************************************************************/

#include "../Portability.hpp"
#include "../engine/App.hpp"
#include <memory> // for std::auto_ptr
#ifndef _WIN32
	#include <unistd.h> // getopt
#endif
#ifdef _WIN32
	#include "vld.h" // Visual Leak Detector (Memory leak detection)
#endif 
//------------------------------------------------------------------------------

// Application version
#define DEF_AppVersion "1.2"
// 17/10/08 1.2 Add adorner command line option
// 29/06/08 1.1 Refactoring with engine library
// 01/07/07 1.0 Begining of implementation

#define DEF_AppName "Atoll console"
//------------------------------------------------------------------------------

using namespace Common;
//------------------------------------------------------------------------------

//! Program usage
int Usage(const char *inProgname)
{
	std::cout << inProgname << ": [options]\n"
		"\n"
		"Options:\n"
		"  -j <test|adorner|indexer> job\n"
		"  -h <home> database home directory\n"
		"  -s <srcdir> source directory\n"
		"  -d <dstdir> destination directory\n"
		"  -l <logfile> log file name\n"
		"  -x <xmlfile> xmlexec file name\n"
		"  -v   verbose -- Using the switch a second time increases verbosity\n"
		"  -V   version -- Print software version\n"
		"  -?   help -- Print this message\n"
		<< std::endl;

	std::cout << "Press any key to continue...";
	getc(stdin);

	return (-1);
}
//------------------------------------------------------------------------------

#ifdef _WIN32
#undef	BADCH
#define	BADCH (int)'?'
#undef	BADARG
#define	BADARG (int)':'
#undef	EMSG
#define	EMSG	""
char	*optarg = NULL; // argument associated with option
//! Parse argc/argv argument vector.
int getopt(int nargc, char * const *nargv, const char *ostr)
{
	static char *staticProgname = NULL;
	static char *staticPlace = EMSG; // option letter processing
	static int staticOptind = 1; // index into parent argv vector

	char *oli; // Option letter list index
	int	opterr = 1; // If error message should be printed
	int optopt = 0; // Character checked for validity

	if (staticProgname == NULL) {
		staticProgname = strrchr(nargv[0], '/');
		if (staticProgname == NULL)
			staticProgname = strrchr(nargv[0], '\\');
		if (staticProgname)
			staticProgname++;
		else
			staticProgname = nargv[0];
	}

	if (!*staticPlace) {		/* update scanning pointer */
		if (staticOptind >= nargc || *(staticPlace = nargv[staticOptind]) != '-') {
			staticPlace = EMSG;
			return (EOF);
		}
		if (staticPlace[1] && *++staticPlace == '-') {	/* found "--" */
			++staticOptind;
			staticPlace = EMSG;
			return (EOF);
		}
	} // Option letter okay?
	if ((optopt = (int) * staticPlace++) == (int)':' ||
			!(oli = (char *)strchr(ostr, optopt))) {
		/*
		 * if the user didn't specify '-' as an option,
		 * assume it means EOF.
		 */
		if (optopt == (int)'-')
			return (EOF);
		if (!*staticPlace)
			++staticOptind;
		if (opterr && *ostr != ':')
			fprintf(stderr, "%s: illegal option -- %c\n", staticProgname, optopt);
		return (BADCH);
	}
	if (*++oli != ':') {			/* don't need argument */
		optarg = NULL;
		if (!*staticPlace)
			++staticOptind;
	}
	else { // Need an argument
		if (*staticPlace)			/* no white space */
			optarg = staticPlace;
		else if (nargc <= ++staticOptind) {	/* no arg */
			staticPlace = EMSG;
			if (*ostr == ':')
				return (BADARG);
			if (opterr)
				fprintf(stderr, "%s: option requires an argument -- %c\n", staticProgname, optopt);
			return (BADCH);
		}
		else // White space
			optarg = nargv[staticOptind];
		staticPlace = EMSG;
		++staticOptind;
	}

	return (optopt);			/* dump back option letter */
}
#endif // _WIN32
//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	const char *progname;
	char ch, lastChar;

	progname = strrchr(argv[0], '/');
	if (progname == NULL)
		progname = strrchr(argv[0], '\\');
	if (progname)
		progname++;
	else
		progname = argv[0];

	if (argc == 1)
		return Usage(progname);

	int verbose = 0;
	bool wantVersion = false; // Display program version ?
	bool wantUnitTest = false; // Run unit tests ?
	bool wantAdorner = false; // Run adorner
	bool wantIndexer = false; // Run indexer

	// Initialize the directories
	std::string srcDir;
	std::string dstDir;
	std::string dbDir;
	std::string logFile;
	std::string xmlExecFile;

	// Parse the command line arguments
	while ((ch = getopt(argc, argv, "j:s:d:h:l:x:vV?")) != EOF) {
		switch (ch) {
		case 'j':
			if (!optarg)
				return Usage(progname);
			if (strcmp(optarg, "test") == 0)
				wantUnitTest = true;
			else if (strcmp(optarg, "adorner") == 0)
				wantAdorner = true;
			else if (strcmp(optarg, "indexer") == 0)
				wantIndexer = true;
			else
				return Usage(progname);
			break;
		case 's':
			if (!optarg)
				return Usage(progname);
			srcDir = optarg;
			lastChar = srcDir[srcDir.size() - 1];
			if (lastChar != '/' && lastChar != '\\')
				return Usage(progname);
			break;
		case 'd':
			if (!optarg)
				return Usage(progname);
			dstDir = optarg;
			lastChar = dstDir[dstDir.size() - 1];
			if (lastChar != '/' && lastChar != '\\')
				return Usage(progname);
			break;
		case 'h':
			if (!optarg)
				return Usage(progname);
			dbDir = optarg;
			lastChar = dbDir[dbDir.size() - 1];
			if (lastChar != '/' && lastChar != '\\')
				return Usage(progname);
			break;
		case 'l':
			if (!optarg)
				return Usage(progname);
			logFile = optarg;
			break;
		case 'x':
			if (!optarg)
				return Usage(progname);
			xmlExecFile = optarg;
			break;
		case 'v':
			verbose++;
			break;
		case 'V':
			wantVersion = true;
			break;
		case '?':
		default:
			return Usage(progname);
		}
	}

	// Create the application
	std::auto_ptr<Atoll::App> app(new Atoll::App());

	// Get the application logger
	Common::Logger &logger = app->GetEngineLogger();

	int returnValue = 0;
	try {
		if (wantVersion) {
			std::cout << DEF_AppName << " version: " << DEF_AppVersion << std::endl;
			std::cout << app->GetEngineVersion() << std::endl;
			return 0;
		}
		app->SetAppLogVerbose(verbose);
		app->SetAppLogFile(logFile);
		if (!srcDir.empty())
			app->SetAppSrcDir(srcDir);
		if (!dstDir.empty())
			app->SetAppDstDir(dstDir);
		if (!dbDir.empty())
			app->SetAppDbDir(dbDir);
		// Initialize
		if (!app->InitApp())
			DEF_Exception(AppException::InternalError, "Application initilisation");
		if (wantUnitTest)
			returnValue = app->RunUnitTest();
		if (wantAdorner)
			returnValue = app->RunAdorner();
		if (wantIndexer)
			returnValue = app->RunIndexer();
		if (xmlExecFile.length()) {
			returnValue = app->RunXmlFile(xmlExecFile);
		}
	}
	catch (const AppException &e) {
		logger.log(eTypLogError, "Err > Main exception: %s", e.what());
		returnValue = -1;
	}
	catch (std::exception &e) {
		logger.log(eTypLogError, "Err > Main exception: %s", e.what());
		returnValue = -1;
	}

/*
#ifdef WIN32
	std::cout << "Press any key to continue...";
	getc(stdin);
#endif
*/

	return returnValue;
}
//------------------------------------------------------------------------------
