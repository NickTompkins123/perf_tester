import xml.dom.minidom
import sys
import os
import shutil
import glob

numberOfErrors = 0
workspace = sys.argv[1] #Absolute path to the workspace folder of Hudson job
lintOutput = sys.argv[2] #Relative path to the lint output folder (e.g. Kanzi\Engine\output\lint)
directory = os.path.join(workspace, lintOutput)

files = glob.glob(directory + '/*.xml')

for fileName in files:
    # Parse filename, generate module specific subdir
    kanziModuleName = fileName.split(".")[0]
    outFileName =  "lint-output.log"
    outFilePath = os.path.join(kanziModuleName, outFileName)
    try:
        shutil.rmtree(kanziModuleName)
    except:
        pass
    os.makedirs(kanziModuleName)
    
    outFile = None
    
    file = xml.dom.minidom.parse(fileName)
    lint = file.getElementsByTagName("lint")
    modules = lint[0].getElementsByTagName("module")
    for module in modules:
        name = module.getAttribute("name")
        messages = module.getElementsByTagName("message")
        for message in messages:
            fileElement = message.getElementsByTagName("file")[0]
            lineNumber = fileElement.getAttribute("line")
            absoluteFileName = fileElement.getAttribute("name")

            if len(absoluteFileName) < 2:
                relativeFileName = name;
                #print ("Debug: " + relativeFileName)
            else:
                relativeFileName = absoluteFileName[len(workspace)+1:len(absoluteFileName)] # +1 to remove that extra separator
                
            messageType = message.getAttribute("type")
            if messageType == 'Error':
                numberOfErrors = numberOfErrors + 1
            messageCode = message.getAttribute("code")
            textNode = message.getElementsByTagName("text")[0].childNodes[0]
            messageText = textNode.data
            lineOut = "%s(%s):%s %s:%s"%(relativeFileName, lineNumber, messageType, messageCode, messageText)
            if len(lineOut) > 0:
                if outFile == None:
                    outFile = open(outFilePath, 'w')
                outFile.write(lineOut + '\n')

    if outFile != None:
        outFile.close()
        
if numberOfErrors > 0:
    sys.exit("Lint check reported %i errors."%(numberOfErrors))
