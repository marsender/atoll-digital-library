/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Engine.hpp

*******************************************************************************/

#ifndef __Engine_HPP
#define	__Engine_HPP
//------------------------------------------------------------------------------

#include "../common/LibExport.hpp"
#include <string>
//------------------------------------------------------------------------------

namespace Common
{
class Logger;
}

namespace Atoll
{

//! Main application class
/**
	This class is created and initialised by the main program,
	which then calls the run method to execute the work

	Logic:
		- Create the main application collator
		- Create the main database environment
*/
class DEF_Export App
{
public:
  App();
	~App();

	//! Get atoll library version
	std::string GetEngineVersion();
	//! Get atoll library logger
	Common::Logger &GetEngineLogger();

	//! Init application
	bool InitApp();
	//! Run unit tests
  int RunUnitTest();
	//! Run the application adorner
  int RunAdorner();
	//! Run the application indexer
  int RunIndexer();
	//! Run and execute an xml file
  int RunXmlFile(const std::string &inFileName);
	//! Set the application log file
	void SetAppLogFile(const std::string &inFileName);
	//! Set the application log verbosity
	void SetAppLogVerbose(int inVerbose);
	//! Set the application source directory
	void SetAppSrcDir(const std::string &inPath);
	//! Set the application destination directory
	void SetAppDstDir(const std::string &inPath);
	//! Set the application database directory
	void SetAppDbDir(const std::string &inPath);

private:
	bool mIsValid;
	std::string mLogFileName;
	std::string mAppSrcDir, mAppDstDir, mAppDbDir;

	void CloseApp();
	bool ParseDataAdorner();
	bool ParseDataIndexer();
};
//------------------------------------------------------------------------------

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
