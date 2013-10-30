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

Logger.cpp

*******************************************************************************/

#include "Logger.hpp"
#include <iomanip> // For setw
//#include <stdio.h> // For vsprintf
//#include <fstream> // For fstream (logging to file)
//#include <pthread.h>
#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h> // For DWORD and GetTickCount() function
	#include <time.h> // For time_t
#else
	#include <stdio.h>
	#include <stdarg.h> // For va_start
	#include <string.h>
#endif
//------------------------------------------------------------------------------

#define DEF_WantCerrInfo    true   //!< Output error infos on cerr ?
#define DEF_MaxHeaderSize   300    //!< maximum size of a header info
#define DEF_BufSize         3072   //!< maximum size of a log message
//------------------------------------------------------------------------------

/*
	// Log sample:
  log.log(3, "hello");
	log.setHeader("jelle: ");
  log.logWithTime(2, "time1");
#ifdef WIN32
  Sleep(1000);
#else
  poll(0,0,1000);
#endif
  log.logWithTime(3, "time2");
  int j = 2;
  double i = 2.324234;
  //printf("hoi: "); fflush(stdout);
  log.log(1, "|%f %d|", i, j);
	log.removeLogLevel(3);
  log.log(3, "no level 3");
*/

/*****************************************************************************/
/**************************** LOGGER *****************************************/
/*****************************************************************************/

using namespace Common;
//------------------------------------------------------------------------------

Logger::Logger(std::ostream &o /*= std::cout*/)
{
	//pthread_mutex_init(&mutex_stream, NULL);
	mNbLogError = mNbLogWarning = 0;
	mBuf = new char[DEF_BufSize];
	mHeader = new char[DEF_MaxHeaderSize];
	*mHeader = 0;
	m_timing.restartTime();
	setVerbose(1); // Set default verbosity
	m_os = &o;
	m_os2 = NULL;
	mEnableOs1 = mEnableOs2 = true;
}
//------------------------------------------------------------------------------

Logger::~Logger()
{
	delete [] mBuf;
	delete [] mHeader;
}
//------------------------------------------------------------------------------

bool Logger::log(int iLevel, std::string inStr)
{
	if (isInLogLevel(iLevel)) {
		if (iLevel <= eTypLogWarning)
			iLevel <= eTypLogError ? mNbLogError++ : mNbLogWarning++;
		if (iLevel != eTypLogAction) {
			logSignal();
		}
		if (mEnableOs1) {
			*m_os << mHeader << inStr;
		}
		if (mEnableOs2 && m_os2 /*&& iLevel != eTypLogAction*/) {
			*m_os2 << mHeader << inStr;
			m_os2->flush();
		}
		return true;
	}

	return false;
}
//------------------------------------------------------------------------------

bool Logger::log(int iLevel, const char *inStr, ...)
{
	if (isInLogLevel(iLevel)) {
		if (iLevel <= eTypLogWarning)
			iLevel <= eTypLogError ? mNbLogError++ : mNbLogWarning++;
		if (iLevel != eTypLogAction) {
			logSignal();
		}
		va_list ap;
		va_start(ap, inStr);
		if (vsnprintf(mBuf, DEF_BufSize - 1, inStr, ap) == -1) {
			if (DEF_WantCerrInfo)
				std::cerr << "Logger: log buffer is too small !" << std::endl;
		}
		va_end(ap);
		if (mEnableOs1) {
			*m_os << mHeader << mBuf << std::endl;
		}
		if (mEnableOs2 && m_os2 /*&& iLevel != eTypLogAction*/) {
			*m_os2 << mHeader << mBuf << std::endl;
			m_os2->flush();
		}
		return true;
	}

	return false;
}
//------------------------------------------------------------------------------

bool Logger::logWithTime(int iLevel, const char *inStr, ...)
{
	if (isInLogLevel(iLevel)) {
		if (iLevel <= eTypLogWarning)
			iLevel <= eTypLogError ? mNbLogError++ : mNbLogWarning++;
		if (iLevel != eTypLogAction) {
			logSignal();
		}
		va_list ap;
		va_start(ap, inStr);
		if (vsnprintf(mBuf, DEF_BufSize - 1, inStr, ap) == -1) {
			if (DEF_WantCerrInfo)
				std::cerr << "Logger: logWithTime buffer is too small !" << std::endl;
		}
		va_end(ap);
		std::string s = mHeader;
		s.append(mBuf);
		s.copy(mBuf, std::string::npos);
		mBuf[s.length()] = '\0';
		m_timing.printTimeDiffWithText(*m_os, mBuf);
		if (m_os2 /*&& iLevel != eTypLogAction*/) {
			m_timing.printTimeDiffWithText(*m_os2, mBuf);
			m_os2->flush();
		}

		return true;
	}

	return false;
}
//------------------------------------------------------------------------------

void Logger::logAddSignal(const char *inStr, ...)
{
	char buf[DEF_BufSize];

	va_list ap;
	va_start(ap, inStr);
	if (vsnprintf(buf, DEF_BufSize - 1, inStr, ap) == -1) {
		if (DEF_WantCerrInfo)
			std::cerr << "Logger: logAddSignal buffer is too small !" << std::endl;
	}
	va_end(ap);

	//char str[16];
	//sprintf(str, "%2.2f: ", m_timing.getElapsedTime()*1000);
	//m_strSignal.append(str);

	m_strSignal = mHeader;
	m_strSignal += buf;
}
//------------------------------------------------------------------------------

void Logger::logClearSignal()
{
	if (m_strSignal.empty())
		return;

	m_strSignal = "";
}
//------------------------------------------------------------------------------

bool Logger::logSignal()
{
	if (!m_strSignal.empty()) {
		if (mEnableOs1) {
			*m_os << m_strSignal << std::endl ;
		}
		if (mEnableOs2 && m_os2) {
			*m_os2 << m_strSignal << std::endl ;
			m_os2->flush();
		}
		m_strSignal = "";
		return true;
	}

	return false;
}
//------------------------------------------------------------------------------

void Logger::restartTimer()
{
	return m_timing.restartTime();
}
//------------------------------------------------------------------------------

Timing Logger::getTiming() const
{
	return m_timing;
}
//------------------------------------------------------------------------------

bool Logger::isInLogLevel(int iLevel)
{
	return m_setLogLevels.find(iLevel) != m_setLogLevels.end() ;
}
//------------------------------------------------------------------------------

std::set<int> Logger::getLogLevels()
{
	return m_setLogLevels;
}
//------------------------------------------------------------------------------

void Logger::setLogLevels(std::set<int> &inLogLevels)
{
	m_setLogLevels = inLogLevels;
}
//------------------------------------------------------------------------------

void  Logger::clearLogLevels()
{
	m_setLogLevels.clear();
}
//------------------------------------------------------------------------------

void  Logger::addLogLevel(int iLevel)
{
	m_setLogLevels.insert(iLevel);
}
//------------------------------------------------------------------------------

void  Logger::removeLogLevel(int iLevel)
{
	m_setLogLevels.erase(iLevel);
}
//------------------------------------------------------------------------------

void Logger::addLogRange(int iMin, int iMax)
{
	for (int i = iMin; i <= iMax; i++)
		addLogLevel(i);
}
//------------------------------------------------------------------------------

void Logger::removeLogRange(int iMin, int iMax)
{
	for (int i = iMin; i <= iMax; i++)
		removeLogLevel(i);
}
//------------------------------------------------------------------------------

void Logger::showLogLevels(std::ostream &os)
{
	std::set<int>::iterator itr;
	for (itr = m_setLogLevels.begin(); itr != m_setLogLevels.end(); ++itr)
		os << *itr << " ";
}
//------------------------------------------------------------------------------

void Logger::setVerbose(int inVerbose)
{
	// Clear verbosity
	clearLogLevels();

	// Minimum verbosity => Always display errors
	addLogLevel(eTypLogFatal);
	addLogLevel(eTypLogError);

	// Turn on logging if extra verbose is specified
	switch (inVerbose) {
	case 0:
		// Minimum verbosity
		break;
	case 1:
		// Small verbosity 'v' => Add info
		addLogLevel(eTypLogWarning);
		break;
	case 2:
		// Medium verbosity 'vv' => Add info actions
		addLogLevel(eTypLogWarning);
		addLogLevel(eTypLogAction);
		break;
	case 3:
		// High verbosity 'vvv' => Add info actions debug
		addLogLevel(eTypLogWarning);
		addLogLevel(eTypLogAction);
		addLogLevel(eTypLogDebug);
		break;
	default:
		// Highest verbosity 'vvvv' => All
		addLogRange(eTypLogNone + 1, eTypLogEnd - 1); // 'vvv'
		break;
	}
}
//------------------------------------------------------------------------------

char *Logger::getHeader()
{
	return mHeader;
}
//------------------------------------------------------------------------------

void Logger::setHeader(const char *inStr, ...)
{
	va_list ap;
	va_start(ap, inStr);
	if (vsnprintf(mHeader, DEF_MaxHeaderSize - 1, inStr, ap) == -1) {
		if (DEF_WantCerrInfo)
			std::cerr << "Logger: setHeader buffer is too small !" << std::endl;
	}
	va_end(ap);

	if (*mHeader && strlen(mHeader) < DEF_MaxHeaderSize - 3)
		strcat(mHeader, ": ");
}
//------------------------------------------------------------------------------

void Logger::setOutputStream(std::ostream &o)
{
	m_os = &o;
}
//------------------------------------------------------------------------------

void Logger::setOutputStream2(std::ostream &o, const char *in_os2_filename /*= NULL*/)
{
	m_os2 = &o;

	m_os2_filename = in_os2_filename ? in_os2_filename : "";
}
//------------------------------------------------------------------------------

std::ostream &Logger::getOutputStream()
{
	return *m_os;
}
//------------------------------------------------------------------------------

std::ostream &Logger::getOutputStream2()
{
	return *m_os2;
}
//------------------------------------------------------------------------------

const char *Logger::getOutputStream2FileName()
{
	return m_os2_filename.c_str();
}
//------------------------------------------------------------------------------

void Logger::removeOutputStream2()
{
	m_os2 = NULL;
	m_os2_filename = "";
}
//------------------------------------------------------------------------------


/*****************************************************************************/
/********************** CLASS TIMING *****************************************/
/*****************************************************************************/

Timing::Timing()
{
#ifdef WIN32
	mTime = 0;
#else
	mTime.tv_sec = 0;
	mTime.tv_usec = 0;
#endif
}
//------------------------------------------------------------------------------

#ifdef WIN32
double Timing::getTimeDifference(DWORD tv1, DWORD tv2)
{
	return ((double)(tv1 - tv2) / 1000.0) ;
}
#else
double Timing::getTimeDifference(struct timeval tv1, struct timeval tv2)
{

	return ((double)tv1.tv_sec + (double)tv1.tv_usec / 1000000) -
		((double)tv2.tv_sec + (double)tv2.tv_usec / 1000000) ;
}
#endif
//------------------------------------------------------------------------------

void Timing::printTimeDiffWithText(std::ostream &os, const char *inStr, int iFactor)
{
	// set the with to 6 and fill remaining places with '0'.
	os << std::setw(6) << std::setfill('0') << getElapsedTime()*iFactor << ":" << inStr << std::endl;
}
//------------------------------------------------------------------------------

double Timing::getElapsedTime(int iFactor)
{
#ifdef WIN32
	DWORD time2 = ::GetTickCount();
#else
	struct timeval time2;
	gettimeofday(&time2, NULL);
#endif
	return getTimeDifference(time2, mTime)*iFactor;
}
//------------------------------------------------------------------------------

void Timing::restartTime()
{
#ifdef WIN32
	mTime = ::GetTickCount();
#else
	gettimeofday(&mTime, NULL);
#endif
}
//------------------------------------------------------------------------------


/*****************************************************************************/
/**************************** TESTING PURPOSES *******************************/
/*****************************************************************************/

/*
#ifdef WIN32
  #include <windows.h>
#else
  #include<sys/poll.h>
#endif

int main()
{
  ofstream fout("temp.txt");
  Logger log(fout, 0, 2);
  log.log(0, "hello");
  log.setHeader("jelle");
#ifdef WIN32
  Sleep(1000);
#else
  poll(0,0,1000);
#endif
  log.log(2, "hello");
  log.log(3, "hello");
  int j = 2;
  double i = 2.324234;
  printf("hoi: "); fflush(stdout);
  log.logWithTime(1, "|%f %d|", i, j);
}
*/
