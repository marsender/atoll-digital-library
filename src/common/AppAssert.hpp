/*******************************************************************************

Copyright © 2010 Didier Corbiere

Distributable under the terms of the GNU Lesser General Public License,
as specified in the LICENSE file.

AppAssert.hpp

*******************************************************************************/

#ifndef __AppAssert_HPP
#define	__AppAssert_HPP
//------------------------------------------------------------------------------

#include "LibExport.hpp"

namespace Common
{

//! Assertion
/**
	Example code:
		AppAssert(mNumDoc == 0);
 */

#ifdef DEF_EnableAsserts
	//! Action if assertion fails
	DEF_Export void AssertFail(const char *inExpression, const char *inFile, int inLine);
	DEF_Export void AssertFailMessage(const char *inExpression, const char *inMessage, const char *inFile, int inLine);

	/*
	static inline void CheckNullPointer(const void *ptr, const char *message)
	{
		if (ptr == 0) {
			std::string mesg = "Attempt to use uninitialized object: ";
			mesg += message;
			throw AppException(AppException::NullPointer, mesg);
		}
	}
	*/

	#define AppAssert(expr) ((void)((expr) ? 0 : (Common::AssertFail(#expr, __FILE__, __LINE__), 0)))
	#define AppAssertMessage(expr, mess) ((void)((expr) ? 0 : (Common::AssertFailMessage(#expr, mess, __FILE__, __LINE__), 0)))

	/** A "compile-time" assertion.
	 *
	 *  STATIC_ASSERT must be used inside a function (not at the top level, or
	 *  in a class definition).
	 *
	 *  COND must be a compile-time constant expression.
	 *
	 *  If COND is false, the compiler will try to compile an array with negative
	 *  length, which is invalid (we don't use 0 as the length, since GCC allows
	 *  zero length arrays as an extension).  We name the array
	 *  "static_assert_failed" since this won't clash with any sanely named
	 *  variable, and it's likely to appear in the compiler error message and so
	 *  indicate to the developer what went wrong.
	 */
	#define STATIC_ASSERT(COND) \
		do { \
			char static_assert_failed[(COND) ? 1 : -1]; \
			(void)static_assert_failed; \
		} while (0)

	/** Assert at compile-time that type TYPE is unsigned. */
	#define STATIC_ASSERT_UNSIGNED_TYPE(TYPE) STATIC_ASSERT(static_cast<TYPE>(-1) > 0)

#else // DEF_EnableAsserts
	#define AppAssert(expr) ((void)(0))
	#define AppAssertMessage(expr, mess) ((void)(0))
	#define STATIC_ASSERT_UNSIGNED_TYPE(expr) ((void)(0))
#endif

} // namespace Common

//------------------------------------------------------------------------------
#endif
