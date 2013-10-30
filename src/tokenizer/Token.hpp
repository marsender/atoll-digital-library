/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Token.hpp

*******************************************************************************/

#ifndef __Token_HPP
#define __Token_HPP
//------------------------------------------------------------------------------

#include <string>
#include "unicode/unistr.h"
//------------------------------------------------------------------------------

namespace Atoll
{

enum eTypToken {
	eTypTokenNone,
	eTypTokenDefault,         //!< Content
	eTypTokenNote,            //!< Note
	eTypTokenNli,             //!< Line number
	eTypTokenEnd
};
 
//! String token
class Token
{
private:
	eTypToken mTypToken;

public:
	Token(const std::string &inElem);

	static eTypToken StaticGetTypToken(const std::string &inElem);
};

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
