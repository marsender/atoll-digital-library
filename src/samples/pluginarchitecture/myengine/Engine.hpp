/*******************************************************************************

 Engine.hpp

 Copyright © 2010 Didier Corbiere

 Distributable under the terms of the GNU Lesser General Public License,
 as specified in the COPYING file.

*******************************************************************************/

#ifndef __Engine_HPP
#define __Engine_HPP
//------------------------------------------------------------------------------

#include "LibExport.hpp"
#include <string>
//------------------------------------------------------------------------------

namespace MyEngine
{

class Engine
{
public:
	DEF_Export Engine(const char *inStr);

	int Run(const std::string &inStr);
};

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
