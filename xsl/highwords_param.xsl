<?xml version="1.0" encoding="utf-8" standalone="yes" ?>
<xsl:stylesheet
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:html="http://www.w3.org/1999/xhtml"
	version="1.0">

<!--
	highwords_param.xsl
	// 19/08/10 Version 1.0 Begining of implementation
-->

<xsl:output
	method="html"
	indent="no"
	encoding="utf-8"
	/>

<!-- ==================================================================== -->

<!-- Set parameters with their default value -->
<xsl:param name="atoll.highwords"></xsl:param>

<!-- ==================================================================== -->

<xsl:template match="@*|node()">
	<xsl:copy>
		<xsl:apply-templates select="@*|node()"/>
	</xsl:copy>
</xsl:template>

<xsl:template match="html:w|w">
  <xsl:choose>
		<xsl:when test="contains($atoll.highwords, concat(' ', @pos, ' '))">
			<span id="highlight_{@pos}" class="highlight"><xsl:apply-templates/></span>
		</xsl:when>
		<xsl:otherwise>
			<xsl:apply-templates/>
		</xsl:otherwise>
  </xsl:choose>
</xsl:template>

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

</xsl:stylesheet>
