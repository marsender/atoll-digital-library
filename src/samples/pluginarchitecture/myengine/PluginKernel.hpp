/*******************************************************************************

 PluginKernel.hpp

 Copyright © 2010 Didier Corbiere

 Distributable under the terms of the GNU Lesser General Public License,
 as specified in the COPYING file.

*******************************************************************************/

#ifndef MYENGINE_KERNEL_H
#define MYENGINE_KERNEL_H

#include "LibExport.hpp"
#include "storage_server.hpp"
#include "graphics_server.hpp"
#include "Plugin.hpp"

#include <string>
#include <map>

namespace MyEngine
{

/// The engine's system core
class Kernel
{
public:
	/// Access the storage server
	DEF_Export StorageServer &getStorageServer() {
		return m_StorageServer;
	}
	/// Access the graphics server
	DEF_Export GraphicsServer &getGraphicsServer() {
		return m_GraphicsServer;
	}

	/// Loads a plugin
	DEF_Export void loadPlugin(const std::string &sFilename) {
		if (m_LoadedPlugins.find(sFilename) == m_LoadedPlugins.end())
			m_LoadedPlugins.insert(PluginMap::value_type(
			                         sFilename,
			                         Plugin(sFilename)
			                       )).first->second.registerPlugin(*this);
	}

private:
	/// Map of plugins by their associated file names
	typedef std::map<std::string, Plugin> PluginMap;

	PluginMap      m_LoadedPlugins;  ///< All plugins currently loaded
	StorageServer  m_StorageServer;  ///< The storage server
	GraphicsServer m_GraphicsServer; ///< The graphics server
};

} // namespace MyEngine

#endif // MYENGINE_KERNEL_H
