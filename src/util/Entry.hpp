/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

Entry.hpp

*******************************************************************************/

#ifndef __EntryHPP
#define __EntryHPP
//------------------------------------------------------------------------------

#include <set>
#include <string>
//------------------------------------------------------------------------------

#define DEF_SizeOfEntry		   10		// Doc 2 bytes, Pge 4 bytes, Pos 4 bytes
#define DEF_SizeOfEntryScan  28 // 65535-4294967295-4294967295

// Common::BinaryBuffer declaration
namespace Common
{
	class BinaryBuffer;
}
//------------------------------------------------------------------------------

namespace Atoll
{

//! Storage class for document, page and position of indexed content
class DEF_Export Entry
{
public:
  unsigned int mUiEntDoc;
  unsigned long mUlEntPge;
  unsigned long mUlEntPos;

  Entry();
  Entry(unsigned int inDoc, unsigned long inPge, unsigned long inPos);
	Entry &operator=(const Entry &inEntry); //! Copy operator
  bool operator==(const Entry &inEntry) const;
  bool operator<(const Entry &inEntry) const;
  void Set(unsigned int inDoc, unsigned long inPge, unsigned long inPos);
	//! Convert a string buffer with the format poc-pge to an entry (with position = 0)
  bool FromStringDocPage(const char *inStr);
	//! Convert an entry to a string with the format poc-pge. Supply a buffer of DEF_SizeOfEntryScan characters size
  //void ToStringDocPage(char *outStr) const;
	//! Convert a string buffer with the format poc-pge-pos to an entry
  bool FromStringDocPagePos(const char *inStr);
	//! Convert an entry to a string with the format poc-pge-pos. Supply a buffer of DEF_SizeOfEntryScan characters size
  void ToStringDocPagePos(char *outStr) const;
	bool IsEmpty() const;
	std::string ToString() const;
};
//------------------------------------------------------------------------------

//! Entry comparison function by doc, page, position
struct fCmpEntryDocPgePos : std::binary_function<Entry, Entry, bool>
{
  bool operator() (const Entry& e1, const Entry& e2) const;
};
//------------------------------------------------------------------------------

//! Entry Set with comparison function by doc, page, position
typedef std::set <Entry, fCmpEntryDocPgePos, std::allocator<Entry> > EntrySet;
//------------------------------------------------------------------------------

//! Entry comparison function by doc and position
struct fCmpEntryPos : std::binary_function<Entry, Entry, bool>
{
  bool operator() (const Entry& e1, const Entry& e2) const;
};
//------------------------------------------------------------------------------

//! Entry Set with comparison function by doc and position
typedef std::set <Entry, fCmpEntryPos, std::allocator<Entry> > EntryPosSet;
//------------------------------------------------------------------------------

//! Convert an entry to a binary buffer, coded on 2 + 4 + 4 characters
inline void EntryToBuf(char *outBuf, const Entry *inEntry);
//! Convert a binary buffer coded on 2 + 4 + 4 characters to an entry
inline void BufToEntry(const char *inBuf, Entry *outEntry);
//! Convert an entry to a binary buffer, coded on 2 + 4 + 4 characters
void NotInlineEntryToBuf(char *outBuf, const Entry *inEntry);
//! Convert a binary buffer coded on 2 + 4 + 4 characters to an entry
void NotInlineBufToEntry(const char *inBuf, Entry *outEntry);
//! Test if the entry set is new and can be appened to the current data
bool EntrySetCanAppendData(const EntrySet &inEntrySet, const void *inData);
//! Append entry set to the current data
size_t EntrySetAppendData(Common::BinaryBuffer &outBuffer, Common::BinaryBuffer &ioBlobBuffer, const EntrySet &inEntrySet);
//! Serialize an entry set to a binary buffer
size_t EntrySetToBuf(Common::BinaryBuffer &outBuffer, size_t &outDataSize, const EntrySet &inEntrySet);
//! Unserialize a binary buffer to an entry set
size_t BufToEntrySet(EntrySet &outSet, const void *inData, size_t inDataSize, const char *inBufMle);
//! Get the size of a serialized entry set
unsigned long GetBufEntrySetCount(const void *inData);
//! Convert an entry set to a string, for display purpose
std::string EntrySet2String(const EntrySet &inEntrySet);
//------------------------------------------------------------------------------

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
