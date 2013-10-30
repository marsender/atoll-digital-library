/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

FileSystem.hpp

*******************************************************************************/

#ifndef __FileSystem_HPP
#define __FileSystem_HPP
//------------------------------------------------------------------------------

#include "LibExport.hpp"
#include <set>
#include <string>
//------------------------------------------------------------------------------

#define DEF_DirectorySeparator '/'
//------------------------------------------------------------------------------

namespace Common
{

//! Static file system helper class
class DEF_Export FileSystem
{
private:
	//! Shell pattern string comparison
	/**
		Recursively compare the shell pattern inMsk with the string inStr and return
		true if they match, false if they don't or if there is a syntax error in the
		pattern. This routine recurses on itself no more deeply than the number
		of characters in the pattern.
	 */
	static bool PatternMatch(const char *inStr, const char *inMsk, bool inIsCase);
	//! Standard string Compare function
	struct fCmpStdString : std::binary_function<std::string, std::string, bool>
	{
		// String Compare function
		bool operator() (const std::string &s1, const std::string &s2) const;
	};

public:
	//! std::string Set
	typedef std::set < std::string, fCmpStdString, std::allocator<std::string> > FileSet;

	//! Attempts to create a directory
	/**
		Warning: returns false if the directory already exists
	 */
	static bool CreateNewDirectory(const std::string &inPath);
	//! Clear all the files in a directory
	static bool ClearDirectoryContent(const std::string &inPath, const char *inMsk = NULL);
	//! Remove an empty directory
	static bool RemoveEmptyDirectory(const std::string &inPath);
	static bool DirectoryExists(const std::string &inPath);
	//! Get the list of files in a directory
	/**
		The list can be filtred with a file matching specification.
		Examples: *.xml, utf8_* or NULL to get everything
		If case is true, the file / filter comparison is case sensitive
		If recurs is true, the sub folders are explored
		If get dir is true, only the directories are added to the list
	 */
	static void GetDirectoryFiles(FileSet &outFileSet, const char *inPath,
		const char *inMsk, bool inIsCase = true, bool inRecurs = false,
		bool inGetDir = false);

	//! Create an empty file
	static bool CreateEmptyFile(const std::string &inFullFileName);
	//! Test if a file exists
	static bool FileExists(const std::string &inFullFileName);
	//! Remove a file
	static bool RemoveFile(const std::string &inFullFileName);
	//! Binary compare two files
	static bool BinaryCompareFiles(const std::string &inFullFileName1, const std::string &inFullFileName2);
	//! Text compare two files
	/**
	  Test will fail if source file is not encoded in utf-8 or has a different CrLf as the plateform
	 */
	static bool TextCompareFiles(const std::string &inFullFileName1, const std::string &inFullFileName2);

	//! Extract the path from a full name. Eg /home/usr for /home/usr/test.txt
	static std::string GetPath(const std::string &inFullFileName);
	//! Extract the filename from a full name. Eg test.txt for /home/usr/test.txt
	static std::string GetFileName(const std::string &inFullFileName);
	//! Extract the filename without extension. Eg test for /home/usr/test.txt
	static std::string GetFileNameWithoutExt(const std::string &inFullFileName);
	//! Extract the extension from a full name. Eg txt for test.txt
	static std::string GetExtension(const std::string &inFullFileName);
	//! Set the extension in a full name (or change the extension)
	static std::string SetExtension(const std::string &inFullFileName, const std::string &inExt);
	//! Get the full file name from separate path, file name and extension
	static std::string MakeFullFileName(const std::string &inPath,
		const std::string &inFileName, const std::string &inExt);
	//! Get the full file name from separate path and file name
	static std::string MakeFullFileName(const std::string &inPath, const std::string &inFileName);
	//! Set an uniform directory separator
	static std::string SetDirectorySeparator(const std::string &inPath);
	//! Set platform directory separator (eg \ for windows and / for linux)
	static std::string SetPlatformDirectorySeparator(const std::string &inPath);
};

} // namespace Common

//------------------------------------------------------------------------------
#endif
