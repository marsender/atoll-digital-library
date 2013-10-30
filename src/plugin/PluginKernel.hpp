/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

PluginKernel.hpp

*******************************************************************************/

#ifndef __PluginKernel_HPP
#define __PluginKernel_HPP

#include "../common/LibExport.hpp"
#include "PluginServer.hpp"
#include "Plugin.hpp"
#include <string>
#include <map>
//------------------------------------------------------------------------------

namespace AtollPlugin
{

class PluginServer;

//! Plugin system core that encapsulate the plugin server and loads the plugin libraries
class PluginKernel
{
public:
	DEF_Export PluginKernel(Common::Logger &inLog);
	~PluginKernel();

	//! Access the storage server
	DEF_Export PluginServer &GetPluginServer() {
		return mPluginServer;
	}
	DEF_Export Common::Logger &GetLogger() {
		return mLog;
	}

	//! Load a plugin by it's library name
	DEF_Export void LoadPlugin(const std::string &inFileName);
	//! Release all the plugins
	DEF_Export void ReleasePlugins();

private:
	//! Map of plugins by their associated file names
	typedef std::map<std::string, Plugin> PluginMap;

	PluginMap mLoadedPlugins; //!< All plugins currently loaded
	Common::Logger &mLog;
	PluginServer mPluginServer; //!< The plugin server
};

} // namespace AtollPlugin

//------------------------------------------------------------------------------
#endif
