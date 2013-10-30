/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

See the file LicenseBerkeleyDB.txt for redistribution information.
Copyright (c) 2002,2006 Oracle. All rights reserved.

AppException.hpp

*******************************************************************************/

#ifndef __AppException_HPP
#define __AppException_HPP

#include "LibExport.hpp"
#include <string>
#include <exception> 
//------------------------------------------------------------------------------

#define DEF_Exception(x, y) throw Common::AppException((x), (y), __FILE__, __LINE__)
//------------------------------------------------------------------------------

namespace Common
{

//! Application exception
/*!
	Example code:
	try {
		DEF_Exception(AppException::InternalError, "Test");
		...
		std::ostringstream oss;
		oss << "test";
		DEF_Exception(AppException::InternalError, oss.str());
	}
	catch (AppException &e) {
		gLog.log(eTypLogError, "Err > Application exception: %s", e.what());
		return false;
	}
*/
class AppException : public std::exception
{
public:
	//! Error codes
	enum ExceptionCode
	{
		ExceptionCodeNone,       //!< Don't show exception code
		AssertionError,          //!< Assertion error
		InternalError,           //!< Internal error
		RuntimeError,            //!< Runtime error
		NoMemoryError,           //!< Unable to allocate memory
		UnimplementedError,      //!< Unimplemented
		SerialisationError,      //!< Serialisation error
		QueryParserError,        //!< Query parser error
		NullPointer,             //!< Null pointer
		InvalidValue,            //!< Invalid value
		InvalidOperation,        //!< Invalid operation
		InvalidState,            //!< Invalid state
		InvalidArgument,         //!< Invalid argument
		OutOfBounds,             //!< Out of bounds
		IoError,                 //!< IO error
		FileError,               //!< File error
		ParserError,             //!< Parser error
		UnitTestError,           //!< Unit test error
		BerkeleyDbError,         //!< Berkeley DB database error
		AtollIndexerError,       //!< Atoll indexer error
		ExceptionCodeEnd
	};
	DEF_Export AppException(ExceptionCode err, const std::string &description, const char *file = 0, int line = 0);
	DEF_Export AppException(ExceptionCode err, const char *description, const char *file = 0, int line = 0);
	DEF_Export AppException(const AppException &);
	DEF_Export virtual ~AppException() throw();
	virtual const char *what() const throw();
	//! Get the the error code for this exception.
	ExceptionCode getExceptionCode() const { return exceptionCode_; }
	const char *getQueryFile() const { return qFile_; }
	int getQueryLine() const { return qLine_; }
	int getQueryColumn() const { return qCol_; }

private:
	// No need for assignment
	AppException &operator=(const AppException &);

	void describe();

	ExceptionCode exceptionCode_;
	char *description_;

	char *qFile_;
	int qLine_, qCol_;

	const char *file_; // The file where the bad thing happened.
	int line_; // The line where the bad thing happened.

	char *text_;
};

} // namespace Common

//------------------------------------------------------------------------------
#endif
