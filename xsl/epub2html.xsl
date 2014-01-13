<?xml version="1.0" ?>
<xsl:stylesheet
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:opf="http://www.idpf.org/2007/opf"
	xmlns:dc="http://purl.org/dc/elements/1.1/"
	xmlns="http://www.w3.org/1999/xhtml"
	exclude-result-prefixes="opf"
	xml:lang="en"
	version="1.0">

	<xsl:output
		method="xml"
		indent="no"
		encoding="utf-8"
		doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"
		doctype-public="-//W3C//DTD XHTML 1.1//EN"
		/>

	<xsl:template match="/opf:package">
<html xmlns="http://www.w3.org/1999/xhtml">
	<head>
		<title><xsl:call-template name="get_title"/></title>
		<meta name="author" content="atoll-digital-library.org" />
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
		<style type='text/css'>
		div, p {
			margin: 0;
			padding: 0;
		}
		</style>
	</head>
	<body>
		<h1><xsl:call-template name="get_title"/></h1>
		<h2><xsl:call-template name="get_author"/></h2>
		<p>
			<b>Date de publication: </b><xsl:call-template name="get_date"/><br />
			<b>Source: </b><xsl:call-template name="get_source"/><br />
			<b>Editeur: </b><xsl:call-template name="get_publisher"/><br />
			<b>Url: </b><xsl:call-template name="get_url"/><br />
		</p>
		<xsl:apply-templates select="/opf:package/opf:manifest"/>
	</body>
</html>
	</xsl:template>

	<xsl:template name="get_title">
		<xsl:value-of select="/opf:package/opf:metadata/dc:title"/>
	</xsl:template>
	
	<xsl:template name="get_author">
		<xsl:value-of select="/opf:package/opf:metadata/dc:creator"/>
	</xsl:template>
	
	<xsl:template name="get_date">
		<xsl:value-of select="/opf:package/opf:metadata/dc:date[@opf:event='original-publication']"/>
	</xsl:template>
	
	<xsl:template name="get_source">
		<xsl:value-of select="/opf:package/opf:metadata/dc:source"/>
	</xsl:template>
	
	<xsl:template name="get_publisher">
		<xsl:value-of select="/opf:package/opf:metadata/dc:publisher"/>
	</xsl:template>
	
	<xsl:template name="get_url">
		<xsl:value-of select="/opf:package/opf:metadata/dc:identifier[@opf:scheme='URI']"/>
	</xsl:template>
	
  <xsl:template match="opf:metadata" />

	<xsl:template match="opf:manifest"><xsl:apply-templates /></xsl:template>

	<!-- Insert the book documents -->
  <xsl:template match="opf:item">
		<xsl:if test="@media-type = 'application/xhtml+xml'">
			<xsl:variable name="docname" select="@href" />
			<xsl:choose>
				<xsl:when test="$docname = 'cover.xml'"></xsl:when>
				<xsl:when test="$docname = 'title.xml'"></xsl:when>
				<xsl:when test="$docname = 'about.xml'"></xsl:when>
				<xsl:when test="$docname = 'similar.xml'"></xsl:when>
				<xsl:when test="$docname = 'feedbooks.xml'"></xsl:when>
				<xsl:otherwise>
					<div class="partie" xmlns="http://www.w3.org/1999/xhtml">
						<xsl:apply-templates select="document($docname)"/>
					</div>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:if>
  </xsl:template>

	<xsl:template match="html"><xsl:apply-templates /></xsl:template>

  <xsl:template match="head" />

	<xsl:template match="body"><xsl:apply-templates /></xsl:template>

  <xsl:template match="div"><xsl:apply-templates /></xsl:template>

	<!--
  <xsl:template match="a"><a href="{@href}" xmlns="http://www.w3.org/1999/xhtml"><xsl:apply-templates /></a></xsl:template>
	-->
	<xsl:template match="a" />

  <xsl:template match="@*|node()">
    <xsl:copy>
      <xsl:apply-templates select="@*|node()"/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>
