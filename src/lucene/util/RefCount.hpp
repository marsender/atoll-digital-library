/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
*
* Distributable under the terms of either the Apache License (Version 2.0) or
* the GNU Lesser General Public License, as specified in the LICENSE file.
------------------------------------------------------------------------------*/
#ifndef _lucene_debug_refcount_
#define _lucene_debug_refcount_

#ifdef _LUCENE_PRAGMA_ONCE
# pragma once
#endif

CL_NS_DEF(util)

class RefCount
{
public:
	int __cl_refcount;
	RefCount() {
		__cl_refcount = 1;
	}
	inline int __cl_getref() const {
		return __cl_refcount;
	}
	inline int __cl_addref() {
		__cl_refcount++;
		return __cl_refcount;
	}
	inline int __cl_decref() {
		__cl_refcount--;
		return __cl_refcount;
	}
	virtual ~RefCount() {};
};

CL_NS_END
#endif //_lucene_debug_refcount_
