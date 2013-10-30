/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Plugin.hpp

*******************************************************************************/

#ifndef __Plugin_HPP
#define __Plugin_HPP
//------------------------------------------------------------------------------

#include "../common/LibExport.hpp"
#include <string>

#define DEF_PluginVersion 1
//------------------------------------------------------------------------------

namespace Common
{
	class Logger;
}

namespace AtollPlugin
{

class PluginKernel;

//! Wrapper class, to load and release the plugin dynamic library
class Plugin
{
public:
	//! Loads the specified plugin as a dynamic library and locates the plugin's exported functions
	DEF_Export Plugin(Common::Logger &inLog, const std::string &inFileName);
	//! Copy an existing plugin instance. Required to provide correct semantics for storing plugins in an STL map container.
	DEF_Export Plugin(const Plugin &inPlugin);
	//! Destroys the plugin, unloading its library when no more references to it exist
	DEF_Export ~Plugin();

public:
	//! Query the plugin for its expected engine version
	DEF_Export int GetEngineExpectedVersion() const {
		return mFuncionGetEngineExpectedVersion();
	}

	//! Register the plugin to a kernel
	DEF_Export void RegisterPlugin(PluginKernel &inPluginKernel) {
		mFuncionRegisterPlugin(inPluginKernel);
	}

private:
	//! Assignment operator. Required to provide correct semantics for storing plugins in an STL map container.
	Plugin &operator=(const Plugin &Other);

	//! Signature for the engine version query function
	typedef int fnGetEngineExpectedVersion();
	//! Signature for the plugin's registration function
	typedef void fnRegisterPlugin(PluginKernel &inPluginKernel);

	void *mLibHandle; //!< Library handle
	size_t *mRefCount; //!< Number of references to the library
	fnGetEngineExpectedVersion *mFuncionGetEngineExpectedVersion; //!< Version query function
	fnRegisterPlugin *mFuncionRegisterPlugin; //!< Plugin registration function
	Common::Logger &mLog;
};

} // namespace AtollPlugin

//------------------------------------------------------------------------------
#endif
