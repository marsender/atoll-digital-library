/*******************************************************************************

 myapplication.cpp

 Copyright © 2010 Didier Corbiere

 Distributable under the terms of the GNU Lesser General Public License,
 as specified in the COPYING file.

*******************************************************************************/

#include "../myengine/PluginKernel.hpp"
#include "../myengine/Engine.hpp"
#include <iostream>
#include <stdexcept>
#ifdef _WIN32
#include "vld.h" // Visual Leak Detector (Memory leak detection)
#endif

using namespace std;
//using namespace MyEngine;

/// Test which tries to open a zip archive
std::auto_ptr<MyEngine::Archive> tryOpenArchive(MyEngine::StorageServer &SS)
{
	std::auto_ptr<MyEngine::Archive> archive;

	try {
		archive = SS.openArchive("some_archive.zip");

		cout << "\tArchive successfully opened" << endl;

		return archive;
	}
	catch (const exception &Ex) {
		cout << "\tError opening archive: " << Ex.what() << endl;
	}

	cout << endl;

	return archive;
}

/// Test which lists all available graphics drivers
void listGraphicsDrivers(MyEngine::GraphicsServer &GS)
{
	size_t count = GS.getDriverCount();
	for (size_t DriverIndex = 0; DriverIndex < count; ++DriverIndex)
		cout << "\t" << GS.getDriver(DriverIndex).getName() << endl;

	if (count == 0)
		cout << "\t" << "None" << endl;

	cout << endl;
}

/// Program entry point
int main()
{
	MyEngine::Kernel TheKernel;
	MyEngine::StorageServer &SS = TheKernel.getStorageServer();
	MyEngine::GraphicsServer &GS = TheKernel.getGraphicsServer();
	std::auto_ptr<MyEngine::Archive> archive;

	try {
		// Try to open the archive without the required plugins
		cout << "Trying to open archive before plugins were loaded..." << endl;
		archive = tryOpenArchive(SS);
		if (archive.get()) {
			cout << "No archive opened..." << endl;
		}
		cout << "Available graphics drivers:" << endl;
		listGraphicsDrivers(GS);

		// Now load the plugins
		cout << "Loading plugins..." << endl;
		TheKernel.loadPlugin("OpenglPlugin");
		TheKernel.loadPlugin("ZipPlugin");
		cout << endl;

		// Try the same again, this time the plugins are loaded
		cout << "Trying to open archive with plugins loaded..." << endl;
		archive = tryOpenArchive(SS);
		if (archive.get()) {
			size_t count = 0;
			MyEngine::StorageServer::ArchiveReader *ar = SS.getArchiveReader("anocher_archive.zip");
			if (ar)
				count = ar->archiveReaderCountFiles(archive.get());
			cout << "\tArchive contains " << count << " files" << endl;
		}
		cout << "Available graphics drivers:" << endl;
		listGraphicsDrivers(GS);

		MyEngine::Engine engine("bip");
	}
	catch (const exception &Ex) {
		cout << "Unhandled error: " << Ex.what() << endl;
	}
}
