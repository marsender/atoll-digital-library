/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

EngineApi.hpp

*******************************************************************************/

#ifndef __EngineApiHPP
#define __EngineApiHPP
//------------------------------------------------------------------------------

#include "../common/LibExport.hpp"
#include "../common/Containers.hpp"
#include "../util/ColMeta.hpp"
#include "../util/DocMeta.hpp"
#include "../util/FieldMeta.hpp"
#include "../util/Entry.hpp"
#include "../util/SearchCrit.hpp"
#include "../util/WordInt.hpp"
#include "unicode/unistr.h"
#include <set>
#include <string>
#include <memory> // for std::auto_ptr
//------------------------------------------------------------------------------

namespace AtollPlugin
{
class PluginMessage;
}

namespace Atoll
{
class EngineEnv;
class SearchRecord;

//! Index sort types
enum eTypDynSort {
  eDynSortNotDef,
  eDynSortMotOcc,
  eDynSortOccCroi, eDynSortOccDecr,
  eDynSortFinMot,
  eDynSortNumCroi, eDynSortNumDecr,
  eDynSortEnd
};

//! Get atoll library name
DEF_Export const char *GetLibName();
//! Get atoll library dot version. Eg: 1.3
DEF_Export const char *GetLibVersion();
//! Get Icu library version. Eg: 3.8
DEF_Export const char *GetIcuVersion();
//! Get Berkeley DB library version. Eg: Berkeley DB 4.7.25: (May 15, 2008)
DEF_Export const char *GetDbVersion();
//! Get Xerces library dot version. Eg: 2.8.0
DEF_Export const char *GetXercesVersion();
//! Get Xalan library dot version. Eg: 1.11.0
DEF_Export const char *GetXalanVersion();
//! Get atoll library and dependencies versions
DEF_Export const char *GetFullVersion();
//! Make sure we're loaded with the right version of the DB library
DEF_Export void CheckDbVersion();
//! Make sure we're loaded with the right version of zlib
DEF_Export void CheckZlibVersion();
//! Get atoll library logger
DEF_Export Common::Logger &GetLogger();
//! Get atoll library logger file name
DEF_Export const char *GetLogFileName();
//! Get atoll library trace file name
DEF_Export const char *GetTraceFileName();
//! Create the application global collator
DEF_Export bool CreateCollator();
//! Delete the application global collator
DEF_Export void DeleteCollator();
//! Open the application log file
bool OpenLogFile(const std::string &inFileName);
//! Create the application global plugin kernel
void CreatePluginKernel();
//! Delete the application global plugin kernel
void DeletePluginKernel();

//! Execute an xmlexec unicode buffer
DEF_Export std::auto_ptr<AtollPlugin::PluginMessage> XmlExecBuffer(const UChar *inStr, int32_t inLength);
//! Execute an xmlexec unicode string buffer
DEF_Export std::auto_ptr<AtollPlugin::PluginMessage> XmlExecBuffer(const UnicodeString &inStr);
//! Execute an xmlexec file
DEF_Export std::auto_ptr<AtollPlugin::PluginMessage> XmlExecFile(const std::string &inFileName);
//! Execute an xslt with an xml buffer and a stylesheet buffer
DEF_Export std::auto_ptr<AtollPlugin::PluginMessage> XsltBuffer(const UChar *inStr, int32_t inLength, const UChar *inXsl, int32_t inLengthXsl, const Common::StringToUnicodeStringMap &inStylesheetParamMap);
//! Execute an xslt with an unicode xml string and an unicode stylesheet
DEF_Export std::auto_ptr<AtollPlugin::PluginMessage> XsltBuffer(const UnicodeString &inStr, const UnicodeString &inXsl, const Common::StringToUnicodeStringMap &inStylesheetParamMap);
//! Execute an xslt with an unicode xml string and a named precompiled stylesheet
DEF_Export std::auto_ptr<AtollPlugin::PluginMessage> XsltCompiled(const UnicodeString &inStr, const UnicodeString &inXsltName, const Common::StringToUnicodeStringMap &inStylesheetParamMap);

//! Get the list of all collection records of a database
DEF_Export bool EngineApiGetColMetaVector(const EngineEnv &inEngineEnv, ColMetaVector &outVector);
//! Add a new collection metadata record in a database. Returns the new collection id
DEF_Export bool EngineApiAddCollection(UnicodeString &outColId, const EngineEnv &inEngineEnv, const ColMeta &inColMeta);
//! Get a collection metadata record in a database
DEF_Export bool EngineApiGetCollection(const EngineEnv &inEngineEnv, ColMeta &ioColMeta);
//! Set a collection metadata record in a database
DEF_Export bool EngineApiSetCollection(const EngineEnv &inEngineEnv, const ColMeta &inColMeta);
//! Remove a full collection content in a database
DEF_Export bool EngineApiDelCollection(const EngineEnv &inEngineEnv, const UnicodeString &inColId);

//! Get the list of all the document records of a collection
DEF_Export bool EngineApiGetDocMetaVector(const EngineEnv &inEngineEnv, DocMetaVector &outVector);
//! Add a document metadata in a collection. The document file name must be set. Returns the new document number
DEF_Export bool EngineApiAddDocument(unsigned int &outDocNum, const EngineEnv &inEngineEnv, const DocMeta &inDocMeta, bool inWantIndex);
//! Get a document metadata record in a collection
DEF_Export bool EngineApiGetDocument(const EngineEnv &inEngineEnv, DocMeta &ioDocMeta);
//! Set a document metadata record in a collection
DEF_Export bool EngineApiSetDocument(const EngineEnv &inEngineEnv, const DocMeta &inDocMeta);
//! Remove a full document in a collection
DEF_Export bool EngineApiDelDocument(const EngineEnv &inEngineEnv, unsigned int inDocNum);
//! Index a document and store it's content in a collection
DEF_Export bool EngineApiIndexDocument(const EngineEnv &inEngineEnv, unsigned int inDocNum);
//! Remove a document index and storage content in a collection
DEF_Export bool EngineApiDelDocumentIndex(const EngineEnv &inEngineEnv, unsigned int inDocNum);
//! Get a document content from it's document number and page
DEF_Export bool EngineApiGetDocumentContent(const EngineEnv &inEngineEnv, UnicodeString &outContent, const Entry &inEntry, const Common::IntVector &inHilightVector);

//! Get the list of all field records of a database for a given type, or all types if type = eTypFieldNone
DEF_Export bool EngineApiGetFieldMetaVector(const EngineEnv &inEngineEnv, FieldMetaVector &outVector, eTypField inTypField);
//! Add a new field metadata record in a database
DEF_Export bool EngineApiAddField(const EngineEnv &inEngineEnv, const FieldMeta &inFieldMeta);
//! Get a field metadata record in a database
DEF_Export bool EngineApiGetField(const EngineEnv &inEngineEnv, FieldMeta &ioFieldMeta);
//! Set a field metadata record in a database
DEF_Export bool EngineApiSetField(const EngineEnv &inEngineEnv, const FieldMeta &inFieldMeta);
//! Remove a field metadata record in a database
DEF_Export bool EngineApiDelField(const EngineEnv &inEngineEnv, const FieldMeta &inFieldMeta);

//! Search the database for a criteria
DEF_Export bool EngineApiComputeCrit(const EngineEnv &inEngineEnv, SearchRecord &ioSearchRecord);
//! Extract a search result from a set of positions (only the first position of consecutives positions is kept)
DEF_Export void EngineApiGetSearchResult(EntrySet &outEntrySet, const EntrySet &inEntrySet, unsigned long inMin, unsigned long &ioNb, unsigned long &outCount);
//! Search the database for an index content
DEF_Export bool EngineApiComputeIndex(const EngineEnv &inEngineEnv, WordIntVector &outWordIntVector, const SearchRecord &ioSearchRecord, const UnicodeString &inNdxName, unsigned long &ioMin, const UnicodeString &inWord, unsigned long &ioNb, unsigned long &outCount);
//! Get the map of all index
DEF_Export bool EngineApiGetIndexMap(const EngineEnv &inEngineEnv, Common::UnicodeStringToIntMap &outMap);

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
