/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

FileSystem.cpp

*******************************************************************************/

#include "FileSystem.hpp"
#include "Logger.hpp"
#ifdef WIN32
	#include <io.h> // For access
	#include <direct.h>
	#include "FileSystemWin32.hpp"	// Dirent Interface for Microsoft C/C++
#else
	#include <stdio.h>
	#include <string.h>
	#include <dirent.h>
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#define MAX_PATH          260
#endif
//#include <sys/utime.h>
//------------------------------------------------------------------------------

//! Lowercase a character if needed
#define DEF_Case(x)  (inIsCase ? (x) : tolower(x))
//------------------------------------------------------------------------------

using namespace Common;

bool FileSystem::fCmpStdString::operator() (const std::string &s1, const std::string &s2) const
{
	return s1.compare(s2) < 0;
}
//------------------------------------------------------------------------------

bool FileSystem::CreateNewDirectory(const std::string &inPath)
{
#ifdef WIN32
	if (::CreateDirectoryA(inPath.c_str(), 0) != 0) {
		return true;
	}
	if (::_mkdir(inPath.c_str()) == 0) {
		return true;
	}
#else
	if (::mkdir(inPath.c_str(), 0755) == 0) {
		return true;
	}
#endif

	return false;
}
//------------------------------------------------------------------------------

bool FileSystem::ClearDirectoryContent(const std::string &inPath,
																			 const char *inMsk /*= NULL*/)
{
	FileSystem::FileSet fileSet;
	FileSystem::GetDirectoryFiles(fileSet, inPath.c_str(), inMsk ? inMsk : "*");

	bool isOk = true;
	FileSystem::FileSet::const_iterator it, itEnd;
	for (it = fileSet.begin(), itEnd = fileSet.end(); it != itEnd; ++it) {
		const std::string fullFileName = (*it);
		isOk &= FileSystem::RemoveFile(fullFileName);
	}

	return isOk;
}
//------------------------------------------------------------------------------

bool FileSystem::RemoveEmptyDirectory(const std::string &inPath)
{
#ifdef WIN32
	if (::RemoveDirectoryA(inPath.c_str()) != 0) {
		return true;
	}
	if (::_rmdir(inPath.c_str()) == 0) {
		return true;
	}
#else
	if (::rmdir(inPath.c_str()) == 0) {
		return true;
	}
#endif

	return false;
}
//------------------------------------------------------------------------------

bool FileSystem::DirectoryExists(const std::string &inPath)
{
#ifdef WIN32
	DWORD ret = ::GetFileAttributesA(inPath.c_str());
	if ((ret != (DWORD)-1) && (ret & FILE_ATTRIBUTE_DIRECTORY))
		return true;
#else
  struct stat path_stat;
	if (::stat(inPath.c_str(), &path_stat) != 0)
		return false;
	if (S_ISDIR(path_stat.st_mode))
		return true;
#endif

	return false;
}
//------------------------------------------------------------------------------

void FileSystem::GetDirectoryFiles(FileSet &outFileSet, const char *inPath,
                const char *inMsk, bool inIsCase /*= true*/, bool inRecurs /*= false*/,
                bool inGetDir /*= false*/)
{
	// Quit if there is an incorrect path
	if (!inPath || *inPath == 0)
		return;

	// Replace the directory separator
  char path[MAX_PATH + 1], *s;
  strcpy(path, inPath);
	while ((s = strchr(path, '\\')) != NULL)
		*s = DEF_DirectorySeparator;
  // Clear the last directory separator if it exists
  s = path + strlen(path) - 1;
  if (*s == DEF_DirectorySeparator)
    *s = 0;

  DIR *dir;
  struct dirent *ent;

  // Open the directory
  if ((dir = opendir(path)) == NULL)
		return;

  // Get directory content
  char buf[MAX_PATH + 1];
  rewinddir(dir);
  while ((ent = readdir(dir)) != NULL) {
    // Clear names begining with a dot (hidden files, current and parent directory)
    if (*(ent->d_name) == '.')
      continue;
    // Get sub directories or recurse or clear them
    DIR *nextdir;
    sprintf(buf, "%s%c%s", path, DEF_DirectorySeparator, ent->d_name);
    nextdir = opendir(buf);
    if (nextdir) {
      closedir(nextdir);
      if (inGetDir) {
				if (!inMsk || PatternMatch(ent->d_name, inMsk, inIsCase))
          outFileSet.insert(buf);
      }
			// Recursive call ?
      if (inRecurs)
        GetDirectoryFiles(outFileSet, buf, inMsk, inIsCase, inRecurs, inGetDir);
      continue;
    }
    if (inGetDir)
      continue;
		if (!inMsk || PatternMatch(ent->d_name, inMsk, inIsCase))
      outFileSet.insert(buf);
  }
  if (closedir(dir) != 0)
		gLog.log(eTypLogError, "Err > Unable to close directory: %s", inPath);
}
//------------------------------------------------------------------------------

bool FileSystem::CreateEmptyFile(const std::string &inFileName)
{
  FILE *f = fopen(inFileName.c_str(), "wb");
	if (!f) {
		gLog.log(eTypLogError, "Err > Unable to create file: %s", inFileName.c_str());
		return false;
	}
	fclose(f);

	return true;
}
//------------------------------------------------------------------------------

bool FileSystem::FileExists(const std::string &inPath)
{
#ifdef WIN32
	// Check modes:
	//   0 Existence only
	//   2 Write permission
	//   4 Read permission
	//   6 Read and write permission 
	if (_access(inPath.c_str(), 0) != -1) {
		return true;
	}
	//DWORD ret = ::GetFileAttributesA(inPath.c_str());
	//if ((ret != (DWORD)-1) && !(ret & FILE_ATTRIBUTE_DIRECTORY))
	//	return true;
#else
  struct stat path_stat;
	if (::stat(inPath.c_str(), &path_stat) != 0)
		return false;
	if (S_ISREG(path_stat.st_mode))
		return true;
#endif

	return false;
}
//------------------------------------------------------------------------------

bool FileSystem::RemoveFile(const std::string &inFullFileName)
{
	// Portability Note: use the ANSI/ISO function remove instead of unlink
	int ret = ::remove(inFullFileName.c_str());

	if (ret != 0) {
		gLog.log(eTypLogError, "Err > Unable to delete file: %s", inFullFileName.c_str());
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------

bool FileSystem::BinaryCompareFiles(const std::string &inFullFileName1, const std::string &inFullFileName2)
{
	bool isOk = true;
	FILE *f1, *f2;
	char *buffer1, *buffer2;
	size_t size1, size2, size = 0xFFFu;	// 4095

	f1 = fopen(inFullFileName1.c_str(), "rb");
	f2 = fopen(inFullFileName2.c_str(), "rb");
	if (f1 == NULL || f2 == NULL) {
		if (f1 == NULL)
			gLog.log(eTypLogError, "Err > Unable to open file: %s", inFullFileName1.c_str());
		if (f2 == NULL)
			gLog.log(eTypLogError, "Err > Unable to open file: %s", inFullFileName2.c_str());
		if (f1)
			fclose(f1);
		if (f2)
			fclose(f2);
		return false;
	}

	buffer1 = new char[size];
	buffer2 = new char[size];
	while (true) {
		size1 = fread(buffer1, 1, size, f1);
		size2 = fread(buffer2, 1, size, f2);
		if (size1 == 0 && size2 == 0)
			break;
		if (size1 != size2 || memcmp(buffer1, buffer2, size1) != 0) {
			isOk = false;
			break;
		}
	}
	delete [] buffer1;
	delete [] buffer2;
	fclose(f1);
	fclose(f2);

	if (!isOk)
		gLog.log(eTypLogError, "Err > Incorrect binary file comparison: %s <=> %s",
			inFullFileName1.c_str(), inFullFileName2.c_str());

	return isOk;
}
//------------------------------------------------------------------------------

bool FileSystem::TextCompareFiles(const std::string &inFullFileName1, const std::string &inFullFileName2)
{
	bool isOk = true;
	FILE *f1, *f2;
	char *buffer1, *buffer2;
	const char *test1, *test2;
	size_t size1, size2;
	int size = 0xFFF;	// 4095

	f1 = fopen(inFullFileName1.c_str(), "r");
	f2 = fopen(inFullFileName2.c_str(), "r");
	if (f1 == NULL || f2 == NULL) {
		if (f1 == NULL)
			gLog.log(eTypLogError, "Err > Unable to open file: %s", inFullFileName1.c_str());
		if (f2 == NULL)
			gLog.log(eTypLogError, "Err > Unable to open file: %s", inFullFileName2.c_str());
		if (f1)
			fclose(f1);
		if (f2)
			fclose(f2);
		return false;
	}

	buffer1 = new char[size];
	buffer2 = new char[size];
	while (true) {
		test1 = fgets(buffer1, size, f1);
		test2 = fgets(buffer2, size, f2);
		if (test1 == NULL || test2 == NULL) {
			if (!(feof(f1) && feof(f2))) {
				isOk = false;
			}
			break;
		}
		//if (ferror(f1) || ferror(f2)) {
		//	isOk = false;
		//	break;
		//}
		//if (feof(f1) || feof(f2)) {
		//	if (!(feof(f1) && feof(f2))) {
		//		isOk = false;
		//	}
		//	break;
		//}
		size1 = strlen(buffer1);
		size2 = strlen(buffer2);
		// Clear crlf
		while (size1 && (buffer1[size1 - 1] == '\r' || buffer1[size1 - 1] == '\n')) {
			buffer1[--size1] = 0;
		}
		while (size2 && (buffer2[size2 - 1] == '\r' || buffer2[size2 - 1] == '\n')) {
			buffer2[--size2] = 0;
		}
		if (size1 != size2 || memcmp(buffer1, buffer2, size1) != 0) {
			isOk = false;
			break;
		}
	}
	delete [] buffer1;
	delete [] buffer2;
	fclose(f1);
	fclose(f2);

	if (!isOk)
		gLog.log(eTypLogError, "Err > Incorrect texte file comparison: %s <=> %s",
			inFullFileName1.c_str(), inFullFileName2.c_str());

	return isOk;
}
//------------------------------------------------------------------------------

/**
	Copyright on PatternMatch from DOSDIR: Portable DOS/UNIX/VMS Directory Interface
	Copyright (C) 1990-1992 Mark Adler, Richard B. Wales, Jean-loup Gailly,
	Kai Uwe Rommel and Igor Mandrichenko.

	Distributable under the terms of the GNU Lesser General Public License,
	as specified in the LICENSE file.
 */
bool FileSystem::PatternMatch(const char *inStr, const char *inMsk, bool inIsCase)
{
	// Error if there is no input string or pattern
	if (!inStr || !inMsk)
		return false;

	unsigned char c; // Pattern char or start of range in [-] loop

	// Get first character, the pattern for new pattern match calls follows
	c = *inMsk++;

	// If that was the end of the pattern, match if string empty too
	if (c == 0)
		return (*inStr == 0);

	// '?' matches any character (but not an empty string)
	if (c == '?')
		return *inStr ? PatternMatch(inStr + 1, inMsk, inIsCase) : false;

	// '*' matches any number of characters, including zero
	if (c == '*') {
		if (*inMsk == 0)
			return true;
		for (; *inStr; ++inStr) {
			if (PatternMatch(inStr, inMsk, inIsCase))
				return true;
		}
		return false; // means give up--match will return false
	}

	// Parse and process the list of characters and ranges in brackets
	if (c == '[') {
		int e; // flag true if next char to be taken literally
		const unsigned char *q; // pointer to end of [-] group
		int r; // flag true to match anything but the range

		// need a character to match
		if (*inStr == 0)
			return false;
		// see if reverse
		inMsk += (r = (*inMsk == '!' || *inMsk == '^'));
		// find closing bracket
		for (q = (const unsigned char *)inMsk, e = 0; *q; ++q) {
			if (e)
				e = 0;
			else {
				if (*q == '\\') // change to ^ for MS-DOS, OS/2?
					e = 1;
				else if (*q == ']')
					break;
			}
		}
		// Nothing matches if bad syntax
		if (*q != ']')
			return false;
		// Go through the list
		for (c = 0, e = *inMsk == '-'; (const unsigned char *)inMsk < q; ++inMsk) {
			if (e == 0 && *inMsk == '\\') // set escape flag if '\'
				e = 1;
			else if (e == 0 && *inMsk == '-') // set start of range if -
				c = *(inMsk - 1);
			else {
				unsigned char cc = DEF_Case(*inStr);

				if (*(inMsk + 1) != '-') {
					// compare range
					for (c = c ? c : *inMsk; c <= *inMsk; ++c) {
						if (DEF_Case(c) == cc)
							return r ? false : PatternMatch(inStr + 1, (const char *)q + 1, inIsCase);
					}
				}
				// Clear range, escape flags
				c = e = 0;
			}
		}
		// Bracket match failed
		return r ? PatternMatch(inStr + 1, (const char *)q + 1, inIsCase) : false;
	}

	// If escape '\', just compare next character
	if (c == '\\' && (c = *inMsk++) == 0) // If \ at end, then syntax error
		return false;

	// Just a character--compare it
	return DEF_Case(c) == DEF_Case(*inStr) ? PatternMatch(++inStr, inMsk, inIsCase) : false;
}
//------------------------------------------------------------------------------

std::string FileSystem::GetPath(const std::string &inFullFileName)
{
	const char *s, *sSep;

	s = inFullFileName.c_str();
	sSep = strrchr(s, DEF_DirectorySeparator);
	if (!sSep)
		return "";

	char buf[MAX_PATH + 1];
	strncpy(buf, s, sSep - s);
	buf[sSep - s] = 0;

	return buf;
}
//------------------------------------------------------------------------------

std::string FileSystem::GetFileName(const std::string &inFullFileName)
{
	const char *s, *sSep;

	s = inFullFileName.c_str();
	sSep = strrchr(s, DEF_DirectorySeparator);

	return sSep ? sSep + 1 : s;
}
//------------------------------------------------------------------------------

std::string FileSystem::GetFileNameWithoutExt(const std::string &inFullFileName)
{
	const char *s, *sSep;
	char buf[MAX_PATH + 1], *sExt;

	// Get the filename
	s = inFullFileName.c_str();
	sSep = strrchr(s, DEF_DirectorySeparator);
	sSep = sSep ? sSep + 1 : s;
	strcpy(buf, sSep);

	// Remove the extension
	sExt = strrchr(buf, '.');
	// The extension separator '.' must be after the last directory separator
	if (!sExt || (sExt && strchr(sExt, DEF_DirectorySeparator)))
		return buf;
	*sExt = 0;

	return buf;
}
//------------------------------------------------------------------------------

std::string FileSystem::GetExtension(const std::string &inFullFileName)
{
	const char *s, *sExt;

	s = inFullFileName.c_str();
	sExt = strrchr(s, '.');
	// The extension separator '.' must be after the last directory separator
	if (!sExt || (sExt && strchr(sExt, DEF_DirectorySeparator)))
		return "";

	return sExt + 1;
}
//------------------------------------------------------------------------------

std::string FileSystem::SetExtension(const std::string &inFullFileName, const std::string &inExt)
{
	char buf[MAX_PATH + 1], *sExt;
	const char *sNewExt;

	strcpy(buf, inFullFileName.c_str());

	// Remove the extension
	sExt = strrchr(buf, '.');
	// The extension separator '.' must be after the last directory separator
	if (!sExt || (sExt && strchr(sExt, DEF_DirectorySeparator))) {
		sExt = buf + strlen(buf);
		strcpy(sExt, ".");
	}
	sExt++;

	// Append new extension
	sNewExt = inExt.c_str();
	if (*sNewExt == '.')
		sNewExt++;
	strcpy(sExt, sNewExt);

	return buf;
}
//------------------------------------------------------------------------------

std::string FileSystem::MakeFullFileName(const std::string &inPath,
																		 const std::string &inFileName,
																		 const std::string &inExt)
{
	std::string fullFileName;

	if (inPath.size()) {
		fullFileName += inPath;
		// Append directory separator if needed
		if (inPath[inPath.size() - 1] != DEF_DirectorySeparator)
			fullFileName += DEF_DirectorySeparator;
	}
	fullFileName += inFileName;

	// Append extension
	const char *ext = inExt.c_str();
	if (*ext) {
		if (*ext != '.')
			fullFileName += ".";
		fullFileName += inExt;
	}

	return fullFileName;
}
//------------------------------------------------------------------------------

std::string FileSystem::MakeFullFileName(const std::string &inPath,
																		 const std::string &inFileName)
{
	std::string fullFileName;

	if (inPath.size()) {
		fullFileName += inPath;
		// Append directory separator if needed
		if (inPath[inPath.size() - 1] != DEF_DirectorySeparator)
			fullFileName += DEF_DirectorySeparator;
	}
	fullFileName += inFileName;

	return fullFileName;
}
//------------------------------------------------------------------------------

std::string FileSystem::SetDirectorySeparator(const std::string &inPath)
{
	std::string path(inPath);

	size_t i;
	while ((i = path.find('\\')) != std::string::npos)
		path[i] = DEF_DirectorySeparator;

	return path;
}
//------------------------------------------------------------------------------

std::string FileSystem::SetPlatformDirectorySeparator(const std::string &inPath)
{
	std::string path(inPath);

#ifdef WIN32
	char cFind = '/';
	char cRepl = '\\';
#else
	char cFind = '\\';
	char cRepl = '/';
#endif

	size_t i;
	while ((i = path.find(cFind)) != std::string::npos)
		path[i] = cRepl;

	return path;
}
//------------------------------------------------------------------------------
