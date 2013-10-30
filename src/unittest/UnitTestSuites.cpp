/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

UnitTestSuites.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "UnitTestSuites.hpp"
#include <cppunit/extensions/HelperMacros.h>
//------------------------------------------------------------------------------

// Register the test suites to run by default
CPPUNIT_REGISTRY_ADD_TO_DEFAULT(Atoll::UnitTestSuites::Adorner());
CPPUNIT_REGISTRY_ADD_TO_DEFAULT(Atoll::UnitTestSuites::Collection());
CPPUNIT_REGISTRY_ADD_TO_DEFAULT(Atoll::UnitTestSuites::Common());
CPPUNIT_REGISTRY_ADD_TO_DEFAULT(Atoll::UnitTestSuites::Core());
CPPUNIT_REGISTRY_ADD_TO_DEFAULT(Atoll::UnitTestSuites::Database());
CPPUNIT_REGISTRY_ADD_TO_DEFAULT(Atoll::UnitTestSuites::Engine());
CPPUNIT_REGISTRY_ADD_TO_DEFAULT(Atoll::UnitTestSuites::Indexer());
CPPUNIT_REGISTRY_ADD_TO_DEFAULT(Atoll::UnitTestSuites::Parser());
CPPUNIT_REGISTRY_ADD_TO_DEFAULT(Atoll::UnitTestSuites::Query());
CPPUNIT_REGISTRY_ADD_TO_DEFAULT(Atoll::UnitTestSuites::RecordBreaker());
CPPUNIT_REGISTRY_ADD_TO_DEFAULT(Atoll::UnitTestSuites::Tokenizer());
CPPUNIT_REGISTRY_ADD_TO_DEFAULT(Atoll::UnitTestSuites::Util());
