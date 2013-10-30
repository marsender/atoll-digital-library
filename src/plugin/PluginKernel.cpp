/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

PluginKernel.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "PluginKernel.hpp"
//------------------------------------------------------------------------------

using namespace AtollPlugin;
using namespace Common;
//------------------------------------------------------------------------------

PluginKernel::PluginKernel(Common::Logger &inLog)
	: mLog(inLog),
	mPluginServer(inLog)
{
	mLog.log(eTypLogDebug, "Deb > PluginKernel constructor");
}
//------------------------------------------------------------------------------

PluginKernel::~PluginKernel()
{
	mLog.log(eTypLogDebug, "Deb > PluginKernel destructor");
}
//------------------------------------------------------------------------------

//! Loads a plugin
void PluginKernel::LoadPlugin(const std::string &inFileName) {
	if (mLoadedPlugins.find(inFileName) == mLoadedPlugins.end()) {
		Plugin &plugin = mLoadedPlugins.insert(PluginMap::value_type(inFileName,
			Plugin(mLog, inFileName))).first->second;
		plugin.RegisterPlugin(*this);
	}
}
//------------------------------------------------------------------------------

void PluginKernel::ReleasePlugins()
{
	GetPluginServer().DeletePlugins();
}
//------------------------------------------------------------------------------
