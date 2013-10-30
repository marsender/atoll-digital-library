/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Entry.cpp

*******************************************************************************/

#include "../Portability.hpp"
#include "Entry.hpp"
#include "../common/BinaryBuffer.hpp"
#include "../common/Zlib.hpp"
#include <stdio.h>
#include <algorithm>
//------------------------------------------------------------------------------

//#define DEF_Log(x) { gLog.log(eTypLogError, "Err > Entry: %s", x); }

// Sizes:
//   2 for the doc
//   4 for the page, pos and the Entry Count
//   6 for the dec (2 for the doc + 4 for the Entry Count)
#define DEF_SizeOfDec (DEF_SizeOfInt + DEF_SizeOfLong)

// Want zlib record compression / decompression ?
const bool cWantZlib = false; // No compression as it is done during data storage
const bool cWantDec = false; // Store doc entry count ?
const size_t cNbEntMinForZlib = 100;

using namespace Atoll;
using namespace Common;
//------------------------------------------------------------------------------

inline void Atoll::EntryToBuf(char *outBuf, const Entry *inEntry)
{
	AppAssertMessage(inEntry->mUiEntDoc != 0, "EntryToBuff: Incorrect doc num");

  SetShBuf(outBuf, inEntry->mUiEntDoc);
  SetLgBuf(outBuf + 2, inEntry->mUlEntPge);
  SetLgBuf(outBuf + 6, inEntry->mUlEntPos);
}
//------------------------------------------------------------------------------

inline void Atoll::BufToEntry(const char *inBuf, Entry *outEntry)
{
  outEntry->mUiEntDoc = GetShBuf(inBuf);
  outEntry->mUlEntPge = GetLgBuf(inBuf + 2);
  outEntry->mUlEntPos = GetLgBuf(inBuf + 6);

	AppAssertMessage(outEntry->mUiEntDoc != 0, "BufToEntry: Incorrect doc num");
}
//------------------------------------------------------------------------------

void Atoll::NotInlineEntryToBuf(char *outBuf, const Entry *inEntry)
{
  EntryToBuf(outBuf, inEntry);
}
//------------------------------------------------------------------------------

void Atoll::NotInlineBufToEntry(const char *inBuf, Entry *outEntry)
{
  BufToEntry(inBuf, outEntry);
}
//------------------------------------------------------------------------------

bool Atoll::EntrySetCanAppendData(const EntrySet &inEntrySet, const void *inData)
{
	// Cannot work if buffer is compressed
	if (cWantZlib)
		return false;

	// Total number of docs and entries
	const char *data = (const char *)inData;
	// First DEF_SizeOfInt is reserved for nb blobs
	unsigned int nbDoc = GetShBuf(data + DEF_SizeOfInt);

	unsigned int lastDoc;
	if (cWantDec) {
		const char *decBuf = data + DEF_SizeOfInt + DEF_SizeOfInt + DEF_SizeOfLong;
		// Go to the last dec
		const char *decPos = decBuf + ((nbDoc - 1) * DEF_SizeOfDec);
		lastDoc = GetShBuf(decPos);
	}
	else {
		lastDoc = GetShBuf(data + DEF_SizeOfInt);
	}

	// Cannot work if the doc number of the data to append is not greater
	EntrySet::const_iterator it = inEntrySet.begin();
	if (it->mUiEntDoc <= lastDoc)
		return false;

	return true;
}
//------------------------------------------------------------------------------

size_t Atoll::EntrySetAppendData(BinaryBuffer &outBuffer, BinaryBuffer &ioBlobBuffer, const EntrySet &inEntrySet)
{
	// Total number of docs and entries
	char *data = (char *)outBuffer.getBuffer();
	unsigned long nbEnt = GetLgBuf(data + DEF_SizeOfInt + DEF_SizeOfInt);

	char entBuf[DEF_SizeOfEntry];
	if (cWantDec) {
		// First DEF_SizeOfInt is reserved for nb blobs
		unsigned int nbDoc = GetShBuf(data + DEF_SizeOfInt);
		char decBuf[DEF_SizeOfDec];
		unsigned int lastDoc = DEF_LongMax, nbDocEntries = 0;
		EntrySet::const_iterator it = inEntrySet.begin();
		EntrySet::const_iterator itEnd = inEntrySet.end();
		for (; it != itEnd; ++it) {
			const Entry &e = *it;
			if (e.mUiEntDoc != lastDoc) {
				// Write last Dec
				if (lastDoc != DEF_LongMax) {
					SetShBuf(decBuf, lastDoc);
					SetLgBuf(decBuf + DEF_SizeOfInt, nbDocEntries);
					outBuffer.write(decBuf, DEF_SizeOfDec);
					nbDocEntries = 0;
				}
				nbDoc++;
				lastDoc = e.mUiEntDoc;
			}
			nbEnt++;
			nbDocEntries++;
			// Write the entry
			EntryToBuf(entBuf, &e);
			ioBlobBuffer.write(entBuf, DEF_SizeOfEntry);
		}
		// Write last Dec
		if (lastDoc != DEF_LongMax) {
			SetShBuf(decBuf, lastDoc);
			SetLgBuf(decBuf + DEF_SizeOfInt, nbDocEntries);
			outBuffer.write(decBuf, DEF_SizeOfDec);
		}
		// Update header infos
		data = (char *)outBuffer.getBuffer();
		// First DEF_SizeOfInt is reserved for nb blobs
		SetShBuf(data + DEF_SizeOfInt, nbDoc);
		SetLgBuf(data + DEF_SizeOfInt + DEF_SizeOfInt, (unsigned long)nbEnt);
	}
	else {
		EntrySet::const_iterator it = inEntrySet.begin();
		EntrySet::const_iterator itEnd = inEntrySet.end();
		for (; it != itEnd; ++it) {
			nbEnt++;
			// Write the entry
			EntryToBuf(entBuf, &(*it));
			ioBlobBuffer.write(entBuf, DEF_SizeOfEntry);
		}
		// Get last doc
		Entry e;
		BufToEntry(entBuf, &e);
		// Update header infos
		data = (char *)outBuffer.getBuffer();
		// First DEF_SizeOfInt is reserved for nb blobs
		SetShBuf(data + DEF_SizeOfInt, e.mUiEntDoc);
		SetLgBuf(data + DEF_SizeOfInt + DEF_SizeOfInt, (unsigned long)nbEnt);
	}

	return nbEnt;
}
//------------------------------------------------------------------------------

#define DEF_InitNbDec    100
#define DEF_InitNbEntry  1000
size_t Atoll::EntrySetToBuf(BinaryBuffer &outBuffer, size_t &outDataSize, const EntrySet &inEntrySet)
{
	size_t nbEnt = inEntrySet.size();

	if (cWantDec) {
		// Set of Dec
		size_t decSize = DEF_InitNbDec * DEF_SizeOfDec;
		char *decBuf = (char *)::malloc(decSize);
		char *decPos = decBuf;
		// Set of Entry
		size_t entSize = DEF_InitNbEntry * DEF_SizeOfEntry;
		char *entBuf = (char *)::malloc(entSize);
		char *entPos = entBuf;

		EntrySet::const_iterator it = inEntrySet.begin();
		EntrySet::const_iterator itEnd = inEntrySet.end();
		unsigned int nbDoc = 0, lastDoc = DEF_LongMax, nbDocEntries = 0;
		for (; it != itEnd; ++it) {
			const Entry &e = *it;
			if (e.mUiEntDoc != lastDoc) {
				// Write last Dec
				if (lastDoc != DEF_LongMax) {
					// Reserve space for the next Dec
					if (decPos - decBuf == (int)decSize) {
						decSize *= 2; //+= DEF_InitNbDec * DEF_SizeOfDec;
						char *newBuf = (char *)::realloc(decBuf, decSize);
						if (!newBuf)
							DEF_Exception(AppException::NoMemoryError, "Buffer: failed to allocate memory");
						decPos = newBuf + (decPos - decBuf);
						decBuf = newBuf;
					}
					SetShBuf(decPos, lastDoc);
					SetLgBuf(decPos + DEF_SizeOfInt, nbDocEntries);
					nbDocEntries = 0;
					decPos += DEF_SizeOfDec;
				}
				nbDoc++;
				lastDoc = e.mUiEntDoc;
			}
			nbDocEntries++;
			// Reserve space for the Entry
			if (entPos - entBuf == (int)entSize) {
				entSize *= 2; //+= DEF_InitNbEntry * DEF_SizeOfEntry;
				char *newBuf = (char *)::realloc(entBuf, entSize);
				if (!newBuf)
					DEF_Exception(AppException::NoMemoryError, "Buffer: failed to allocate memory");
				entPos = newBuf + (entPos - entBuf);
				entBuf = newBuf;
			}
			// Write the entry
			EntryToBuf(entPos, &(*it));
			entPos += DEF_SizeOfEntry;
		}

		// Write last Dec
		if (lastDoc != DEF_LongMax) {
			// Reserve space for the next Dec
			if (decPos - decBuf == (int)decSize) {
				decSize += DEF_SizeOfDec; //DEF_InitNbDec * DEF_SizeOfDec;
				char *newBuf = (char *)::realloc(decBuf, decSize);
				if (!newBuf)
					DEF_Exception(AppException::NoMemoryError, "Buffer: failed to allocate memory");
				decPos = newBuf + (decPos - decBuf);
				decBuf = newBuf;
			}
			SetShBuf(decPos, lastDoc);
			SetLgBuf(decPos + DEF_SizeOfInt, nbDocEntries);
			decPos += DEF_SizeOfDec;
		}

		// Total number of docs and entries
		size_t tnbSize = DEF_SizeOfInt + DEF_SizeOfInt + DEF_SizeOfLong;
		char *tnbBuf = (char *)::malloc(tnbSize);
		// First DEF_SizeOfInt is reserved for nb blobs
		SetShBuf(tnbBuf + DEF_SizeOfInt, nbDoc);
		SetLgBuf(tnbBuf + DEF_SizeOfInt + DEF_SizeOfInt, (unsigned long)nbEnt);

		decSize = decPos - decBuf;
		if (decSize != DEF_SizeOfDec * nbDoc)
			DEF_Exception(AppException::InternalError, "Buffer: incorrect dec size");
		entSize = entPos - entBuf;
		if (entSize != DEF_SizeOfEntry * nbEnt)
			DEF_Exception(AppException::InternalError, "Buffer: incorrect dec size");

		// Raz output binary buffer
		BinaryBuffer *binaryBuffer;
		if (cWantZlib && nbEnt > cNbEntMinForZlib) {
			outBuffer.reset();
			binaryBuffer = new BinaryBuffer;
		}
		else {
			binaryBuffer = &outBuffer;
			binaryBuffer->reset(); // resetCursorPosition();
		}

		// Write all in the output buffer
		binaryBuffer->resetCursorPosition();
		outBuffer.write(tnbBuf, tnbSize);
		outBuffer.write(decBuf, decSize);
		outDataSize = outBuffer.getOccupancy();
		binaryBuffer->write(entBuf, entSize);

		::free(decBuf);
		::free(entBuf);
		::free(tnbBuf);

		if (cWantZlib && nbEnt > cNbEntMinForZlib) {
			char buf[DEF_SizeOfLong];
			unsigned long length = (unsigned long)binaryBuffer->getOccupancy();
			SetLgBuf(buf, length);
			// Write the size of the uncompressed buffer
			outBuffer.write(buf, DEF_SizeOfLong);
			outDataSize = outBuffer.getOccupancy();
			// Append the compressed buffer
			ZlibCompress(outBuffer, (const char *)binaryBuffer->getBuffer(), length);
			delete binaryBuffer;
		}
	}
	else {
		// Total number of docs and entries
		size_t tnbSize = DEF_SizeOfInt + DEF_SizeOfInt + DEF_SizeOfLong;
		char *tnbBuf = (char *)::malloc(tnbSize);
		// Write the header
		outBuffer.reset();
		outBuffer.write(tnbBuf, tnbSize);
		::free(tnbBuf);
		outDataSize = outBuffer.getOccupancy();
		// Add the entries
		char entBuf[DEF_SizeOfEntry];
		EntrySet::const_iterator it = inEntrySet.begin();
		EntrySet::const_iterator itEnd = inEntrySet.end();
		for (; it != itEnd; ++it) {
			// Write the entry
			EntryToBuf(entBuf, &(*it));
			outBuffer.write(entBuf, DEF_SizeOfEntry);
		}
		// Get last doc
		unsigned int docMax = 0;
		if (nbEnt) {
			Entry e;
			BufToEntry(entBuf, &e);
			docMax = e.mUiEntDoc;
		}
		// Update header infos
		char *data = (char *)outBuffer.getBuffer();
		// First DEF_SizeOfInt is reserved for nb blobs
		SetShBuf(data + DEF_SizeOfInt, docMax);
		SetLgBuf(data + DEF_SizeOfInt + DEF_SizeOfInt, (unsigned long)nbEnt);
	}

	return nbEnt;
}
//------------------------------------------------------------------------------

size_t Atoll::BufToEntrySet(EntrySet &outSet, const void *inData, size_t inDataSize, const char *inBufMle)
{
	outSet.clear();

	// Nothing to do if no data is given
	if (inData == NULL)
		return 0;

	// Total number of docs and entries
	const char *data = (const char *)inData;
	unsigned long nbEnt = GetLgBuf(data + DEF_SizeOfInt + DEF_SizeOfInt);

	if (cWantDec) {
		// First DEF_SizeOfInt is reserved for nb blobs
		unsigned int nbDoc = GetShBuf(data + DEF_SizeOfInt);
		const char *decBuf = data + DEF_SizeOfInt + DEF_SizeOfInt + DEF_SizeOfLong;

		size_t decSize = nbDoc * DEF_SizeOfDec;
		const char *entBuf = decBuf + decSize;

		// Zlib decompression
		char *uncompr = NULL;
		if (cWantZlib && nbEnt > cNbEntMinForZlib) {
			unsigned long length = GetLgBuf(entBuf);
			entBuf += DEF_SizeOfLong;
			unsigned long comprLength = inDataSize - (entBuf - data);
			if (!ZlibDeflate(uncompr, length, (void *)entBuf, comprLength)) {
				::free(uncompr);
				return 0;
			}
			entBuf = uncompr;
		}

		if (inDataSize - (entBuf - data) != nbEnt * DEF_SizeOfEntry)
			DEF_Exception(AppException::InternalError, "Entry set buffer: incorrect size");

		// Check if the buffer size is correct
		//size_t entSize = nbEnt * DEF_SizeOfEntry;
		//if (bufSize != DEF_SizeOfDec + decSize + entSize)
		//	DEF_Exception(AppException::InternalError, "Entry set buffer: incorrect buffer size");

		unsigned long entriesCpt = 0;	// Nombre total d'entrees ajoutees

		// No document filter buffer: get all entries
		if (inBufMle == NULL) {
			// Read and add all the entries in the output set
			Entry entry;
			const char *c = entBuf;
			unsigned long theNdxCpt = nbEnt;
			for (unsigned long i = 0; i < theNdxCpt; i++) {
				BufToEntry(c, &entry);
				outSet.insert(entry);
				c += DEF_SizeOfEntry;
			}
			entriesCpt += theNdxCpt;
		}
		// Document filter buffer given: use the filter
		else {
			// Intelligent header:
			// ulong string size, uint entries size, ulong entries number
			unsigned int n = GetShBuf(inBufMle + 4); // Comparison size
			unsigned long thePlaCpt = GetLgBuf(inBufMle + 6); // Entries number
			unsigned long theNdxCpt;
			unsigned int theNbDoc = nbDoc;
			long cmp;
			// Left list pointer (after the intelligent header)
			const char *l = inBufMle + 10;
			// Right list pointer (en debut de liste du fichier ndx)
			const char *c, *r = entBuf;
			const char *Idc = decBuf;	// Entete Idc : Doc n
			Entry entry;
			// Loop on the documents
			while (theNbDoc && thePlaCpt) {
				theNdxCpt = GetLgBuf(Idc + 2);	// Idc header : EntryCount n
				for (; thePlaCpt; l += n, thePlaCpt--) {
					// Idc is used as a right list
					if ((cmp = memcmp(l, Idc, n)) >= 0) {
						if (cmp == 0) {
							c = r;
							for (unsigned long i = 0; i < theNdxCpt; i++) {
								BufToEntry(c, &entry);
								outSet.insert(entry);
								c += DEF_SizeOfEntry;
							}
							entriesCpt += theNdxCpt;
						}
						break;
					}
				}
				r += DEF_SizeOfEntry * theNdxCpt;
				Idc += DEF_SizeOfDec;	// Goto the next Doc|EntryCount
				theNbDoc--;
			}
		}

		if (uncompr) {
			::free(uncompr);
		}
		if (nbEnt != entriesCpt)
			DEF_Exception(AppException::InternalError, "Entry set buffer: incorrect number of entries");
	}
	else {
		const char *entBuf = data + DEF_SizeOfInt + DEF_SizeOfInt + DEF_SizeOfLong;
		// Read and add all the entries in the output set
		Entry entry;
		const char *c = entBuf;
		for (unsigned long i = 0; i < nbEnt; i++) {
			BufToEntry(c, &entry);
			outSet.insert(entry);
			c += DEF_SizeOfEntry;
		}
	}

	return nbEnt;
}
//------------------------------------------------------------------------------

unsigned long Atoll::GetBufEntrySetCount(const void *inData)
{
	// Nothing to do if no data is given
	if (inData == NULL)
		return 0;

	// Get the total number of entries
	const char *data = (const char *)inData;
	// First DEF_SizeOfInt is reserved for nb blobs
	// Second DEF_SizeOfInt is reserved for nb docs
	unsigned long nbEnt = GetLgBuf(data + DEF_SizeOfInt + DEF_SizeOfInt);

	return nbEnt;
}
//------------------------------------------------------------------------------

/*
class ForEachEntryToString {
public:
	static std::string mResult;
  void operator() (Entry inEntry) {
		mResult += inEntry.ToString() + " ";
	}
};
std::string ForEachEntryToString::mResult;
//------------------------------------------------------------------------------

std::string Atoll::EntrySet2String(const EntrySet &inEntrySet)
{
	std::string s("EntrySet: ");
	unsigned long nbEnt = inEntrySet.size();

	if (nbEnt == 0) {
		s += "Empty";
		return s;
	}

	ForEachEntryToString forEach;
	ForEachEntryToString::mResult = "";
	std::for_each(inEntrySet.begin(), inEntrySet.end(), forEach);
	s += ForEachEntryToString::mResult;
	ForEachEntryToString::mResult = "";

	return s;
}*/
//------------------------------------------------------------------------------

std::string Atoll::EntrySet2String(const EntrySet &inEntrySet)
{
	std::string s("EntrySet: ");
	size_t nbEnt = inEntrySet.size();

	if (nbEnt == 0) {
		s += "(empty)";
	}
	else	{
		char buf[50];
		sprintf (buf, "(count = %u)", (unsigned int)nbEnt);
		s += buf;
		s += " [ ";
		unsigned nb = 0, nbMax = 10;
		EntrySet::const_iterator it = inEntrySet.begin();
		EntrySet::const_iterator itEnd = inEntrySet.end();
		for (; it != itEnd; ++it) {
			const Entry &e = *it;
			s += e.ToString();
			if (nb++ == nbMax) {
				s += "... ";
				break;
			}
			s += " ";
		}
		s += "]";
	}


	return s;
}
//------------------------------------------------------------------------------

Entry::Entry()
{
  mUiEntDoc = 0;
  mUlEntPge = 0;
  mUlEntPos = 0;
}
//------------------------------------------------------------------------------

Entry::Entry(unsigned int inDoc, unsigned long inPge, unsigned long inPos)
{
  mUiEntDoc = inDoc < DEF_IntMax ? inDoc : DEF_IntMax;
  mUlEntPge = inPge;
  mUlEntPos = inPos;
}
//------------------------------------------------------------------------------

Entry &Entry::operator=(const Entry &inEntry)
{
	mUiEntDoc = inEntry.mUiEntDoc;
	mUlEntPge = inEntry.mUlEntPge;
  mUlEntPos = inEntry.mUlEntPos;

	return *this;
}
//------------------------------------------------------------------------------

bool Entry::operator==(const Entry &inEntry) const
{
  return inEntry.mUlEntPos == mUlEntPos
         && inEntry.mUlEntPge == mUlEntPge
         && inEntry.mUiEntDoc == mUiEntDoc;
}
//------------------------------------------------------------------------------

bool Entry::operator<(const Entry& inEntry) const
{
	// Same as fCmpEntryDocPgePos
	if (mUiEntDoc == inEntry.mUiEntDoc) {
		if (mUlEntPge == inEntry.mUlEntPge) {
			if (mUlEntPos == inEntry.mUlEntPos)
				return false;
			return mUlEntPos < inEntry.mUlEntPos;
		}
		else
			return mUlEntPge < inEntry.mUlEntPge;
	}
	else
		return mUiEntDoc < inEntry.mUiEntDoc;
}
//------------------------------------------------------------------------------

void Entry::Set(unsigned int inDoc, unsigned long inPge, unsigned long inPos)
{
  mUiEntDoc = inDoc < DEF_IntMax ? inDoc : DEF_IntMax;
  mUlEntPge = inPge;
  mUlEntPos = inPos;
}
//------------------------------------------------------------------------------

bool Entry::FromStringDocPage(const char *inStr)
{
	AppAssert(inStr);

  if (sscanf(inStr, "%u-%lu", &mUiEntDoc, &mUlEntPge) != 2) {
		gLog.log(eTypLogError, "Err > Entry: Incorrect doc-page string -%s-", inStr);
		return false;
	}

  mUlEntPos = 0;
	return true;
}
//------------------------------------------------------------------------------

/*
void Entry::ToStringDocPage(char *outStr) const
{
  sprintf(outStr, "%u-%lu", mUiEntDoc, mUlEntPge);
}*/
//------------------------------------------------------------------------------

bool Entry::FromStringDocPagePos(const char *inStr)
{
	AppAssert(inStr);

	if (sscanf(inStr, "%u-%lu-%lu", &mUiEntDoc, &mUlEntPge, &mUlEntPos) != 3) {
		gLog.log(eTypLogError, "Err > Entry: Incorrect doc-page-pos string -%s-", inStr);
		return false;
	}

	return true;
}
//------------------------------------------------------------------------------

// Fournir un buffer de sortie de 28 min (utiliser DEF_SizeOfEntryScan)
// Exemple: 65535-4294967295-4294967295
void Entry::ToStringDocPagePos(char *outStr) const
{
  sprintf(outStr, "%u-%lu-%lu", mUiEntDoc, mUlEntPge, mUlEntPos);
}
//------------------------------------------------------------------------------

bool Entry::IsEmpty() const
{
	return mUiEntDoc == 0 && mUlEntPge == 0 && mUlEntPos == 0;
}
//------------------------------------------------------------------------------

std::string Entry::ToString() const
{
	char buf[DEF_SizeOfEntryScan];

	ToStringDocPagePos(buf);

	std::string s(buf);

	return s;
}
//------------------------------------------------------------------------------

bool fCmpEntryDocPgePos::operator() (const Entry& e1, const Entry& e2) const
{
  if (e1.mUiEntDoc == e2.mUiEntDoc) {
    if (e1.mUlEntPge == e2.mUlEntPge) {
      if (e1.mUlEntPos == e2.mUlEntPos)
        return false;
      return e1.mUlEntPos < e2.mUlEntPos;
    }
    else
      return e1.mUlEntPge < e2.mUlEntPge;
  }
  else
    return e1.mUiEntDoc < e2.mUiEntDoc;
}
//------------------------------------------------------------------------------

bool fCmpEntryPos::operator() (const Entry& e1, const Entry& e2) const
{
  if (e1.mUiEntDoc == e2.mUiEntDoc) {
    if (e1.mUlEntPos == e2.mUlEntPos)
      return false;
    return e1.mUlEntPos < e2.mUlEntPos;
  }
  else
    return e1.mUiEntDoc < e2.mUiEntDoc;
}
//------------------------------------------------------------------------------
