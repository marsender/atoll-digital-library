/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

PluginServer.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "PluginServer.hpp"
//------------------------------------------------------------------------------

using namespace AtollPlugin;
using namespace Common;
//------------------------------------------------------------------------------

PluginServer::PluginServer(Common::Logger &inLog)
	: mLog(inLog)
{
	mLog.log(eTypLogDebug, "Deb > PluginServer constructor");
}
//------------------------------------------------------------------------------

PluginServer::~PluginServer()
{
	mLog.log(eTypLogDebug, "Deb > PluginServer destructor");

	if (mPluginXmlVector.size()) {
		mLog.log(eTypLogError, "Err > Plugins should have been deleted before plugin server");
		DeletePlugins();
	}
}
//------------------------------------------------------------------------------

void PluginServer::AddPluginXml(PluginXml *inPlugin)
{
	mPluginXmlVector.push_back(inPlugin);
}
//------------------------------------------------------------------------------

void PluginServer::DeletePlugins()
{
	PluginXmlVector::const_reverse_iterator it = mPluginXmlVector.rbegin();
	PluginXmlVector::const_reverse_iterator itEnd = mPluginXmlVector.rend();
	for (; it != itEnd; ++it) {
		PluginXml *pluginXml(*it);
		delete pluginXml;
	}

	mPluginXmlVector.clear();
}
//------------------------------------------------------------------------------

PluginServer::PluginXml *PluginServer::GetPluginXmlExecBuffer()
{
	PluginXmlVector::const_iterator it = mPluginXmlVector.begin();
	PluginXmlVector::const_iterator itEnd = mPluginXmlVector.end();
	for (; it != itEnd; ++it) {
		PluginXml *pluginXml(*it);
		if (pluginXml->CanRunXmlExecBuffer())
			return pluginXml;
	}

	// Throws exception or return NULL
	/*
	mLog.log(eTypLogError, "Err > Invalid or unsupported xml buffer");
	throw Common::AppException(Common::AppException::InternalError, "Invalid or unsupported xml buffer");
	*/

	return NULL;
}
//------------------------------------------------------------------------------

PluginServer::PluginXml *PluginServer::GetPluginXmlExecFile(const std::string &inFileName)
{
	PluginXmlVector::const_iterator it = mPluginXmlVector.begin();
	PluginXmlVector::const_iterator itEnd = mPluginXmlVector.end();
	for (; it != itEnd; ++it) {
		PluginXml *pluginXml(*it);
		if (pluginXml->CanRunXmlExecFile(inFileName))
			return pluginXml;
	}

	return NULL;
}
//------------------------------------------------------------------------------

PluginServer::PluginXml *PluginServer::GetPluginXsltBuffer()
{
	PluginXmlVector::const_iterator it = mPluginXmlVector.begin();
	PluginXmlVector::const_iterator itEnd = mPluginXmlVector.end();
	for (; it != itEnd; ++it) {
		PluginXml *pluginXml(*it);
		if (pluginXml->CanRunXsltBuffer())
			return pluginXml;
	}

	return NULL;
}
//------------------------------------------------------------------------------
