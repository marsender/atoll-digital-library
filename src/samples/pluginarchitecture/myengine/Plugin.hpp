/*******************************************************************************

 Plugin.hpp

 Copyright © 2010 Didier Corbiere

 Distributable under the terms of the GNU Lesser General Public License,
 as specified in the COPYING file.

*******************************************************************************/

#ifndef MYENGINE_PLUGIN_H
#define MYENGINE_PLUGIN_H

#include "LibExport.hpp"
#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace MyEngine
{

class Kernel;

/// Representation of a plugin
class Plugin
{
public:
	/// Initialize and load plugin
	DEF_Export Plugin(const std::string &sFilename);
	/// Copy existing plugin instance
	DEF_Export Plugin(const Plugin &Other);
	/// Unload a plugin
	DEF_Export ~Plugin();

	//
	// Plugin implementation
	//
public:
	/// Query the plugin for its expected engine version
	DEF_Export int getEngineVersion() const {
		return m_pfnGetEngineVersion();
	}

	/// Register the plugin to a kernel
	DEF_Export void registerPlugin(Kernel &K) {
		m_pfnRegisterPlugin(K);
	}

private:
	/// Too lazy to this now...
	Plugin &operator =(const Plugin &Other);

	/// Signature for the version query function
	typedef int fnGetEngineVersion();
	/// Signature for the plugin's registration function
	typedef void fnRegisterPlugin(Kernel &);

#ifdef _WIN32
	HMODULE             m_hDLL;                ///< Win32 DLL handle
#else
	void                *m_hDLL;
#endif
	size_t             *m_pDLLRefCount;        ///< Number of references to the DLL
	fnGetEngineVersion *m_pfnGetEngineVersion; ///< Version query function
	fnRegisterPlugin   *m_pfnRegisterPlugin;   ///< Plugin registration function
};

} // namespace MyEngine

#endif // MYENGINE_PLUGIN_H
