<?xml version="1.0" encoding="UTF-8"?>
<!--

This stylesheet is specifically designed for ADL documentation XHTML.

Based upon stylesheets originally developed by Antenna House.

-->

<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:html="http://www.w3.org/1999/xhtml">

  <xsl:import href="xhtml2fo.xsl"/>

  <xsl:output method="xml"
              version="1.0"
              encoding="UTF-8"
              indent="no"/>

  <!--======================================================================
      Parameters
  =======================================================================-->

  <!-- page size -->
  <xsl:param name="page-width">auto</xsl:param>
  <xsl:param name="page-height">auto</xsl:param>
  <xsl:param name="page-margin-top">1in</xsl:param>
  <xsl:param name="page-margin-bottom">1in</xsl:param>
  <xsl:param name="page-margin-left">1in</xsl:param>
  <xsl:param name="page-margin-right">1in</xsl:param>

  <!-- page header and footer -->
  <xsl:param name="page-header-margin">0.5in</xsl:param>
  <xsl:param name="page-footer-margin">0.5in</xsl:param>
  <xsl:param name="title-print-in-header">true</xsl:param>
  <xsl:param name="page-number-print-in-footer">true</xsl:param>

  <!-- writing-mode: lr-tb | rl-tb | tb-rl -->
  <xsl:param name="writing-mode">lr-tb</xsl:param>

  <!-- text-align: justify | start -->
  <xsl:param name="text-align">justify</xsl:param>

  <!--======================================================================
      Attribute Sets
  =======================================================================-->

  <!--=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
       Root
  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-->

  <xsl:attribute-set name="root">
    <xsl:attribute name="writing-mode"><xsl:value-of select="$writing-mode"/></xsl:attribute>
    <xsl:attribute name="text-align"><xsl:value-of select="$text-align"/></xsl:attribute>
    <!-- specified on fo:root to change the properties' initial values -->
  </xsl:attribute-set>

  <xsl:attribute-set name="page">
    <xsl:attribute name="page-width"><xsl:value-of select="$page-width"/></xsl:attribute>
    <xsl:attribute name="page-height"><xsl:value-of select="$page-height"/></xsl:attribute>
    <!-- specified on fo:simple-page-master -->
  </xsl:attribute-set>

  <xsl:attribute-set name="body">
    <!-- specified on fo:flow's only child fo:block -->
    <!--
    <xsl:attribute name="background-color">#F0F0F0</xsl:attribute>
    <xsl:attribute name="padding-start">6pt</xsl:attribute>
    <xsl:attribute name="padding-end">6pt</xsl:attribute>
    -->
  </xsl:attribute-set>

  <xsl:attribute-set name="page-header">
    <!-- specified on (page-header)fo:static-content's only child fo:block -->
    <xsl:attribute name="font-size">small</xsl:attribute>
    <xsl:attribute name="text-align">center</xsl:attribute>
  </xsl:attribute-set>

  <xsl:attribute-set name="page-footer">
    <!-- specified on (page-footer)fo:static-content's only child fo:block -->
    <xsl:attribute name="font-size">small</xsl:attribute>
    <xsl:attribute name="text-align">center</xsl:attribute>
  </xsl:attribute-set>

	<!--=============================================================================
		 ADL specific transformations.
		 ===========================================================================-->

	<!-- Due to prest weirdness, we drop all ids and names that aren't specific
	adl targets, in order to avoid duplicates.  Yuck- change this when we have a
	better algorithm for creating unique names in prest. -->
	<xsl:template name="process-id">
    <xsl:param name="id"/>
    <xsl:choose>
			<!-- DIV IDs are dropped to avoid a conflict with how reStructuredText works. -->
      <xsl:when test="self::html:div">
      </xsl:when>
			<!-- We drop names that are children of h3 and h4 nodes, since these are
			generated automatically from instruction, etc. names and thus might have
			duplicates. -->
			<xsl:when test="self::html:a and (parent::html:h3 or parent::html:h4)">
			</xsl:when>
			<!-- Otherwise, the id or name becomes the new id value. -->
      <xsl:otherwise>
        <xsl:attribute name="id">
          <xsl:value-of select="$id"/>
        </xsl:attribute>
			</xsl:otherwise>
    </xsl:choose>
	</xsl:template>

	<!-- Class processing.   Basically, we're just duplicating the styles from
			 adldoc.css into fo attributes. -->
  <xsl:template name="class-template">
    <xsl:param name="class"/>
		<xsl:choose>
			<xsl:when test="$class ='field' and self::html:table">
				<xsl:attribute name="font-size">x-small</xsl:attribute>
				<xsl:attribute name="text-align">center</xsl:attribute>
				<xsl:attribute name="border-collapse">collapse</xsl:attribute>
				<xsl:attribute name="border">0</xsl:attribute>
			</xsl:when>
			<xsl:when test="self::html:table and ($class ='subfield' or $class = 'subfieldunused') ">
				<xsl:attribute name="text-align">center</xsl:attribute>
				<xsl:attribute name="border-collapse">collapse</xsl:attribute>
				<xsl:attribute name="border">0</xsl:attribute>
				<xsl:attribute name="margin">0</xsl:attribute>
			</xsl:when>
			<xsl:when test="$class ='field' and self::html:td">
				<xsl:attribute name="text-align">center</xsl:attribute>
				<xsl:attribute name="margin">0</xsl:attribute>
				<xsl:attribute name="padding">0</xsl:attribute>
				<xsl:attribute name="border">0.05em solid black</xsl:attribute>
				<xsl:attribute name="border-top">0.05em solid black</xsl:attribute>
			</xsl:when>
			<xsl:when test="self::html:td and ($class ='subfield' or $class = 'subfieldunused')">
				<xsl:attribute name="font-size">4pt</xsl:attribute>
				<xsl:attribute name="border">0.05em dashed black</xsl:attribute>
				<xsl:attribute name="border-top">0.05em dashed black</xsl:attribute>
			</xsl:when>
			<xsl:when test="self::html:td and $class ='reserved'">
				<xsl:attribute name="text-align">center</xsl:attribute>
				<xsl:attribute name="background-color">#cccccc</xsl:attribute>
				<xsl:attribute name="border">0.05em solid black</xsl:attribute>
				<xsl:attribute name="border-top">0.05em solid black</xsl:attribute>
			</xsl:when>
			<xsl:when test="$class ='section' and self::html:div and
											(self::html:div/html:h2[1] or self::html:div/html:h3[1])">
				<xsl:attribute name="break-before">page</xsl:attribute>
			</xsl:when>
			<xsl:when test="$class ='section' and self::html:div and
											self::html:div/html:h4[1]">
				<xsl:attribute name="keep-together">always</xsl:attribute>
			</xsl:when>
			<xsl:otherwise>
				<xsl:if test="$class = 'singleodd' or $class = 'leftodd' or $class =
											'rightodd' or $class = 'singleeven' or $class = 'lefteven'
											or $class = 'righteven'">
					<xsl:attribute name="font-size">x-small</xsl:attribute>
					<xsl:attribute name="color">black</xsl:attribute>
					<xsl:attribute name="border-style">solid</xsl:attribute>
					<xsl:attribute name="border-top-width">0</xsl:attribute>
				</xsl:if>
				<xsl:if test="$class = 'left' or $class = 'leftodd' or $class = 'lefteven'">
					<xsl:attribute name="text-align">left</xsl:attribute>
					<xsl:attribute name="border-left-width">0.05em</xsl:attribute>
					<xsl:attribute name="border-right-width">0</xsl:attribute>
					<xsl:attribute name="border-bottom-width">0.05em</xsl:attribute>
				</xsl:if>
				<xsl:if test="$class = 'right' or $class = 'rightodd' or $class = 'righteven'">
					<xsl:attribute name="text-align">right</xsl:attribute>
					<xsl:attribute name="border-left-width">0</xsl:attribute>
					<xsl:attribute name="border-right-width">0.05em</xsl:attribute>
					<xsl:attribute name="border-bottom-width">0.05em</xsl:attribute>
				</xsl:if>
				<xsl:if test="$class = 'single' or $class = 'singleodd' or $class = 'singleeven'">
					<xsl:attribute name="text-align">center</xsl:attribute>
					<xsl:attribute name="border-left-width">0.05em</xsl:attribute>
					<xsl:attribute name="border-right-width">0.05em</xsl:attribute>
					<xsl:attribute name="border-bottom-width">0.05em</xsl:attribute>
				</xsl:if>
				<xsl:if test="$class = 'singleodd' or $class = 'leftodd' or $class = 'rightodd'">
					<xsl:attribute name="background-color">#eef</xsl:attribute>
					<xsl:attribute name="border-color">#dde</xsl:attribute>
				</xsl:if>
				<xsl:if test="$class = 'singleeven' or $class = 'lefteven' or $class = 'righteven'">
					<xsl:attribute name="background-color">#ffe</xsl:attribute>
					<xsl:attribute name="border-color">#eed</xsl:attribute>
				</xsl:if>
			</xsl:otherwise>
		</xsl:choose>
  </xsl:template>

</xsl:stylesheet>
