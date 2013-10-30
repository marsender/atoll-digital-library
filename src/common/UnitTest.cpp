/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

UnitTest.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "UnitTest.hpp"
//#ifdef WIN32
//	#include <windows.h> // ShellExecute (requires shell32.lib) shellapi.h ?
//#endif
#include <cppunit/TestRunner.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/XmlOutputter.h>
//------------------------------------------------------------------------------

int Common::RunUnitTest()
{
	// Create the event manager and test controller
	CPPUNIT_NS::TestResult controller;

	// Add a listener that colllects test result
	CPPUNIT_NS::TestResultCollector result;
	controller.addListener(&result);

	// Add a listener that print info as test run.
	//CPPUNIT_NS::TextTestProgressListener progress; // Prints dots
	CPPUNIT_NS::BriefTestProgressListener progress; // Prints current test name
	controller.addListener(&progress);

	// Add the top suite to the test runner
	CPPUNIT_NS::TestRunner runner;
	CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
	runner.addTest(registry.makeTest());

	try
	{
		CPPUNIT_NS::stdCOut() << "*** UnitTest Begin ***" << std::endl;

		// Run tests
		runner.run(controller);

		if (result.wasSuccessful())
			CPPUNIT_NS::stdCOut() << "*** UniTest Ok ***" << std::endl;
		else {
			CPPUNIT_NS::stdCOut() << "*** UniTest Failure ***" << std::endl;
		}

		// Output XML report
		std::string xmlFileName("cppunit_report.xml");
		std::ofstream file(xmlFileName.c_str());
		CPPUNIT_NS::XmlOutputter xml(&result, file);
		xml.setStyleSheet("xsl/cppunit_report.xsl");
		xml.write();
		file.close();

		// Ouvre le fichier si erreur
#ifdef _WIN32
		if (!result.wasSuccessful()) {
			// int ret = (int)ShellExecute(NULL, "open", xmlFileName.c_str(), NULL, NULL, SW_SHOWNORMAL);
		}
#endif
	}
	catch (std::exception &e) {
		CPPUNIT_NS::stdCOut() << "\n" << "Unit tests exception: " << e.what() << std::endl;
		return 1;
	}

	return result.wasSuccessful() ? 0 : 1;
}
//------------------------------------------------------------------------------
