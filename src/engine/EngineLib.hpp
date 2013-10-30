/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

EngineLib.hpp

*******************************************************************************/

#ifndef __EngineLib_HPP
#define __EngineLib_HPP
//------------------------------------------------------------------------------

#include "../common/LibExport.hpp"
//------------------------------------------------------------------------------

//! External functions available for the php extension
#if defined(WIN32)
  extern "C"
  {
    DEF_Export void AtollLoadLib();
    DEF_Export void AtollFreeLib();
    DEF_Export char *AtollXmlExec(const char *inStr);
    DEF_Export void AtollXmlFree(char *inStr);
    DEF_Export const char *AtollVersion();
  }
#else
  extern "C"
  {
    void DEF_Export AtollLoadLib();
    void DEF_Export AtollFreeLib();
    char *DEF_Export AtollXmlExec(const char *inStr);
    void DEF_Export AtollXmlFree(char *inStr);
    const char *DEF_Export AtollVersion();
  }
#endif

//------------------------------------------------------------------------------
#endif
