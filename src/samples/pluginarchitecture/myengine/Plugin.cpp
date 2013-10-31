/*******************************************************************************

 Plugin.cpp

 Copyright © 2010 Didier Corbiere

 Distributable under the terms of the GNU Lesser General Public License,
 as specified in the COPYING file.

*******************************************************************************/

#include "Plugin.hpp"
#include <stdexcept>
#ifndef _WIN32
	#include <dlfcn.h>
#endif

using namespace std;
using namespace MyEngine;

/** Loads the specified plugin as a dynamic library and locates
    the plugin's exported functions

    @param  sFilename  Filename of the plugin to load
*/
Plugin::Plugin(const std::string &sFilename) :
		m_hDLL(0),
		m_pDLLRefCount(0),
		m_pfnGetEngineVersion(0),
		m_pfnRegisterPlugin(0)
{
	// Try to load the plugin as a dynamic library
	std::string sLibname;
#if defined(WIN32)
	sLibname = sFilename + ".dll";
	m_hDLL = ::LoadLibrary(sLibname.c_str());
#else
	sLibname = string("lib") + sFilename + ".so";
	m_hDLL = ::dlopen(sLibname.c_str(), RTLD_LAZY);
#endif
	if (!m_hDLL)
		throw runtime_error(string("Could not load '") + sLibname + "'");

	// Locate the plugin's exported functions
	try {
		m_pfnGetEngineVersion = reinterpret_cast<fnGetEngineVersion *>(
#if defined(WIN32)
		                          ::GetProcAddress(m_hDLL, "getEngineVersion")
#else
		                          (fnGetEngineVersion *)::dlsym(m_hDLL, "getEngineVersion")
#endif
		                        );
		m_pfnRegisterPlugin = reinterpret_cast<fnRegisterPlugin *>(
#if defined(WIN32)
															::GetProcAddress(m_hDLL, "registerPlugin")
#else
		                          (fnRegisterPlugin *)::dlsym(m_hDLL, "registerPlugin")
#endif
														);

		// If the functions aren't found, we're going to assume this is
		// a plain simple DLL and not one of our plugins
		if (!m_pfnGetEngineVersion || ! m_pfnRegisterPlugin)
			throw runtime_error(string("'") + sLibname + "' is not a valid plugin");

		// Initialize a new DLL reference counter
		m_pDLLRefCount = new size_t(1);
	}
	catch (...) {
#if defined(WIN32)
		::FreeLibrary(m_hDLL);
#else
		::dlclose(m_hDLL);
#endif
		throw;
	}
}

/** Creates a copy of a plugin that has already been loaded.
    Required to provide correct semantics for storing plugins in
    an STL map container.

    @param  Other  Other plugin instance to copy
*/
Plugin::Plugin(const Plugin &Other) :
		m_hDLL(Other.m_hDLL),
		m_pDLLRefCount(Other.m_pDLLRefCount),
		m_pfnGetEngineVersion(Other.m_pfnGetEngineVersion),
		m_pfnRegisterPlugin(Other.m_pfnRegisterPlugin)
{
	/// Increase DLL reference counter
	++*m_pDLLRefCount;
}

/** Destroys the plugin, unloading its library when no more references
    to it exist.
*/
Plugin::~Plugin()
{
	// Only unload the DLL if there are no more references to it
	if (!--*m_pDLLRefCount) {
		delete m_pDLLRefCount;
#if defined(WIN32)
		::FreeLibrary(m_hDLL);
#else
		::dlclose(m_hDLL);
#endif
	}
}
