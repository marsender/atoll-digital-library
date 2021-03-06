/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

PluginMessage.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "PluginMessage.hpp"
//------------------------------------------------------------------------------

using namespace AtollPlugin;
//------------------------------------------------------------------------------

PluginMessage::PluginMessage()
{
	mIsOk = false;
}
//------------------------------------------------------------------------------

PluginMessage::~PluginMessage()
{
	// Desctructor is useless but allows to trace when object is deleted
	mIsOk = false;
}
//------------------------------------------------------------------------------
