<?xml version="1.0" encoding="utf-8" standalone="yes" ?>
<xsl:stylesheet
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:html="http://www.w3.org/1999/xhtml"
	version="1.0">

<!--
	identity_param.xsl
	// 19/08/2010 Version 1.0 Begining of implementation
-->

<xsl:output
	method="html"
	indent="no"
	encoding="utf-8"
	/>

<!-- ==================================================================== -->

<!-- Set parameters with their default value -->

<!-- ==================================================================== -->

<xsl:template match="@*|node()">
	<xsl:copy>
		<xsl:apply-templates select="@*|node()"/>
	</xsl:copy>
</xsl:template>

<!-- Remove word positions -->
<xsl:template match="html:w|w">
	<xsl:apply-templates/>
</xsl:template>

<!-- Remove ocr positions -->
<xsl:template match="html:wd|wd">
	<xsl:apply-templates/>
</xsl:template>

<!--
<xsl:template match="html:a|a">
  <xsl:choose>
		<xsl:when test="@rel='external'">
			<xsl:copy-of select="." />
		</xsl:when>
		<xsl:otherwise>
			<xsl:apply-templates/>
		</xsl:otherwise>
  </xsl:choose>
</xsl:template>
-->
</xsl:stylesheet>
