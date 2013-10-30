/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

XmlExecCmd.hpp

*******************************************************************************/

#ifndef __XmlExecCmd_HPP
#define __XmlExecCmd_HPP
//------------------------------------------------------------------------------

#define DEF_Tab        "\t"
#define gXmlTok(x)     XmlExecCmd::mXmlCmd[x]

#include <map>
//------------------------------------------------------------------------------

enum eXmlCmd {
  eXmlCmdNone,
	eAtollXmlExec,
  // Requests
  eReqAll,
  eResAll,
  eReqInit,
  eResInit,
  eReqInfo,
  eResInfo,
  eReqUtil,
  eResUtil,
	eReqDb,
	eResDb,
  eReqCol,
  eResCol,
  eReqDoc,
  eResDoc,
  eReqSearch,
  eResSearch,
  eReqIndex,
  eResIndex,
  eReqSearchCrit,
  eResSearchCrit,
  eReqSearchScreen,
  eResSearchScreen,
  eReqUsr,
  eResUsr,
	eResError,
  // Commands
  eVersion,
  eLogFile,
  eTraceFile,
  eOutFile,
  eDbHome,
  eColPath,
  eUsrPath,
  eFacPath,
  eSrvPath,
  eUsrDynPath,
  eUsrHstPath,
  eUsrReqPath,
  eAttGraphie,
  eAttLang,
  eAttTypClient,
  eAttTypOs,
  eAttUsrAction,
  eAttUsrActionAnnoGet,
  eAttUsrActionAnnoSet,
  eAttUsrActionAnnoDel,
  eAttUsrActionAnnoList,
  eAttUsrActionSignetGet,
  eAttUsrActionSignetSet,
  eAttUsrActionSignetDel,
  eAttUsrActionSignetList,
  eAttUsrActionSearchCritGet,
  eAttUsrActionSearchCritSet,
  eAttUsrActionSearchCritDel,
  eAttUsrActionSearchCritList,
  eAttUsrActionHistRechGet,
  eAttUsrActionHistRechSet,
  eAttUsrActionHistRechDel,
  eAttUsrActionHistRechList,
  eAttUsrActionProfilGet,
  eAttUsrActionProfilSet,
  eAttUsrActionProfilDel,
  eAttUsrActionProfilList,
  eAttUsrActionCorpusNew,
  eAttUsrActionCorpusDel,
  eAttUsrActionCorpusRename,
  eAttUsrActionCorpusClear,
  eAttUsrActionCorpusAddPage,
  eAttUsrActionCorpusAddResRech,
  eAttUsrActionCorpusList,
  eAttUsrActionResRechSuppr,
  eUser,
  eSearchId,
  eCol,
  eColId,
  eColTitle,
	eColSalt,
  //eDocName,
  eDocNum,
  eCountPge,
  eFileName,
	eDcMetadata,
	eIndexerConfig,
	eRecordBreakerConfig,
	eUuid,
	eXsltCode,
	eXmlValidation,
	eIsIndexed,
  eName,
  eNum,
  eDocTexte,
  eDocPagPos,
  eAnno,
  eSignet,
  eNoticeAuteur,
  eNoticeEditeur,
	eInfo4emeCouv,
  eFacsim,
  eOcrInfo,
  eLink,
  eSearch,
  eTexte,
  eAction,
  eOpen,
  eClose,
  eExport,
  eContexte,
  eDef,
  eClearCacheCol,
  eClearCacheUsr,
  eClearCacheUsrDyn,
  eClearCacheUsrHst,
  eClearCacheUsrReq,
  eStartIndex,
  eUpdateIndex,
  eMakeIndexDyn,
  eMakeTabmat,
  eGetListCol,
  eListCol,
  eGetListDoc,
  eGetListField,
  eListDoc,
  eListField,
  eGetListNdx,
  eListNdx,
  eListTexte,
  eListDocTexte,
	eAddCol,
	eGetCol,
	eSetCol,
	eDelCol,
	eAddDoc,
	eGetDoc,
	eSetDoc,
	eDelDoc,
	eField,
  eFieldId,
	eAddField,
	eGetField,
	eSetField,
	eDelField,
	eIndexDoc,
	eDelDocIndex,
  eGetStatus,
  eStatus,
  eNoIndex,
  ePlage,
  eMot,
  eMin,
  eNb,
  eMax,
  eTypeRes,
  eDoc,
  ePage,
  ePos,
  eSearchCrit,
  eCrit,
  eSortList,
  eIdCrit,
  eSearchScreen,
  eMode,
  eOpeRech,
  eTypeIndex,
  eIndex,
  eListe,
  eArticle,
  eTypeTri,
  eMotFreq,
  eFreqCroi,
  eFreqDecr,
  eFinMot,
	eNumCroi,
	eNumDecr,
  ePara,
  ePhrase,
  eNbMot,
  eFile,
  eTabmat,
  eTag,
  eGrec,
  eNbRes,
  eNbDoc,
  eNbPage,
  eNbPos,
  eFormatXml,
  eFormatHtml,
  eTool,
  eStat,
  eBinary,
  eUserInfo,
  eParam,
  eResult,
  eToolBar,
  eObject,
  eType,
	eOrder,
  eEnabled,
  eChecked,
  eCorpus,
  eNewSearch,
	eTitle,
	eSetOfFiles,
	eBinFile,
	eBase64,
	// End
  eXmlCmdEnd
};


namespace AtollPluginXmlExec
{

//! XmlExec command tokens
class XmlExecCmd
{
public:
	typedef std::map < std::string, unsigned int > AssocMap;

	static const char *mXmlCmd[eXmlCmdEnd]; //!< Array of associations from enum to strings

	static AssocMap &StaticGetMap();

private:
	static XmlExecCmd mHandle;
	static bool mIsFirstAccess;
	static AssocMap mStaticMap; //!< Map of associations from strings to enum
};

} // namespace Atoll

//------------------------------------------------------------------------------
#endif
