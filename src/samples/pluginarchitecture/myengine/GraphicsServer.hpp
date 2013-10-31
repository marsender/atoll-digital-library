/*******************************************************************************

 GraphicsServer.hpp

 Copyright © 2010 Didier Corbiere

 Distributable under the terms of the GNU Lesser General Public License,
 as specified in the COPYING file.

*******************************************************************************/

#ifndef MYENGINE_GRAPHICSSERVER_H
#define MYENGINE_GRAPHICSSERVER_H

#include "LibExport.hpp"
#include <vector>
#include <string>

namespace MyEngine
{

class Renderer {}; // Dummy

/// Manages graphics related stuff
class GraphicsServer
{
public:
	/// Engine graphics driver
	class GraphicsDriver
	{
	public:
		/// Destructor
		virtual ~GraphicsDriver() {}
		/// Get name of the graphics driver
		virtual const std::string &getName() const = 0;
		/// Create a renderer
		virtual std::auto_ptr<Renderer> createRenderer() = 0;
	};

	/// Destructor
	DEF_Export ~GraphicsServer() {
		for (GraphicsDriverVector::reverse_iterator It = m_GraphicsDrivers.rbegin();
		     It != m_GraphicsDrivers.rend(); ++It)
			delete *It;
	}

	/// Allows plugins to add new graphics drivers
	DEF_Export void addGraphicsDriver(std::auto_ptr<GraphicsDriver> GD) {
		m_GraphicsDrivers.push_back(GD.release());
	}

	/// Get the total number of registered graphics drivers
	DEF_Export size_t getDriverCount() const {
		return m_GraphicsDrivers.size();
	}
	/// Access a driver by its index
	DEF_Export GraphicsDriver &getDriver(size_t Index) {
		return *m_GraphicsDrivers.at(Index);
	}

private:
	/// A vector of graphics drivers
	typedef std::vector<GraphicsDriver *> GraphicsDriverVector;

	GraphicsDriverVector m_GraphicsDrivers; ///< All available graphics drivers
};

} // namespace MyEngine

#endif // MYENGINE_GRAPHICSSERVER_H
