/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

UnitTest.hpp

*******************************************************************************/

#ifndef __UnitTest_HPP
#define	__UnitTest_HPP
//------------------------------------------------------------------------------

#include <string>
//------------------------------------------------------------------------------

namespace Common
{

//! Run all CppUnit tests
/**
	The tests to run are automatically registred via the test suites macros

	Logic:
		- Run all CppUnit tests
			(the test classes are automatically registred via the test suites macros)
		- BasicTest class allows to try new tests
		- Generate an xml report with stylesheet
		- Opens the xml report automaticaly in case of test failure
*/
int RunUnitTest();

//------------------------------------------------------------------------------

} // namespace Common
//------------------------------------------------------------------------------

#endif
