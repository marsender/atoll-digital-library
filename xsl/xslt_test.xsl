<?xml version="1.0" encoding="utf-8" standalone='yes' ?>

<!--
	xslt_test.xsl
	// 16/09/09 Version 1.0 Begining of implementation
-->

<!DOCTYPE xsl:stylesheet [
  <!ENTITY nbsp '&#160;'>
]>

<xsl:stylesheet
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns="http://www.w3.org/1999/xhtml"
	version="1.0">

<xsl:output
	method="xml"
	indent="no"
	encoding="utf-8"
	doctype-public="-//W3C//DTD XHTML 1.1//EN"
	doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"
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

<xsl:template match="w">
  <xsl:choose>
		<xsl:when test="contains($atoll.highwords, concat(' ', @pos, ' '))">
			<span class="highlight"><xsl:apply-templates/></span>
		</xsl:when>
		<xsl:otherwise>
			<xsl:apply-templates/>
		</xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>
