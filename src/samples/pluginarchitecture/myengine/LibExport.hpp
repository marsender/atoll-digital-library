/*******************************************************************************

 LibExport.hpp

 Copyright © 2010 Didier Corbiere

 Distributable under the terms of the GNU Lesser General Public License,
 as specified in the COPYING file.

*******************************************************************************/

#ifndef __LibExport_HPP
#define	__LibExport_HPP

#if defined(WIN32)
	#ifdef DEF_LibExport
		#define DEF_Export __declspec(dllexport)
	#else
		#define DEF_Export __declspec(dllimport)
	#endif
#else
	#define DEF_Export
#endif

#endif // __LibExport_HPP
