#
# Doxygen.cmake
#
# This file provides support for building documentation
# To build the documention, you will have to enable "ENABLE_DOCS" option
# Then do the equivalent of "make doc"
#

OPTION(ENABLE_DOCS "Build documentation" ON)

MACRO(SET_YESNO)
	FOREACH(param ${ARGV})
		IF (${param})
			SET(${param} "YES")
		ELSE (${param})
			SET(${param} "NO")
		ENDIF (${param})
	ENDFOREACH(param)
ENDMACRO(SET_YESNO)
MACRO(SET_BLANK)
	FOREACH(param ${ARGV})
		IF (NOT ${param})
			SET(${param} "")
		ENDIF (NOT ${param})
	ENDFOREACH(param)
ENDMACRO(SET_BLANK)

IF (ENABLE_DOCS)
	OPTION(CLDOCS_HTML_HELP "Doxygen should compile HTML into a Help file (CHM)." NO)
	OPTION(CLDOCS_HTML      "Doxygen should build HTML documentation." YES)
	OPTION(CLDOCS_XML       "Doxygen should build XML documentation." NO)
	OPTION(CLDOCS_RTF       "Doxygen should build RTF documentation." NO)
	OPTION(CLDOCS_MAN       "Doxygen should build man documentation." NO)
	OPTION(CLDOCS_TAGFILE   "Doxygen should build a tagfile." NO)
	OPTION(CLDOCS_LATEX     "Doxygen should build Latex documentation." NO)

	# Check for the tools
	message(STATUS "Looking for Doxygen")
	FIND_PACKAGE(Doxygen)

	IF (DOXYGEN_FOUND)

		# This creates a new target to build documentation.
		# It runs ${DOXYGEN_EXECUTABLE} which is the full path and executable to
		# Doxygen on your system, set by the FindDoxygen.cmake module
		# (called by FindDocumentation.cmake).
		# It runs the final generated Doxyfile against it.
		# The DOT_PATH is substituted into the Doxyfile.
		ADD_CUSTOM_TARGET(doc
			COMMENT "Make doxygen documentation"
			COMMAND "${DOXYGEN_EXECUTABLE}" "${PROJECT_SOURCE_DIR}/doc/doxygen/Doxyfile"
			)

		IF (CLDOCS_HTML_HELP)
			IF (NOT CLDOCS_HTML)
				MESSAGE(STATUS "CLDOCS_HTML is required to buidl CLDOCS_HTML_HELP")
			ENDIF (NOT CLDOCS_HTML)
			FIND_PACKAGE(HTMLHelp)
			IF (NOT HTML_HELP_COMPILER)
				MESSAGE(STATUS "HTML Help compiler not found, turn CLDOCS_HTML_HELP off to proceed")
			ENDIF (NOT HTML_HELP_COMPILER)

			# Make cygwin work with hhc...
			IF (CYGWIN)
				EXECUTE_PROCESS (COMMAND cygpath "${HTML_HELP_COMPILER}"
				OUTPUT_VARIABLE HTML_HELP_COMPILER_EX)
				STRING (REPLACE "\n" "" HTML_HELP_COMPILER_EX ${HTML_HELP_COMPILER_EX})
				STRING (REPLACE "\r" "" HTML_HELP_COMPILER_EX ${HTML_HELP_COMPILER_EX})
				SET (HTML_HELP_COMPILER_EX "\"${HTML_HELP_COMPILER_EX}\"")
			ELSE (CYGWIN)
				SET (HTML_HELP_COMPILER_EX ${HTML_HELP_COMPILER})
			ENDIF (CYGWIN)
		ENDIF (CLDOCS_HTML_HELP)

		IF (CLDOCS_LATEX)
			FIND_PACKAGE(LATEX)
			IF (NOT LATEX_COMPILER)
				MESSAGE(STATUS "Latex compiler not found, turn CLDOCS_LATEX off to proceed")
			ENDIF (NOT LATEX_COMPILER)
		ENDIF (CLDOCS_LATEX)

		FIND_PACKAGE(Perl)

		IF (DOXYGEN_DOT_EXECUTABLE)
			SET (HAVE_DOT "YES")
		ELSE (DOXYGEN_DOT_EXECUTABLE)
			SET (HAVE_DOT "NO")
		ENDIF (DOXYGEN_DOT_EXECUTABLE)

		# Doxygen expects YES/NO parameters
		SET_YESNO(
			CLDOCS_HTML_HELP
			CLDOCS_LATEX
			CLDOCS_XML
			CLDOCS_HTML
			CLDOCS_RTF
			CLDOCS_MAN
			)
		# Empty out paths if not found
		SET_BLANK(
			PERL_EXECUTABLE
			DOXYGEN_DOT_EXECUTABLE
			HTML_HELP_COMPILER
			LATEX_COMPILER
			)

		IF (CLDOCS_TAGFILE)
			SET (CLDOCS_TAGFILE_LOCATION "${PROJECT_SOURCE_DIR}/doc/tag/${PROJECT_NAME}.tag" )
		ENDIF (CLDOCS_TAGFILE)

		# Generate the final Doxyfile
		CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/doc/doxygen/Doxygen.cfg ${PROJECT_SOURCE_DIR}/doc/doxygen/Doxyfile)

		# Create a target for tar.gz html help
		FIND_PACKAGE(UnixCommands)

		IF (TAR AND GZIP)
			ADD_CUSTOM_TARGET(doc-tarz
				COMMENT "Make an archive of the html documentation"
				COMMAND "${TAR}" -cf ${PROJECT_SOURCE_DIR}/doc/${PROJECT_NAME}-doc.tar -C ${PROJECT_SOURCE_DIR}/doc html
				COMMAND "${GZIP}" -f ${PROJECT_SOURCE_DIR}/doc/${PROJECT_NAME}-doc.tar
				DEPENDS doc
				)
		ENDIF (TAR AND GZIP)

		# Install man if it was built
		IF (CLDOCS_MAN)
			INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/doc/man/ DESTINATION man)
		ENDIF (CLDOCS_MAN)

	ELSE (DOXYGEN_FOUND)
		MESSAGE(STATUS "Doxygen not found, turn ENABLE_DOCS off to proceed")
	ENDIF (DOXYGEN_FOUND)

	message(STATUS "")

ENDIF (ENABLE_DOCS)
