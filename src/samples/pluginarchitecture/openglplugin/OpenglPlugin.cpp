/*******************************************************************************

 OpenglPlugin.cpp

 Copyright © 2010 Didier Corbiere

 Distributable under the terms of the GNU Lesser General Public License,
 as specified in the COPYING file.

*******************************************************************************/

#include "../myengine/PluginKernel.hpp"

using namespace std;
using namespace MyEngine;

/// OpenGL graphics drver
class OpenGLGraphicsDriver : public GraphicsServer::GraphicsDriver
{
public:
	/// Destructor
	DEF_Export virtual ~OpenGLGraphicsDriver() {}

	/// Get name of the graphics driver
	DEF_Export virtual const std::string &getName() const {
		static string sName("OpenGL graphics driver");
		return sName;
	}

	/// Create a renderer
	auto_ptr<Renderer> createRenderer() {
		return auto_ptr<Renderer>(new Renderer());
	}
};

/// Retrieve the engine version we're going to expect
extern "C" DEF_Export int getEngineVersion()
{
	return 1;
}

/// Tells us to register our functionality to an engine kernel
extern "C" DEF_Export void registerPlugin(Kernel &K)
{
	K.getGraphicsServer().addGraphicsDriver(
	  auto_ptr<GraphicsServer::GraphicsDriver>(new OpenGLGraphicsDriver())
	);
}
