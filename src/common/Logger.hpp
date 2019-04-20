/*******************************************************************************

Copyright (c) 2000-2003, Jelle Kok, University of Amsterdam
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the University of Amsterdam nor the names of its
contributors may be used to endorse or promote products derived from this
software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Logger.hpp

*******************************************************************************/

/*! \file Logger.hpp
<pre>
<b>File:</b>          Logger.h
<b>Project:</b>       Robocup Soccer Simulation Team: UvA Trilearn
<b>Authors:</b>       Jelle Kok
<b>Contents:</b>      This file contains the class to log information about the
               system to any output stream. A range can be specified
               for which the received log information is printed.
               Furthermore it is possible to print the time since the
               timer of the Logger has been restarted.
<hr size=2>
<h2><b>Changes</b></h2>
<b>Date</b>             <b>Author</b>          <b>Comment</b>
3/3/2001         Jelle Kok       Initial version created
</pre>
*/

#ifndef __Logger_HPP
#define __Logger_HPP
//------------------------------------------------------------------------------

#include "LibExport.hpp"
#include <iostream>   // For ostream (logging to output stream)
#include <string>     // For string
#include <set>        // For set
#ifdef WIN32
	//typedef unsigned long       DWORD;
#else
	#include <sys/time.h> // For timeval
#endif

namespace Common
{

enum eTypLog {
	eTypLogNone,
	eTypLogFatal,  //!< Fatal error log
	eTypLogError,  //!< Error log
	eTypLogWarning,   //!< Warning log
	eTypLogAction, //!< Action log
	eTypLogDebug,  //!< Debug log
	eTypLogEnd
};
//------------------------------------------------------------------------------

//! Timer class
/**
	This class holds a timer. This timer can be set (restartTime) and text can
	be printed with the elapsed time since the timer was restarted..
*/
class Timing
{
private:
#ifdef WIN32
	unsigned long mTime; //!< number of milliseconds that have elapsed since the Windows was started
#else
	struct timeval mTime; //!< Time the timer has last been restarted
#endif

public:
	Timing();
	
	//! Returns the difference between two timevals in seconds
	/**
		@param tv1 first timeval
		@param tv2 second timeval
		@return double representing the difference between t1 and t2 in seconds
	*/
#ifdef WIN32
	static double getTimeDifference(unsigned long tv1, unsigned long tv2);
#else
	static double getTimeDifference(struct timeval t1, struct timeval t2);
#endif

	//! Prints the time in seconds that elapsed since the timer was restarted
	/**
		It is possible to multiply this time with a factor, such that it can be
		used for different quantity values. In the default case this value is 1000
		with the effect that the resulting value resembles miliseconds.
		@param os output stream to which output should be written.
		@param inStr that should be printed
		@param iFactor with which the elapsed time is multiplied (default 1000)
	*/
	void printTimeDiffWithText(std::ostream &os, const char *inStr, int iFactor = 1000);

	//! This method returns the time (in seconds) since the last time the timer was restarted
	/**
		@return double seconds that have passed since last restart of timer
	*/
	double getElapsedTime(int iFactor = 1);

	//! This method restarts the timer by setting it to the current time
	void restartTime();
};
//------------------------------------------------------------------------------


//! Log class
/**
	This class makes it possible to log information on different abstraction
	levels. All messages are passed to the log method 'log' with a level
	indication. When it has been specified that this level should be logged
	using either the 'addLogLevel' or 'addLogRange' method
	the message is logged, otherwise it is ignored. This makes it
	possible to print only  the information you are interested in.
	There is one global Log class which is used by all classes that use the
	Logger. This instantation of the Logger is located in the file Logger.cpp
	and is called 'Log'. All classes that want use this Logger should make a
	reference to it using the line 'extern Logger Log;' and can then use
	this Logger with the Log.log(...) methods. Furthermore the Logger also
	contains a timer with makes it possible to print the time since the timer
	has been restarted.
*/
class Logger
{
private:
	unsigned int mNbLogError;             //!< Number of error logs
	unsigned int mNbLogWarning;           //!< Number of warning logs
	Timing m_timing;                      //!< timer to print timing information
	char *mBuf;                           //!< buffer needed by different methods
	char *mHeader;                        //!< header string printed before all logs
	std::set<int> m_setLogLevels;         //!< set that contains all log levels

	//pthread_mutex_t mutex_stream;
	std::ostream* m_os;                   //!< output stream to print messages to
	std::ostream* m_os2;                  //!< second output stream to print messages to
	std::string m_os2_filename;           //!< output stream file name
	std::string m_strSignal;              //!< temporary string for other messages

	//! Writes the string that was created with logSetSignal to the specified output stream.
	/**
		@return bool indicating whether the message was logged or not.
	*/
	bool logSignal();

public:
	bool mEnableOs1, mEnableOs2; //!< enabled os

	//! Logger constructor
	/**
		The output stream, the minimal and maximal log level can all be specified.
		The timer in this class is also restarted.
		@param os outputstream (file, cout or cerr) to which information is printed (default cerr)
	*/
	Logger(std::ostream &os = std::cerr);
	
	//! Logger destructor
	~Logger();

	//! Log information if the level is in the level set
	/**
		@param iLevel level corresponding to this message
		@param inStr string that is logged when iLevel is a logging lvel.
		@return bool indicating whether the message was logged or not.
	*/
	bool log(int iLevel, std::string inStr);

	//! Log information if the level is in the level set
	/**
		This method receives a character string that may
		contain format specifiers that are also available to 'printf' (like %d, %f,
		etc.). The remaining arguments are the variables that have to be filled in
		at the location of the specifiers.
		@param iLevel level corresponding to this message
		@param inStr character string with (possible) format specifiers
		@param ... variables that define the values of the specifiers.
		@return bool indicating whether the message was logged or not.
	*/
	DEF_Export bool log(int iLevel, const char *inStr, ...);

	//! Log information if the level is in the level set
	/**
		This method receives a character string that may
		contain format specifiers that are also available to 'printf' (like %d, %f,
		etc.). The remaining arguments are the variables that have to be filled in
		at the location of the specifiers. Before the logged message the elapsed
		time since the timer has been restarted is printed.
		@param iLevel level corresponding to this message
		@param inStr character string with (possible) format specifiers
		@param ... variables that define the values of the specifiers.
		@return bool indicating whether the message was logged or not.
	*/
	DEF_Export bool logWithTime(int iLevel, const char *inStr, ...);

	//! Add log information that will be added before the next log (if there is one)
	/**
		@param inStr character string with (possible) format specifiers
		@param ... variables that define the values of the specifiers.
	*/
	void logAddSignal(const char *inStr, ...);

	//! Clear the signal.
	void logClearSignal();

	//! Restarts the timer associated with this Logger
	DEF_Export void restartTimer();

	//! Return the instance of the timing class that denotes the time the counter is running.
	Timing getTiming() const;

	//! Returns whether the supplied log level is recorded
	/**
		@param iLevel log level that should be checked
		@return bool indicating whether the supplied log level is logged.
	*/
	bool isInLogLevel(int iLevel);

	//! Get the log levels
	std::set<int> getLogLevels();

	//! Set the log levels
	void setLogLevels(std::set<int> &inLogLevels);

	//! Clears the set of logged levels. No log level will be printed
	void clearLogLevels();

	//! Adds the log level to the set of logged levels
	/**
		@param iLevel level that will be added to the set
	*/
	void addLogLevel(int iLevel);

	//! Removes the log level from the set of logged levels
	/**
		@param iLevel level that will be removed from the set
	*/
	void removeLogLevel(int iLevel);

	//! Adds all the log levels in the interval [iMin..iMax] to the set of logged levels
	/**
		@param iMin minimum log level that is added
		@param iMax maximum log level that is added
	*/
	void addLogRange(int iMin, int iMax);

	//! Removes all the log levels in the interval [iMin..iMax] from the set of logged levels
	/**
		@param iMin minimum log level that is removed
		@param iMax maximum log level that is removed
	*/
	void removeLogRange(int iMin, int iMax);

	//! This method outputs all the log levels that are logged to the output stream os
	/**
		@param os output stream to which log levels are printed.
	*/
	void showLogLevels(std::ostream &os);

	//! Set verbosity according to an integer (v, vv, vvv from command line)
	/**
		Different levels:
			- 0 Minimum verbosity => Errors
			- 1 Small verbosity   => Errors info
			- 2 Medium verbosity  => Errors info actions
			- 3 High verbosity    => Errors info actions debug
	*/
	void setVerbose(int inVerbose);

	//! Returns the current log header
	/**
		@return current header
	*/
	char *getHeader();

	//! Sets the header that is written before the actual logging text
	/**
		@param inStr that represents the character string
		@return bool indicating whether the update was succesfull
	*/
	DEF_Export void setHeader(const char *inStr, ...);

	//! Sets the output stream to which the log information is written
	/**
		This outputstream can be standard output (cout) or a reference to a
		file.
		@param os outputstream to which log information is printed
		@return bool indicating whether update was succesfull
	*/
	void setOutputStream(std::ostream &os);

	//! Sets the second output stream to which the log information is written
	/**
		This outputstream can be standard output (cout) or a reference to a
		file.
		@param os outputstream to which log information is printed
		@param in_os2_filename File name of the second output stream to which log information is printed
		@return bool indicating whether update was succesfull
	*/
	void setOutputStream2(std::ostream &os, const char *in_os2_filename = NULL);

	//! Returns the output stream to which the log information is written
	/**
		This outputstream can be standard output (cout) or a reference to a file.
		@return o outputstream to which log information is printed
	*/
	std::ostream &getOutputStream();

	//! Returns the second output stream to which the log information is written
	/**
		This outputstream can be standard output (cout) or a reference to a file.
		@return o outputstream to which log information is printed.
	*/
	std::ostream &getOutputStream2();

	//! Returns the output stream file name (if any)
	const char *getOutputStream2FileName();

	//! Remove the second output stream to which the log information is written
	void removeOutputStream2();

	//! Return the number of error and warning logs since the logger is created
	unsigned int getNbLog() const { return mNbLogError + mNbLogWarning; }

	//! Return the number of error logs since the logger is created
	unsigned int getNbLogError() const { return mNbLogError; }

	//! Return the number of warning logs since the logger is created
	unsigned int getNbLogWarning() const { return mNbLogWarning; }
};
//------------------------------------------------------------------------------

} // namespace Common

// Reference to the global log class
extern Common::Logger gLog;

//------------------------------------------------------------------------------
#endif
