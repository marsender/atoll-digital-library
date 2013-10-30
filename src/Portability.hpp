/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Portability.hpp

*******************************************************************************/

#ifndef __Portability_HPP
#define	__Portability_HPP
//------------------------------------------------------------------------------

// Keep compilers quiet about unused parameters
#define	UNUSED(x) static_cast<void>(&(x))

#if defined(_MSC_VER)
// versions check:
// we don't support Visual C++ prior to version 6:
#if (_MSC_VER<1200)
#error "Compiler not supported"
#endif // _MSC_VER<1200

// MSVC requires explicit exportation of symbols accessed in a DLL
// If we are building the library, export symbols
// If we are linking against the library, import symbols

// In order to turn off the link warning
// LINK : warning LNK4089: all references to "icuio32.dll" discarded by /OPT:REF
// => Add /ignore:4089 in the linking options

// turn off the warnings before we #include anything
#pragma warning(disable : 4786) // ident trunc to '255' chars in debug info
#pragma warning(disable : 4996) // warning: deprecation of strdup() in VS8
#pragma warning(disable : 4800) // warning: forcing bool variable to one or zero
#pragma warning(disable : 4251) // warning: class 'std::vector<_Ty>' nécessite une interface DLL pour être utilisé(e) par les clients de class
/*
#pragma warning(disable : 4503) // warning: decorated name length exceeded
#pragma warning(disable : 4290) // warning: C++ exception specification ignored
//disable warnings on extern before template instantiation
#pragma warning(disable : 4231)
*/

#else // !_MSC_VER

// Building on a non-Windows platform.

#endif // !_MSC_VER

// Program headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string> // std::string
//#include <iostream>
//#include <sstream>
//#ifndef WIN32
//	#include <limits.h> // for ULONG_MAX
//#endif
#include "common/AppAssert.hpp"
#include "common/AppException.hpp"
#include "common/BufUtil.hpp"
#include "common/FileSystem.hpp"
#include "common/Logger.hpp"
#include "common/Trace.hpp"

//------------------------------------------------------------------------------
#endif
