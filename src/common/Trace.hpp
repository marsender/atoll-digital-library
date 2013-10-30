/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Trace.hpp

*******************************************************************************/

#ifndef __Trace_HPP
#define __Trace_HPP
//------------------------------------------------------------------------------

#include "LibExport.hpp"

namespace Common
{

//! Trace information using printf syntax
//! Example: Trace("Debug %s: %s", str.c_str(), object.ToString().c_str());
DEF_Export void Trace(const char *inStr, ...);

const char *GetTraceFileName();

//! Init trace file name (ie trace.log)
void SetTraceFileName(const char *inName);
//! Get trace path
const char *GetTracePath();
//! Init trace path (ie ./test)
void SetTracePath(const char *inPath = 0);

//! Disable trace
void SetTraceOff(bool inWantTraceOff);
unsigned long GetNbTrace();
//! Store a trace header
void SetTraceEntete(const char *inStr = 0);
//! Remove trace file
void RazTrace();

} // namespace Common

//------------------------------------------------------------------------------
#endif
