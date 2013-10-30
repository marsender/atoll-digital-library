/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

AppAssert.cpp

*******************************************************************************/

#ifdef DEF_EnableAsserts

#include "AppAssert.hpp"
//#include <assert.h> 
//#include <cassert>
#include <string>
#include "AppException.hpp"
//------------------------------------------------------------------------------

void Common::AssertFail(const char *inExpression, const char *inFile, int inLine)
{
	std::string error("Assertion failed: ");
	error += inExpression;

	throw Common::AppException(Common::AppException::AssertionError, error, inFile, inLine);
}

void Common::AssertFailMessage(const char *inExpression, const char *inMessage, const char *inFile, int inLine)
{
	std::string error("Assertion failed [");
	if (inMessage) {
		error += inMessage;
		error += "]: ";
	}
	error += inExpression;

	throw Common::AppException(Common::AppException::AssertionError, error, inFile, inLine);
}

//------------------------------------------------------------------------------
#endif // DEF_EnableAsserts
