/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Plugin.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "Plugin.hpp"
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#else
	#include <dlfcn.h>
#endif

using namespace Common;
using namespace AtollPlugin;
//------------------------------------------------------------------------------

Plugin::Plugin(Common::Logger &inLog, const std::string &inFileName)
	: mLog(inLog)
{
	mRefCount = 0;
	mLibHandle = NULL;
	mFuncionRegisterPlugin = NULL;
	mFuncionGetEngineExpectedVersion = NULL;

	// Try to load the plugin as a dynamic library
	std::string sLibname;
#if defined(WIN32)
	sLibname = inFileName + ".dll";
	mLibHandle = ::LoadLibraryA(sLibname.c_str());
#else
	sLibname = std::string("lib") + inFileName + ".so";
	mLibHandle = ::dlopen(sLibname.c_str(), RTLD_LAZY);
#endif
	if (!mLibHandle) {
		std::string error("Could not load '");
		error += sLibname + std::string("'");
		throw Common::AppException(Common::AppException::InternalError, error);
	}

	//mLog.log(eTypLogAction, "Act > Plugin load library %p: %s", mLibHandle, inFileName.c_str());

	// Locate the plugin's exported functions
	try {
		mFuncionGetEngineExpectedVersion = reinterpret_cast<fnGetEngineExpectedVersion *>(
#if defined(WIN32)
			::GetProcAddress((HMODULE)mLibHandle, "GetEngineExpectedVersion"));
#else
			(fnGetEngineExpectedVersion *)::dlsym(mLibHandle, "GetEngineExpectedVersion"));
#endif

		mFuncionRegisterPlugin = reinterpret_cast<fnRegisterPlugin *>(
#if defined(WIN32)
			::GetProcAddress((HMODULE)mLibHandle, "RegisterPlugin"));
#else
			(fnRegisterPlugin *)::dlsym(mLibHandle, "RegisterPlugin"));
#endif

		// If the functions aren't found, we're going to assume this is
		// a plain simple library and not one of our plugins
		if (!mFuncionGetEngineExpectedVersion || !mFuncionRegisterPlugin) {
			std::string error("'");
			error += sLibname + std::string("' is not a valid plugin");
			throw Common::AppException(Common::AppException::InternalError, error);
		}

		if (GetEngineExpectedVersion() != DEF_PluginVersion) {
			std::string error("'");
			error += sLibname + std::string("' plugin expected version of engine is incorrect");
			throw Common::AppException(Common::AppException::InternalError, error);
		}

		// Create and initialize a new library reference counter
		mRefCount = new size_t(1);
	}
	catch (...) {
#if defined(WIN32)
		::FreeLibrary((HMODULE)mLibHandle);
#else
		::dlclose(mLibHandle);
#endif
		throw;
	}
}
//------------------------------------------------------------------------------

Plugin::Plugin(const Plugin &inPlugin)
	:mLog(inPlugin.mLog)
{
	mLibHandle = inPlugin.mLibHandle;
	mFuncionGetEngineExpectedVersion = inPlugin.mFuncionGetEngineExpectedVersion;
	mFuncionRegisterPlugin = inPlugin.mFuncionRegisterPlugin;
	mRefCount = inPlugin.mRefCount;

	//! Increase library reference counter
	++*mRefCount;
}
//------------------------------------------------------------------------------

Plugin::~Plugin()
{
	// Only unload the library if there are no more references to it
	if (--*mRefCount > 0) {
		//mLog.log(eTypLogAction, "Act > Plugin library %p ref count %d", mLibHandle, *mRefCount);
		return;
	}

	//mLog.log(eTypLogAction, "Act > Plugin unload library %p", mLibHandle);

	delete mRefCount;
#if defined(WIN32)
	::FreeLibrary((HMODULE)mLibHandle);
#else
	::dlclose(mLibHandle);
#endif
}
//------------------------------------------------------------------------------
