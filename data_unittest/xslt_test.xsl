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

<xsl:template match="/">
<html>
<head>
	<title>Plugin xslt</title>
	<meta name="author" content="atoll-digital-library.org" />
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
	<style type='text/css'>
div, p {
	margin: 0;
	padding: 0;
}
.title {
	font-weight: bold;
}
.titleabbrev {
	font-style: italic;
}
.important {
	color: #FF0000;
}
.tip {
	color: #8F6B47;
}
span.exp {
	vertical-align: 30%;
	font-size: 80%;
}
	</style>
</head>
<body>
	<div class="content">
		<p>Plugin xslt</p>
  	<xsl:apply-templates select="book"/>
	</div>
</body>
</html>
</xsl:template>

<xsl:template match="br">
	<br/>
</xsl:template>

<xsl:template match="title">
	<div class="title"><xsl:apply-templates /></div>
</xsl:template>

<xsl:template match="titleabbrev">
	<div class="titleabbrev"><xsl:apply-templates /></div>
</xsl:template>

<xsl:template match="para">
	<p><xsl:apply-templates /></p>
</xsl:template>

<xsl:template match="important">
	<div class="important"><xsl:apply-templates /></div>
</xsl:template>

<xsl:template match="tip">
	<div class="tip"><xsl:apply-templates /></div>
</xsl:template>

<xsl:template match="e">
	<span class="exp"><xsl:apply-templates /></span>
</xsl:template>

<xsl:template match="w">
	<xsl:apply-templates />
</xsl:template>

</xsl:stylesheet>
