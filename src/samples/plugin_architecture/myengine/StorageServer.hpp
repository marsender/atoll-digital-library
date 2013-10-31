/*******************************************************************************

 storage_server.hpp

 Copyright © 2010 Didier Corbiere

 Distributable under the terms of the GNU Lesser General Public License,
 as specified in the COPYING file.

*******************************************************************************/

#ifndef MYENGINE_STORAGE_SERVER_H
#define MYENGINE_STORAGE_SERVER_H

#include "LibExport.hpp"
#include <list>
#include <string>
#include <stdexcept>
#include <memory> // for std::auto_ptr

namespace MyEngine
{

class Archive {}; // Dummy

/// Manages storage related stuff
class StorageServer
{
public:
	/// File archive reader
	class ArchiveReader
	{
	public:
		/// Destructor
		virtual ~ArchiveReader() {}

		/// Check whether the archive reader can open an archive
		virtual bool canOpenArchive(const std::string &sFilename) = 0;
		/// Open an archive for reading
		virtual std::auto_ptr<Archive> archiveReaderOpen(const std::string &sFilename) = 0;
		/// Get the number of files in the archive
		virtual size_t archiveReaderCountFiles(Archive *inArchive) = 0;
	};

	/// Destructor
	DEF_Export ~StorageServer() {
		for (ArchiveReaderList::reverse_iterator It = m_ArchiveReaders.rbegin();
		     It != m_ArchiveReaders.rend(); ++It) {
			delete *It;
		}
	}

	/// Allows plugins to add new archive readers
	DEF_Export void addArchiveReader(std::auto_ptr<ArchiveReader> AR) {
		m_ArchiveReaders.push_back(AR.release());
	}

	/// Open archive by searching for a matching archive reader
	DEF_Export std::auto_ptr<Archive> openArchive(const std::string &sFilename) {
		for (ArchiveReaderList::iterator It = m_ArchiveReaders.begin();
		     It != m_ArchiveReaders.end(); ++It) {
			if ((*It)->canOpenArchive(sFilename))
				return (*It)->archiveReaderOpen(sFilename);
		}

		throw std::runtime_error("Invalid or unsupported archive type");
	}

	/// Search for a matching archive reader
	DEF_Export ArchiveReader *getArchiveReader(const std::string &sFilename) {
		for (ArchiveReaderList::iterator It = m_ArchiveReaders.begin();
		     It != m_ArchiveReaders.end(); ++It) {
			if ((*It)->canOpenArchive(sFilename))
				return *It;
		}

		// Throws exception or return NULL
		throw std::runtime_error("Invalid or unsupported archive type");
		// return NULL;
	}

private:
	/// List of archive readers
	typedef std::list<ArchiveReader *> ArchiveReaderList;

	ArchiveReaderList m_ArchiveReaders; ///< All available archive readers
};

} // namespace MyEngine

#endif // MYENGINE_STORAGE_SERVER_H
