/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

PluginMessage.hpp

*******************************************************************************/

#ifndef __PluginMessage_HPP
#define __PluginMessage_HPP
//------------------------------------------------------------------------------

#include "../common/LibExport.hpp"
#include "unicode/unistr.h"
using icu::UnicodeString;
//------------------------------------------------------------------------------

namespace AtollPlugin
{

//! Message to allow communication beween the server and the plugin
class DEF_Export PluginMessage {
public:
	bool mIsOk;
	UnicodeString mMessage;
	UnicodeString mError;

	PluginMessage();
	~PluginMessage();
};

} // namespace AtollPlugin

//------------------------------------------------------------------------------
#endif
