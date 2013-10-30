/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

UnitTestSuites.hpp

*******************************************************************************/

#ifndef __UnitTestSuites_HPP
#define	__UnitTestSuites_HPP
//------------------------------------------------------------------------------

#include <string>
//------------------------------------------------------------------------------

namespace Atoll
{

//! Unit test suites configuration
/**
	Get the names of the test suites, and register which suites to run by default

	Logic:
		- Get the names of the test suites
		- Get some configuration info for the test suites
		- Register the test suites to run by default
*/
class UnitTestSuites
{
public:
	//! Get Adorner test suite name
	static std::string Adorner() { return "Adorner"; }
	//! Get Collection test suite name
	static std::string Collection() { return "Collection"; }
	//! Get Common test suite name
	static std::string Common() { return "Common"; }
	//! Get Core test suite name
	static std::string Core() { return "Core"; }
	//! Get Database test suite name
	static std::string Database() { return "Database"; }
	//! Get Engine test suite name
	static std::string Engine() { return "Engine"; }
	//! Get Indexer test suite name
	static std::string Indexer() { return "Indexer"; }
	//! Get Parser test suite name
	static std::string Parser() { return "Parser"; }
	//! Get Query test suite name
	static std::string Query() { return "Query"; }
	//! Get RecordBreaker test suite name
	static std::string RecordBreaker() { return "RecordBreaker"; }
	//! Get Tokenizer test suite name
	static std::string Tokenizer() { return "Tokenizer"; }
	//! Get Util test suite name
	static std::string Util() { return "Util"; }

	//! Get the database home directory
	static const char *GetDbHome() { return "./data_unittest/"; }
	//! Get the database collection name
	static const char *GetColId() { return "ColUnitTestSuites"; }
};
//------------------------------------------------------------------------------

} // namespace Atoll
//------------------------------------------------------------------------------

#endif
