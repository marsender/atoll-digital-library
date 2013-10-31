/*******************************************************************************

 Copyright © 2010 Didier Corbiere

 Distributable under the terms of the GNU Lesser General Public License,
 as specified in the COPYING file.

getopt.cpp

Get program options for platforms without this function

#ifdef _WIN32
extern "C" {
  extern int getopt(int, char * const *, const char *);
  extern char *optarg;
}
#else
#include <unistd.h>
#endif

*******************************************************************************/

#ifdef _WIN32

#include <stdio.h>
#include <string.h>

#undef	BADCH
#define	BADCH (int)'?'
#undef	BADARG
#define	BADARG (int)':'
#undef	EMSG
#define	EMSG	""
char	*optarg = NULL; // argument associated with option
//! Parse argc/argv argument vector.
int getopt(int nargc, char * const *nargv, const char *ostr)
{
	static char *staticProgname = NULL;
	static char *staticPlace = EMSG; // option letter processing
	static int staticOptind = 1; // index into parent argv vector

	char *oli; // Option letter list index
	int	opterr = 1; // If error message should be printed
	int optopt = 0; // Character checked for validity

	if (staticProgname == NULL) {
		staticProgname = strrchr(nargv[0], '/');
		if (staticProgname == NULL)
			staticProgname = strrchr(nargv[0], '\\');
		if (staticProgname)
			staticProgname++;
		else
			staticProgname = nargv[0];
	}

	if (!*staticPlace) {		/* update scanning pointer */
		if (staticOptind >= nargc || *(staticPlace = nargv[staticOptind]) != '-') {
			staticPlace = EMSG;
			return (EOF);
		}
		if (staticPlace[1] && *++staticPlace == '-') {	/* found "--" */
			++staticOptind;
			staticPlace = EMSG;
			return (EOF);
		}
	}					/* option letter okay? */
	if ((optopt = (int) * staticPlace++) == (int)':' ||
	    !(oli = (char *)strchr(ostr, optopt))) {
		/*
		 * if the user didn't specify '-' as an option,
		 * assume it means EOF.
		 */
		if (optopt == (int)'-')
			return (EOF);
		if (!*staticPlace)
			++staticOptind;
		if (opterr && *ostr != ':')
			(void)fprintf(stderr, "%s: illegal option -- %c\n", staticProgname, optopt);
		return (BADCH);
	}
	if (*++oli != ':') {			/* don't need argument */
		optarg = NULL;
		if (!*staticPlace)
			++staticOptind;
	}
	else {					/* need an argument */
		if (*staticPlace)			/* no white space */
			optarg = staticPlace;
		else if (nargc <= ++staticOptind) {	/* no arg */
			staticPlace = EMSG;
			if (*ostr == ':')
				return (BADARG);
			if (opterr)
				(void)fprintf(stderr, "%s: option requires an argument -- %c\n", staticProgname, optopt);
			return (BADCH);
		}
		else				/* white space */
			optarg = nargv[staticOptind];
		staticPlace = EMSG;
		++staticOptind;
	}

	return (optopt);			/* dump back option letter */
}
//------------------------------------------------------------------------------

#endif // _WIN32
