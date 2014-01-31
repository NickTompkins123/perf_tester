<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:template match="/report">
        <xsl:variable name="name" select="@name"/>
        <xsl:variable name="testCount" select="count(/report/benchmark/tests/child::*)"/>
        <html>
            <head>
                <title><xsl:value-of select="$name"/></title>
                     <link rel="stylesheet" href="reportdata/style.css" type="text/css" />
                    <script language="javascript" type="text/javascript" src="reportdata/excanvas.js"></script>
                    <script language="javascript" type="text/javascript" src="reportdata/jit.js"></script>
                    <script language="javascript" type="text/javascript" src="reportdata/test.js"></script>
                    <script language="javascript" type="text/javascript" src="reportdata/report.js"></script>
                    
                    <script>
                        var json = new Array(<xsl:value-of select="count(/report/benchmark/tests/child::*)"/>);
                              
                        <xsl:for-each select="/report/benchmark/tests/test">
                        <xsl:variable name="loopIndex"><xsl:value-of select="position()-1" /></xsl:variable>
                                            
                            <xsl:for-each select="frameTimes"> 
    
                                <xsl:variable name="content"><xsl:value-of select="." /></xsl:variable>
                                <xsl:variable name="separator">,</xsl:variable>
                                <xsl:variable name="index">1</xsl:variable>

                                json[<xsl:value-of select="$loopIndex"/>] =
                                {
                                    'label': ['frametime'],
                                    'values': [
                                        <xsl:call-template name="split">
                                          <xsl:with-param name="list"      select="$content" />
                                          <xsl:with-param name="separator" select="$separator" />
                                          <xsl:with-param name="index" select="$index" />
                                        </xsl:call-template>
                                 ]  
                                };
                            </xsl:for-each>
                        </xsl:for-each>
                        
                        for(n in json)
                        {
                            var maxFrames = 30;
                            if(json[n]['values'].length > maxFrames)
                            {
                                var l = json[n]['values'].length;
                                var m = l / maxFrames;
                                for(i in json[n]['values'])
                                {
                                    // TODO: Calc average
                                    if(i % m != 0)
                                    {
                                        json[n]['values'].splice(i, m-1);
                                    }
                                }
                            }
                        }
                    </script>
            </head>
            
            <body>
                <xsl:attribute name="onload">init(<xsl:value-of select="$testCount"/>);</xsl:attribute>
                    
            <div class="header">
                <center>
                    <table>
                        <tr>
                            <td><img src="reportdata/benchmark_logo.png" alt="UIMark" width="256" /></td>
                            <td><img src="reportdata/rightware_logo.png" alt="Rightware" width="256" /></td>
                        </tr>
                    </table>
                </center>
            </div>

        
            <div class="master">
                <div class="bg">
                    <div>
                        <div>          
                            
                            <h2>Score</h2>
                            <div class="text">
                                <b>
                               <xsl:if test="/report/validTestSuite = '1'">
                                	BasemarkCL
                                <xsl:if test="/report/benchmark/settings/OpenCLFullProfile = 1">FULL PROFILE</xsl:if>
                                <xsl:if test="/report/benchmark/settings/OpenCLFullProfile != 1">EMBEDDED PROFILE</xsl:if>  
				<xsl:text> score: </xsl:text> <xsl:value-of select="/report/benchmark/overallScore/score"/> 
																</xsl:if>
											
																
                                <xsl:if test="/report/validTestSuite != '1'">
                                Weighted geometric average for tests run in
                                <xsl:if test="/report/benchmark/settings/OpenCLFullProfile = 1">FULL PROFILE</xsl:if>
                                <xsl:if test="/report/benchmark/settings/OpenCLFullProfile != 1">EMBEDDED PROFILE</xsl:if>  
				<xsl:text>: </xsl:text> <xsl:value-of select="/report/benchmark/overallScore/score"/> 
                                    <br />
                                    All tests were not run. Given geometric average is not official benchmark score.
                                </xsl:if>
                        
                                <xsl:if test="/report/binaryKernels = '1'">
                                    <xsl:text>&#xa0;</xsl:text>
                                    <br />
                                    Binary kernels used.
                                </xsl:if>
                                                                       
                                <xsl:if test="/report/opengl/OpenGL = 'No OpenGL used.'">
                                    <xsl:text>&#xa0;</xsl:text>
                                    <br />
                                    No OpenGL used.
                                </xsl:if>   
                                    
                                </b> <br /> <br />
                            
                            
                                <xsl:if test="/report/validData = 0">
                                    <i><b>Benchmark kernels are not official ones!</b></i>
                                    <br />
                                    <br />
                                </xsl:if>
                            
                                Physics tests: <xsl:value-of select="/report/benchmark/overallScore/physicsTestScore"/><br />
                                General tests: <xsl:value-of select="/report/benchmark/overallScore/generalTestScore"/><br />
                                Image tests: <xsl:value-of select="/report/benchmark/overallScore/imageTestScore"/><br />
                                Video tests: <xsl:value-of select="/report/benchmark/overallScore/videoTestScore"/><br />
                            </div>
                            
                            <h2>Run info</h2>
                            <table class="kanzi-style">
                            <tr><td>Date:</td><td><xsl:value-of select="/report/date"/></td></tr>
                            <tr><td>Time: </td><td><xsl:value-of select="/report/time"/></td></tr>
                            <tr><td>Benchmark build: </td><td><xsl:value-of select="/report/benchmarkVersion"/> </td></tr>
                            <tr><td>Used Benchmark profile: </td><td>
                                <xsl:if test="/report/benchmark/settings/OpenCLFullProfile = 1">FULL PROFILE</xsl:if>
                                <xsl:if test="/report/benchmark/settings/OpenCLFullProfile != 1">EMBEDDED PROFILE</xsl:if>                            
                            </td></tr>  
                            <tr><td>CL device profile: </td><td><xsl:value-of select="/report/clProfile"/> </td></tr>
                            <tr><td>Valid data and kernels: </td>
                            <td>
                                <xsl:if test="/report/validData = 1">Yes</xsl:if>
                                <xsl:if test="/report/validData != 1">No</xsl:if>
                            </td></tr>
                            <tr><td>Command line parameters: </td><td><xsl:value-of select="/report/commandLineParameters"/> </td></tr>
                            </table>
                        </div> 
                    
                        <div class="footer"><h1>Test scores</h1></div>
                            
                        <div>
                            <xsl:call-template name="foreachtest"/>
                        </div>
                    
                    <div class="footer"><h1>Benchmark configuration</h1></div>
                            <xsl:for-each select="/report/benchmark/settings">
                                <xsl:call-template name="producetable"/>
                            </xsl:for-each>

                    <div class="footer"><h1>Platform info</h1></div>
                        <h2><xsl:value-of select="/report/device/platform"/></h2>                
                    
                            <h3>Surface</h3>
                            <xsl:for-each select="/report/surface">
                                <xsl:call-template name="producetable"/>
                            </xsl:for-each>
                            
                            <h3>OpenGL</h3>
                            <xsl:for-each select="/report/opengl">
                                <xsl:call-template name="producetable"/>
                            </xsl:for-each>
                            
                            <h3>OpenCL platform</h3>
                            <table class="kanzi-style">
                            <tr>
                                <th>Name</th>
                                <th>Value</th>
                            </tr><tr>
                                <td>Used platform ID</td>
                                <td><xsl:value-of select="/report/opencl/selectedPlatform"/></td>
                            </tr><tr>
                                <td>Used device ID</td>
                                <td><xsl:value-of select="/report/opencl/selectedDevice"/></td>
                            </tr><tr>
                                <td>Platform</td>
                                <td>
                                    <xsl:variable name="platform"><xsl:value-of select="/report/opencl/selectedPlatform"/></xsl:variable>
                                    <xsl:for-each select="/report/opencl/platforms/platform[@id=$platform]">
                                        <table style="border:0; margin: 0; padding: 0; vertical-align: top;">
                                            <xsl:for-each select="*">      
                                                <xsl:if test="count(*) = 0">
                                                        <tr><td style="border:0; margin: 0; padding: 0; padding-bottom: 0; vertical-align: top;">
                                                            <xsl:value-of select="name()" />: 
                                                        </td><td style="border:0; margin: 0; padding: 0; padding-bottom: 0; vertical-align: top;">
                                                            <xsl:value-of select="." />
                                                        </td></tr>
                                                </xsl:if>
                                            </xsl:for-each>
                                        </table>
                                    </xsl:for-each>
                                </td>                            
                            </tr><tr>
                                <td>Device</td>
                                <td>
                                    <xsl:variable name="usedplatform"><xsl:value-of select="/report/opencl/selectedPlatform"/></xsl:variable>
                                    <xsl:variable name="useddevice"><xsl:value-of select="/report/opencl/selectedDevice"/></xsl:variable>
                                    <xsl:for-each select="/report/opencl/platforms/platform[@id=$usedplatform]/devices/device[@id=$useddevice]"> 
                                        <xsl:call-template name="producetablevalue"/>
                                    </xsl:for-each>
                                </td>
                            </tr>
                            </table>
                            
                            <h3>OpenCL available platforms</h3>
                            <xsl:for-each select="/report/opencl/platforms">
                                <xsl:call-template name="producetable"/>
                            </xsl:for-each>
                          <p />                              
                    </div>
                </div>
            </div>
    
    
            </body>
        </html>
    </xsl:template>
    
    
    <xsl:template name="foreachtest">     
        <xsl:for-each select="/report/benchmark/tests/test">
            
            <xsl:variable name="positio"> 
                <xsl:value-of select="position()"/>
            </xsl:variable> 
            
            <xsl:variable name="valid"> 
                <xsl:value-of select="@validData"/>
            </xsl:variable> 
            
            <h2>
                <xsl:value-of select="position()"/>. <xsl:value-of select="@name"/>
                <xsl:if test="@isPartOfOverallScore = 0"> <i> (not included in overall score) </i> </xsl:if>
            </h2>

            <table class="kanzi-style">
            <tr>
                <th>Name</th>
                <th>Value</th>
            </tr>
            
            <tr><td>
                    Valid data and kernels
            </td><td>
                    <xsl:if test="$valid = 1">Yes</xsl:if>
                    <xsl:if test="$valid != 1">No</xsl:if>
            </td></tr>
            <xsl:for-each select="*"> 
                <tr><td>
                    <xsl:value-of select="name()" />
                </td><td>
                    
                    <xsl:choose>
                        <xsl:when test="name() = 'frameTimes' and string-length(.) > 150">
                            <div class="less">
                             <xsl:attribute name="id"><xsl:value-of select="$positio"/>-short</xsl:attribute>
                                <xsl:value-of select="substring(.,0,100)" /> 
                                <br />
                                <a class="showLink">
                                    <xsl:attribute name="href">#<xsl:value-of select="$positio"/></xsl:attribute>
                                    <xsl:attribute name="onclick">showHide('<xsl:value-of select="$positio"/>');return false;</xsl:attribute>
                                    more.
                                </a>
	                            <xsl:value-of select="number" />
                            </div>
                            <div class="more">
                             <xsl:attribute name="id"><xsl:value-of select="$positio"/>-long</xsl:attribute>
                             <xsl:value-of select="." />
                             <br />
                             <a class="hideLink">
                                <xsl:attribute name="href">#<xsl:value-of select="$positio"/></xsl:attribute>
                                <xsl:attribute name="onclick">showHide('<xsl:value-of select="$positio"/>');return false;</xsl:attribute>
                                hide
                             </a>                             
                            </div>
                        </xsl:when>
                        <xsl:when test="name() = 'binaryKernels'">
                            <xsl:if test=". = 1">Yes</xsl:if>
                            <xsl:if test=". != 1">No</xsl:if>
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:value-of select="." />
                        </xsl:otherwise>
                    </xsl:choose>
                    
                </td></tr>
            </xsl:for-each>
            </table>
            
            <center>
                <div id="infovis{position()-1}" class="infovis"></div>
            </center>
            
        </xsl:for-each>
    </xsl:template>


    <xsl:template name="producetable">
        <h2><xsl:value-of select="@name"/></h2>
        <table class="kanzi-style">
        <tr>
            <th>Name</th>
            <th>Value</th>
        </tr>
            <xsl:call-template name="producetablecontent"/>
        </table>
    </xsl:template>
    
    <xsl:template name="producetablecontent">
        <xsl:for-each select="*"> 
            <tr><td>
                <xsl:value-of select="name()" /> 
            </td><td>
                <xsl:call-template name="producetablevalue"/>
            </td></tr>
        </xsl:for-each>
    </xsl:template>
    
    <xsl:template name="producetablevalue">
        <xsl:if test="count(*) = 0">
            <xsl:value-of select="." />
        </xsl:if>
        <xsl:if test="count(*) > 0">
            <table style="border:0; margin: 0; padding: 0; vertical-align: top;">
            <xsl:for-each select="*"> 
                <tr style="border:0; margin: 0; padding: 0; padding-bottom: 0; vertical-align: top;">
                <td style="border:0; margin: 0; padding: 0; padding-bottom: 0; vertical-align: top;">
                <xsl:value-of select="name()" />:
                    </td><td style="border:0; margin: 0; padding: 0; padding-bottom: 0;">
                        <xsl:call-template name="producetablevalue"/>                        
                    </td>
                </tr>
            </xsl:for-each>
            </table>
        </xsl:if>
    </xsl:template>    

    <xsl:template name="split">
      <xsl:param name="list"      select="''" />
      <xsl:param name="separator" select="','" />
      <xsl:param name="index" />
    
      <xsl:if test="not($list = '' or $separator = '')">
        <xsl:variable name="head" select="substring-before(concat($list, $separator), $separator)" />
        <xsl:variable name="tail" select="substring-after($list, $separator)" />
                { 'label': '<xsl:value-of select="$index" />', 'values': [<xsl:value-of select="$head" />] }
                <xsl:if test="not($tail = '')">,</xsl:if>
        <xsl:call-template name="split">
          <xsl:with-param name="list"      select="$tail" />
          <xsl:with-param name="separator" select="$separator" />
          <xsl:with-param name="index">
                <xsl:value-of select="$index + 1"/>
          </xsl:with-param>
        </xsl:call-template>
      </xsl:if>
    </xsl:template>

    
</xsl:stylesheet>
