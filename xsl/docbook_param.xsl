<?xml version="1.0" encoding="utf-8" standalone="yes" ?>
<xsl:stylesheet
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:d="http://docbook.org/ns/docbook"
	xmlns:doc="http://nwalsh.com/xsl/documentation/1.0"
	exclude-result-prefixes="doc d"
	version="1.0">

<!--
	docbook_param.xsl
	// 22/09/09 Version 1.1 Begining of implementation
-->

<xsl:output
	method="html"
	indent="no"
	encoding="utf-8"
	/>

<!-- ==================================================================== -->

<!-- Set parameters with their default value -->
<xsl:param name="atoll.highwords"></xsl:param>
<xsl:param name="generate.toc">0</xsl:param>
<xsl:param name="suppress.navigation">1</xsl:param>
<xsl:param name="html.stylesheet">./docbook_atoll.css</xsl:param>
<xsl:param name="l10n.gentext.language">fr</xsl:param>

<!-- ==================================================================== -->

<xsl:template match="d:w|w">
  <xsl:choose>
		<xsl:when test="contains($atoll.highwords, concat(' ', @pos, ' '))">
			<span id="highlight_{@pos}" class="highlight"><xsl:apply-templates/></span>
		</xsl:when>
		<xsl:otherwise>
			<xsl:apply-templates/>
		</xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>
