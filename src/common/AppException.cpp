/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

See the file LicenseBerkeleyDB.txt for redistribution information.
Copyright (c) 2002,2006 Oracle. All rights reserved.

AppException.cpp

*******************************************************************************/

#include "AppException.hpp"
#include <sstream> // For std::ostringstream 
#include <stdlib.h>
#include <string.h>
#if defined(_MSC_VER)
#pragma warning(disable : 4996) // warning: deprecation of strdup() in VS8
#endif // !_MSC_VER
//------------------------------------------------------------------------------

using namespace Common;

static const char *GetExceptionCodeName(AppException::ExceptionCode inEc)
{
  switch (inEc) {
	case AppException::ExceptionCodeNone:
		return "";
  case AppException::AssertionError:
    return "Assertion error";
  case AppException::InternalError:
    return "Internal error";
  case AppException::RuntimeError:
    return "Runtime error";
  case AppException::NoMemoryError:
    return "Unable to allocate memory";
	case AppException::UnimplementedError:
		return "Unimplemended error";
	case AppException::SerialisationError:
		return "Serialisation error";
	case AppException::QueryParserError:
		return "Query parser error";
  case AppException::NullPointer:
    return "Null pointer";
  case AppException::InvalidValue:
    return "Invalid value";
	case AppException::InvalidOperation:
		return "Invalid operation";
	case AppException::InvalidState:
		return "Invalid state";
	case AppException::InvalidArgument:
		return "Invalid argument";
	case AppException::OutOfBounds:
		return "Out of bounds";
  case AppException::IoError:
    return "IO error";
  case AppException::FileError:
    return "File error";
  case AppException::ParserError:
    return "Parser error";
  case AppException::UnitTestError:
    return "Unit test error";
  case AppException::BerkeleyDbError:
    return "Berkeley DB database error";
	case AppException::AtollIndexerError:
		return "Atoll indexer error";
	default:
		break;
  }

  return "Undefined code";
}
//------------------------------------------------------------------------------

// jcm - Note that the member variable description is not a std::string
// because in some cases when the AppException object is deleted an exception
// is thrown by the MSVC6 debug heap. Using strdup and free seems to work ok.
AppException::AppException(ExceptionCode ec, const std::string &description, const char *file, int line)
    : exceptionCode_(ec),
    description_(::strdup(description.c_str())),
    qFile_(0),
    qLine_(0),
    qCol_(0),
    file_(file),
    line_(line),
    text_(0)
{
  describe();
}
//------------------------------------------------------------------------------

AppException::AppException(ExceptionCode ec, const char *description, const char *file, int line)
    : exceptionCode_(ec),
    description_(::strdup(description)),
    qFile_(0),
    qLine_(0),
    qCol_(0),
    file_(file),
    line_(line),
    text_(0)
{
  describe();
}
//------------------------------------------------------------------------------

AppException::AppException(const AppException &that)
    : std::exception(that),
    exceptionCode_(that.exceptionCode_),
    description_(::strdup(that.description_)),
    qFile_(that.qFile_ ? ::strdup(that.qFile_) : 0),
    qLine_(that.qLine_),
    qCol_(that.qCol_),
    file_(that.file_),
    line_(that.line_),
    text_(::strdup(that.text_))
{
}
//------------------------------------------------------------------------------

AppException::~AppException() throw()
{
  file_ = 0;
  ::free(description_);
  if (qFile_ != 0)
    ::free(qFile_);
  if (text_ != 0)
    ::free(text_);
}
//------------------------------------------------------------------------------

void AppException::describe()
{
  std::ostringstream s;
  //s << "AppException: ";
	if (exceptionCode_ != AppException::ExceptionCodeNone)
		s << GetExceptionCodeName(exceptionCode_) << " exception: ";
  s << description_;
  if (qFile_ != 0 || qLine_ != 0) {
    s << ", ";
    if (qFile_ != 0)
      s << qFile_;
    else
      s << "<query>";
    if (qLine_ != 0) {
      s << ":" << qLine_;
      if (qCol_ != 0)
        s << ":" << qCol_;
    }
  }
  if (file_ != 0) {
    s << " - File: ";
    s << file_;

    if (line_ != 0) {
      s << " Line: ";
      s << line_;
    }
  }
  text_ = ::strdup(s.str().c_str());
}
//------------------------------------------------------------------------------

const char *AppException::what() const throw()
{
  return text_;
}
//------------------------------------------------------------------------------
