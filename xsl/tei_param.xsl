<?xml version="1.0" encoding="utf-8" standalone="yes" ?>
<xsl:stylesheet
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:xd="http://www.pnp-software.com/XSLTdoc"
	xmlns:tei="http://www.tei-c.org/ns/1.0"
	exclude-result-prefixes="tei xd"
	version="1.0">

<!--
	tei_param.xsl
	// 18/02/11 Version 1.1 Begining of implementation
-->

<xsl:output
	method="html"
	indent="no"
	encoding="utf-8"
	/>

<!-- ==================================================================== -->

<!-- Set parameters with their default value -->
<xsl:param name="atoll.highwords"></xsl:param>
<xsl:param name="cssFile">./tei_atoll.css</xsl:param>
<xsl:param name="cssPrintFile"></xsl:param>
<xsl:param name="pageLayout"></xsl:param>
<xsl:param name="topNavigationPanel">false</xsl:param>
<xsl:param name="bottomNavigationPanel">false</xsl:param>
<xsl:param name="autoToc">false</xsl:param>
<xsl:param name="pagebreakStyle">none</xsl:param>
<xsl:param name="generateParagraphIDs">false</xsl:param>
<xsl:template name="stdfooter"><xsl:param name="file"/></xsl:template>
<xsl:template name="generateLocalCSS"></xsl:template>

<!-- ==================================================================== -->

<xsl:template match="tei:w|w">
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
