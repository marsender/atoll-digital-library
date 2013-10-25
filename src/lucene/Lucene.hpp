/*******************************************************************************

Lucene.hpp

Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
Distributable under the terms of either the Apache License (Version 2.0) or
the GNU Lesser General Public License, as specified in the LICENSE file.

*******************************************************************************/

#ifndef __Lucene_HPP
#define	__Lucene_HPP
//------------------------------------------------------------------------------

#include "../common/AppAssert.hpp"
#include "../common/AppException.hpp"
//#include "../common/Containers.hpp" // For CollatorStrcmp CollatorStrncmp
#include "../common/Logger.hpp"
#include "../common/Trace.hpp"
//#include "unicode/ustring.h"
#include "unicode/uchar.h"
#ifndef _WIN32
	#include <math.h> // For float_t
#endif 
//------------------------------------------------------------------------------

// Remember to find and remove all #ifdef _UCS2

// Namespaces
#define CL_NS_DEF(sub) namespace Lucene{ namespace sub{
#define CL_NS_END }}
#define CL_NS_USE(sub) using namespace Lucene::sub;
#define CL_NS(sub) Lucene::sub
#define CL_NS_STD(func) std::func

// New / delete
#define _CLNEW new
#define _CLLDELETE(x) if (x != NULL) { delete x; }
#define _CLDELETE(x) if (x != NULL) { delete x; x = NULL; }
#define _CL_NEWARRAY(type,size) new type[size]
#define _CLDELETE_ARRAY(x) if (x != NULL) { delete [] x; x = NULL; }
#define _CLDELETE_LARRAY(x) if (x != NULL) { delete [] x; }
#define _CLDELETE_CARRAY(x) if (x != NULL) { delete [] x; x = NULL; }
#define _CLDELETE_LCARRAY(x) if (x != NULL) { delete [] x; }
#define _CLDELETE_CARRAY_ALL(x) { if ( x != NULL) { for(int xcda=0;x[xcda]!=NULL;xcda++)_CLDELETE_CARRAY(x[xcda]); } _CLDELETE_ARRAY(x) };
#define _CLDELETE_ARRAY_ALL(x) { if (x != NULL) { for(int xcda=0;x[xcda]!=NULL;xcda++)_CLDELETE(x[xcda]); } _CLDELETE_ARRAY(x) };
// Ref count
#define _CL_POINTER(x) (x == NULL ? NULL : (x->__cl_addref() >= 0 ? x : x)) // Return a add-ref'd object
//_CLDECDELETE deletes objects which are *always* refcounted
#define _CLDECDELETE(x) if (x!=NULL) { CND_PRECONDITION((x)->__cl_refcount>=0,"__cl_refcount was < 0"); if ((x)->__cl_decref() <= 0)delete x; x=NULL; }
#define _CLLDECDELETE(x) if (x!=NULL) { CND_PRECONDITION((x)->__cl_refcount>=0,"__cl_refcount was < 0"); if ((x)->__cl_decref() <= 0)delete x; }

// File / Line info
#define CL_FILELINE
#define CL_FILELINEREF
#define CL_FILELINEREF2 ,NULL,-1
#define CL_FILELINEPARAM

// Exceptions
#define CLuceneError Common::AppException
#define CL_ERR_IO Common::AppException::IoError
#define CL_ERR_NullPointer AppException::NullPointer
#define CL_ERR_Runtime AppException::RuntimeError
#define CL_ERR_IllegalArgument AppException::InvalidArgument
#define CL_ERR_Parse AppException::QueryParserError
#define CL_ERR_TokenMgr AppException::NoMemoryError
#define CL_ERR_UnsupportedOperation AppException::InvalidOperation
#define CL_ERR_InvalidState AppException::InvalidState
#define CL_ERR_IndexOutOfBounds AppException::OutOfBounds
#define CL_ERR_TooManyClauses AppException::RuntimeError
//#define CL_ERR_RAMTransaction 11
//#define CL_ERR_InvalidCast 12
//#define CL_ERR_IllegalState 13
#define _CLFINALLY(x) catch(...) { x; throw; } x // note: code x is not run if return is called
#define _CLTHROWA(code, str) DEF_Exception(code, str)
//#define _CLTHROWA(number, str) throw CLuceneError(number, str, false)
#define _CLTHROWT(code, str) DEF_Exception(code, str)
//#define _CLTHROWA_DEL(number, str) throw CLuceneError(number, str, true) // throw a string ensures the value is deleted
//#define _CLTHROWT_DEL(number, str) throw CLuceneError(number, str, true) // throw a string ensures the value is deleted
#define CND_CONDITION(cond, usermessage) AppAssertMessage(cond, usermessage)
#define CND_PRECONDITION(cond, usermessage) AppAssertMessage(cond, usermessage)

// Constants / enum
#ifndef LUCENE_STATIC_CONSTANT
	//autoconf is not properly detecting the correct method for this, and since there's no real big
	//harm in always using an enum, we'll probably just make this the default.
	/*#if LUCENE_STATIC_CONSTANT_SYNTAX == 1
		#define LUCENE_STATIC_CONSTANT(type, assignment) static const type assignment
	#elif LUCENE_STATIC_CONSTANT_SYNTAX == 2*/
		#define LUCENE_STATIC_CONSTANT(type, assignment) enum { assignment }
	/*#else
		#error "LUCENE_STATIC_CONSTANT not defined, and/or LUCENE_STATIC_CONSTANT_SYNTAX is not defined to a valid value"
	#endif*/
#endif
//Analysis options
//maximum length that the CharTokenizer uses. Required.
//By adjusting this value, you can greatly improve the performance of searching
//and especially indexing. Default is 255, but smaller numbers will decrease
//the amount of memory used as well as increasing the speed.
#define  LUCENE_MAX_WORD_LEN 255
//Size of the CharTokenizer buffersize.
#define LUCENE_IO_BUFFER_SIZE 1024
//The initial value set to BooleanQuery::maxClauseCount. Default is 1024
#define LUCENE_BOOLEANQUERY_MAXCLAUSECOUNT 1024

// Strings
//#define TCHAR UChar // wchar_t
#define _T(x) x // L ## x or use U_STRING_DECL and U_STRING_INIT
// Formating functions from "unicode/ustdio.h"
#define _sntprintf u_snprintf_u // u_snprintf // Make a formatted a string
#define _tprintf u_sprintf_u // Print a formatted string
// String handling functions from "unicode/ustring.h"
#define _tcscpy u_strcpy // copy a string to another string
#define _tcsncpy u_strncpy // copy a specified amount of one string to another string.
#define _tcscat u_strcat // copy a string onto the end of the other string
#define _tcschr u_strchr // find location of one character
#define _tcsstr u_strstr // find location of a string
#define _tcslen u_strlen // get length of a string
#define _tcscmp u_strcmp // (Common::CollatorStrcmp) case sensitive compare two strings
#define _tcsncmp u_strncmp // (Common::CollatorStrncmp) case sensitive compare two strings (at most n characters)
#define _tcscspn u_strcspn // location of any of a set of character in a string
#define _tcsicmp(x, y) u_strcasecmp(x, y, 0) // case insensitive compare two string
// Conversion functions from <wchar.h>
//#define _tcstod wcstod // convert a string to a double see strtod
//#define _tcstoi64 wcstoll
int64_t lucene_tcstoi64(const UChar*, UChar**, int);
#define _tcstoi64 lucene_tcstoi64 // convers a string to an 64bit bit integer
#define _i64tot lucene_i64tot // converts a 64 bit integer to a string (with base)
// Unicode character properties from "unicode/uchar.h"
#define _istalnum u_isalnum
#define _istalpha u_isalpha
#define _istspace u_isspace
#define _istdigit u_isdigit
#define _totlower u_tolower // Full case mappings uStr.toLower() should be used whenever possible
#define _totupper u_toupper // Full case mappings uStr.toUpper() should be used whenever possible
// Str duplicate
char* lucenestrdup(const char* v CL_FILELINEPARAM);
UChar* lucenewcsdup(const UChar* v CL_FILELINEPARAM);
#define stringDuplicate(x) lucenewcsdup(x CL_FILELINE) //don't change this... uses [] instead of malloc
#define STRDUP_WtoW(x) lucenewcsdup(x CL_FILELINE)
#define STRDUP_TtoT STRDUP_WtoW
#define STRDUP_WtoT STRDUP_WtoW
#define STRDUP_TtoW STRDUP_WtoW
#define STRDUP_AtoW(x) CL_NS(util)::Misc::_charToWide(x CL_FILELINE)
#define STRDUP_AtoT STRDUP_AtoW
#define STRDUP_WtoA(x) CL_NS(util)::Misc::_wideToChar(x CL_FILELINE)
#define STRDUP_TtoA STRDUP_WtoA
#define STRCPY_WtoW(target,src,len) _tcsncpy(target,src,len)
#define STRCPY_TtoW STRCPY_WtoW
#define STRCPY_WtoT STRCPY_WtoW
#define STRCPY_TtoT STRCPY_WtoW
#define STRCPY_AtoW(target,src,len) CL_NS(util)::Misc::_cpycharToWide(src,target,len)
#define STRCPY_AtoT STRCPY_AtoW
#define STRCPY_WtoA(target,src,len) CL_NS(util)::Misc::_cpywideToChar(src,target,len)
#define STRCPY_TtoA STRCPY_WtoA
UChar* lucene_tcslwr(UChar* str);
#define _tcslwr lucene_tcslwr
//if a wide character is being converted to a ascii character and it
//cannot fit, this character is used instead. Required.
#define LUCENE_OOR_CHAR(c) ((char)(((unsigned short)c)&0xFF))
///a blank string...
extern UChar* _LUCENE_BLANK_STRING;
#define LUCENE_BLANK_STRING _LUCENE_BLANK_STRING
extern char* _LUCENE_BLANK_ASTRING;
#define LUCENE_BLANK_ASTRING _LUCENE_BLANK_ASTRING
//This must always be defined. They can be adjusted if required. But
//general Wildcard string would be '*' and Wildcard Char would be '?'
//Both are Required.
#define LUCENE_WILDCARDTERMENUM_WILDCARD_STRING '*'
#define LUCENE_WILDCARDTERMENUM_WILDCARD_CHAR   '?'
#define LUCENE_SEGMENTTERMENUM_GROWSIZE 8

// Threads
#define SCOPED_LOCK_MUTEX(theMutex)
#define DEFINE_MUTEX(x)
#define STATIC_DEFINE_MUTEX(x)
#define _LUCENE_SLEEP(x)
#define _LUCENE_CURRTHREADID 1
#define _LUCENE_THREADID_TYPE char

// min / max
/*
namespace std
{
//# undef min // just in case
//# undef max // just in case
#define min(a,b) (a>b?b:a)
#define max(a,b) (a>b?a:b)
}
*/

// Types
//typedef long double float_t;
#ifdef _WIN32
	typedef float float_t;
#endif 
/*
# ifdef __GNUC__
#  if  defined(__STRICT_ANSI__)
typedef float float_t;
typedef double double_t;
#  else
typedef double float_t;
typedef double double_t;
#  endif
# else
typedef double float_t;
typedef double double_t;
# endif
*/

//------------------------------------------------------------------------------
#endif
