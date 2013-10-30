/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Trace.cpp

*******************************************************************************/

#include "Trace.hpp"
#include "StdString.hpp" // For TimeToString
#include <string.h>
#include <stdarg.h>  // For va_start
#ifndef WIN32
	#include <stdio.h>
#endif
//------------------------------------------------------------------------------

// Default trace path for all projects
#if defined(WIN32)
	#define DEF_TracePath "c:/temp"
#else
	#define DEF_TracePath "/opt/trace"
#endif
// Default trace file for all projects
#define DEF_TraceFile	"_trace.log"

bool gIsTraceOff = false;
unsigned long gNbTrace = 0;	// Nombre de trace d'erreurs
std::string gTracePath(DEF_TracePath);
std::string gTraceFileName(DEF_TraceFile);
std::string gTraceFullFileName;
std::string gTraceEntete;
//------------------------------------------------------------------------------

using namespace Common;

//! Trace d'info dans un fichier d'erreurs
void Common::Trace(const char *inStr, ...)
{
	// Trace off ?
	if (gIsTraceOff)
		return;

	const char *fileName = GetTraceFileName();

  // Ouvre le fichier trace
  FILE *f = fopen(fileName, "ab");
	if (!f) {
		//gLog.log(eTypLogError, "Err > Unable to open trace file: %s", fileName);
		return;
	}

	// Date et heure de trace ?
	if (!gNbTrace++) {
		std::string time = StdString::TimeToString();
    fprintf(f, "\r\n%s%s\r\n",
			"________________________________________",
			"________________________________________");
		fprintf(f, "%s\r\n\r\n", time.c_str());
	}

	// Entête de trace ?
	if (gTraceEntete != "") {
		fprintf(f, "%s\r\n", gTraceEntete.c_str());
		SetTraceEntete();
	}

	va_list ap;
	va_start(ap, inStr);
	vfprintf(f, inStr, ap);
	va_end(ap);
	fprintf(f, "\r\n");

	fclose(f);
}
//------------------------------------------------------------------------------

const char *Common::GetTraceFileName()
{
	gTraceFullFileName = gTracePath + '/' + gTraceFileName;

	return gTraceFullFileName.c_str();
}
//------------------------------------------------------------------------------

void Common::SetTraceFileName(const char *inName)
{
	gTraceFileName = inName && *inName != 0 ? inName : DEF_TraceFile;
}
//------------------------------------------------------------------------------

const char *Common::GetTracePath()
{
	return gTracePath.c_str();
}
//------------------------------------------------------------------------------

void Common::SetTracePath(const char *inPath /*= NULL*/)
{
	gTracePath = inPath ? inPath : "";
}
//------------------------------------------------------------------------------

void Common::SetTraceOff(bool inWantTraceOff)
{
	gIsTraceOff = inWantTraceOff;
}
//------------------------------------------------------------------------------

unsigned long Common::GetNbTrace()
{
	return gNbTrace;
}
//------------------------------------------------------------------------------

void Common::RazTrace()
{
	const char *fileName = GetTraceFileName();

	remove(fileName);
}
//------------------------------------------------------------------------------

void Common::SetTraceEntete(const char *inStr /*= NULL*/)
{
  if (inStr == NULL) {
    gTraceEntete = "";
    return;
  }

	gTraceEntete = inStr;
}
//------------------------------------------------------------------------------
