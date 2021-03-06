<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
   <xsl:output method="xml" indent="yes" />

   <xsl:param name="suitename" />

   <xsl:template match="/">
      <testsuite>
         <xsl:attribute name="errors">
            <xsl:value-of select="TestRun/Statistics/Errors" />
         </xsl:attribute>
         <xsl:attribute name="failures">
            <xsl:value-of select="TestRun/Statistics/Failures" />
         </xsl:attribute>
         <xsl:attribute name="tests">
            <xsl:value-of select="TestRun/Statistics/Tests" />
         </xsl:attribute>
         <xsl:attribute name="name">
            <xsl:value-of select="$suitename" />
         </xsl:attribute>
         <xsl:apply-templates />
      </testsuite>
   </xsl:template>

   <xsl:template match="/TestRun/SuccessfulTests/Test">
      <testcase>
         <xsl:attribute name="classname">KanziTest</xsl:attribute>
         <xsl:attribute name="name">
            <xsl:value-of select="Name" />
         </xsl:attribute>
         <xsl:attribute name="time">0</xsl:attribute>
      </testcase>
   </xsl:template>

   <xsl:template match="/TestRun/FailedTests/FailedTest">
      <testcase>
         <xsl:attribute name="classname">KanziTest</xsl:attribute>
         <xsl:attribute name="name">
            <xsl:value-of select="Name" />
         </xsl:attribute>

         <xsl:attribute name="time">0</xsl:attribute>

         <xsl:choose>
            <xsl:when test="FailureType='Error'">
               <error>
               <xsl:attribute name="message">
                  <xsl:value-of select=" normalize-space(Message)" />
               </xsl:attribute>
               <xsl:attribute name="type">
                  <xsl:value-of select="FailureType" />
               </xsl:attribute>
               <xsl:value-of select="Message" />
               File:<xsl:value-of select="Location/File" />
               Line:<xsl:value-of select="Location/Line" />
               </error>
            </xsl:when>

            <xsl:otherwise>
               <failure>
               <xsl:attribute name="message">
                  <xsl:value-of select=" normalize-space(Message)" />
               </xsl:attribute>
               <xsl:attribute name="type">
                  <xsl:value-of select="FailureType" />
               </xsl:attribute>
               <xsl:value-of select="Message" />
               File:<xsl:value-of select="Location/File" />
               Line:<xsl:value-of select="Location/Line" />
               </failure>
            </xsl:otherwise>
         </xsl:choose>
      </testcase>
   </xsl:template>

   <xsl:template match="text()|@*" />
</xsl:stylesheet>
