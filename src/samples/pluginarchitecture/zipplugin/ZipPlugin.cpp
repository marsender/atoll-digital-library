/*******************************************************************************

 ZipPlugin.cpp

 Copyright © 2010 Didier Corbiere

 Distributable under the terms of the GNU Lesser General Public License,
 as specified in the COPYING file.

*******************************************************************************/

#include "../myengine/PluginKernel.hpp"
#include <stdexcept>

using namespace std;
using namespace MyEngine;

/// Zip archive reader
class ZipArchiveReader : public StorageServer::ArchiveReader
{
public:
	/// Destructor
	DEF_Export virtual ~ZipArchiveReader() {}

	/// Check whether the archive reader can open an archive
	DEF_Export bool canOpenArchive(const string &sFilename) {
		return sFilename.find(".zip") == (sFilename.length() - 4);
	}

	/// Open an archive for reading
	DEF_Export auto_ptr<Archive> archiveReaderOpen(const string &sFilename) {
		if (!canOpenArchive(sFilename))
			throw runtime_error("No Zip archive");

		return auto_ptr<Archive>(new Archive());
	}

	/// Get the number of files in the archive
	DEF_Export size_t archiveReaderCountFiles(Archive *inArchive) {
		if (inArchive == NULL)
			return 0;

		return 100; //inArchive->archiveCountFiles();
	}
};

/// Retrieve the engine version we're going to expect
extern "C" DEF_Export int getEngineVersion()
{
	return 1;
}

/// Tells us to register our functionality to an engine kernel
extern "C" DEF_Export void registerPlugin(Kernel &K)
{
	K.getStorageServer().addArchiveReader(
	  auto_ptr<StorageServer::ArchiveReader>(new ZipArchiveReader())
	);
}
