import os
import fnmatch
import shutil
import subprocess
import re
import xml.etree.ElementTree as ET
import sys
import difflib


sys.path.append(os.path.normpath(".."))
import kanzi


# Would rather use pysvn for SVN support, but it doesn't support 64-bit Python.

def svn(command, *arguments):
    command = "svn " + command + " --xml " + " ".join(arguments)
    print(command)
    proc = subprocess.Popen(command, shell=False, stdout=subprocess.PIPE)
    (result, stderr) = proc.communicate()
    return ET.XML(result)


def createDiffFile(oldFileContent, newFileContent, fileName):
    htmlDiff = difflib.HtmlDiff().make_table(oldFileContent, newFileContent, "old", "new")
    diffFile = open(fileName, "w")
    diffFile.write('''
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
          "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html>

<head>
    <meta http-equiv="Content-Type"
          content="text/html; charset=ISO-8859-1" />
    <title></title>
    <style type="text/css">
        table.diff {font-family:Consolas; font-size: 75%; border:medium;}
        td {vertical-align:top}
        .diff_header {background-color:#e0e0e0}
        td.diff_header {text-align:right}
        .diff_next {background-color:#c0c0c0}
        .diff_add {background-color:#aaffaa}
        .diff_chg {background-color:#ffff77}
        .diff_sub {background-color:#ffaaaa}
    </style>
</head>

<body>''')
    diffFile.write(htmlDiff.replace(' nowrap="nowrap"', '').replace("&nbsp;","&ensp;"))
    diffFile.write('''
</body>
</html>''')


def gatherChanges(kanziEnginePath, oldSvnFullPath, newSvnFullPath, outputPath, commonRevision = None):
    repositoryInfo = svn("info", "\"" + newSvnFullPath + "\"")
    headRevision = repositoryInfo.find("entry").get("revision")
    repositoryRoot = repositoryInfo.find("entry").find("repository").find("root").text
    print("Repository: " + repositoryRoot)
    print("HEAD revision: " + headRevision)
    
    oldSvnPath = oldSvnFullPath.replace(repositoryRoot, "")
    newSvnPath = newSvnFullPath.replace(repositoryRoot, "")
    
    if not commonRevision:
        oldLog = svn("log", "--verbose", "\"" + oldSvnFullPath + "\"")
        newLog = svn("log", "--verbose", "\"" + newSvnFullPath + "\"")
        ET.ElementTree(oldLog).write(os.path.join(outputPath, "oldLog_full.xml"), encoding="UTF-8")
        ET.ElementTree(newLog).write(os.path.join(outputPath, "newLog_full.xml"), encoding="UTF-8")
        
        # [old, new]
        revisions = [headRevision, headRevision]
        trackPaths = [oldSvnPath, newSvnPath]
        logentries = [iter(oldLog.findall("logentry")), iter(newLog.findall("logentry"))]
        common = False 
        while not common:
            if revisions[0] > revisions[1]:
                index = 0
                which = "old release"
            else:
                index = 1
                which = "new release"
            logentry = next(logentries[index])
            revision = logentry.get("revision")
            revisions[index] = revision
            #print(trackPaths[index] + "(" + str(index) + "): " + revision + " [vs. " + revisions[1-index] + "]")
            for path in logentry.find("paths").findall("path"):
                item = path.text
                copyfrom = path.get("copyfrom-path")
                if copyfrom:
                    if item in trackPaths[index]:
                        print("Tracking " + which + " from " + copyfrom + " (Revision " + revision + ")")
                        trackPaths[index] = trackPaths[index].replace(item, copyfrom)
            common = (revisions[0] == revisions[1])
        commonRevision = revisions[0]
    else:
        commonRevision = str(commonRevision)
    
    print("Common base revision: " + commonRevision)
    
    #oldLog = svn("log", "--verbose", "-r " + headRevision + ":" + commonRevision, "\"" + oldSvnFullPath + "\"")
    newLog = svn("log", "--verbose", "-r " + headRevision + ":" + commonRevision, "\"" + newSvnFullPath + "\"")
    #ET.ElementTree(oldLog).write("oldLog.xml", encoding="UTF-8")
    ET.ElementTree(newLog).write(os.path.join(outputPath, "newLog.xml"), encoding="UTF-8")
    
    trackPath = newSvnPath
    copies = {}
    for logentry in newLog.findall("logentry"):
        for path in logentry.find("paths").findall("path"):
            if path.get("action") == "A":
                item = path.text
                copyfrom = path.get("copyfrom-path")
                if copyfrom:
                    if item in trackPath:
                        trackPath = trackPath.replace(item, copyfrom)
                    else:
                        item = item.replace(trackPath, newSvnPath)
                        copyfrom = copyfrom.replace(trackPath, newSvnPath)
                    copies[item] = copyfrom
    
    #for copy in copies:
    #    print(copy + " -> " + copies[copy])
    
    diff = svn("diff", "--summarize", "-x \"--ignore-all-space --ignore-eol-style\"", "\"" + oldSvnFullPath + "\"", "\"" + newSvnFullPath + "\"")
    ET.ElementTree(diff).write(os.path.join(outputPath, "diff.xml"), encoding="UTF-8")
    diffPaths = diff.find("paths")
    
    added = set()
    deleted = set()
    modified = set()
    for pathNode in diffPaths.findall("path"):
        if pathNode.get("kind") == "file":
            item = pathNode.get("item")
            path = pathNode.text
            if path.endswith(".h"):
                path = path.replace(oldSvnPath, newSvnPath).replace(repositoryRoot, "")
                if item == "added":
                    added.add(path)
                elif item == "deleted":
                    deleted.add(path)
                elif item == "modified":
                    modified.add(path)
    #return [added, deleted, modified]
    #ET.ElementTree(diffPaths).write("temp.xml", encoding="UTF-8")
    
    #print("Deleted files:")
    #for delete in deleted:
    #    print(delete)
    
    moved = {}
    
    for add in added.copy():
        if add in copies:
            source = add
            while source in copies:
                source = copies[source]
            if source in deleted:
                moved[add] = source
                deleted.remove(source)
                added.remove(add)
            else:
                source = source.replace(newSvnPath, oldSvnPath)
                if source in deleted:
                    moved[add] = source
                    deleted.remove(source)
                    added.remove(add)
    
    xmlRoot = ET.Element("changes")
    xmlAdded = ET.SubElement(xmlRoot, "added")
    xmlDeleted = ET.SubElement(xmlRoot, "deleted")
    xmlMoved = ET.SubElement(xmlRoot, "moved")
    xmlModified = ET.SubElement(xmlRoot, "modified")
    
    for file in sorted(added):
        filePath = file.replace(newSvnPath, "")
        ET.SubElement(xmlAdded, "file", {"name" : filePath})
    for file in sorted(deleted):
        filePath = file.replace(newSvnPath, "")
        ET.SubElement(xmlDeleted, "file", {"name" : filePath})
    for file in sorted(moved):
        filePath = file.replace(newSvnPath, "")
        ET.SubElement(xmlMoved, "file", {"name" : filePath, "source" : moved[file]})
    for file in sorted(modified):
        oldFile = file.replace(newSvnPath, oldSvnPath)
        oldCommand = "svn cat \"" + repositoryRoot + oldFile + "\""
        newCommand = "svn cat \"" + repositoryRoot + file + "\""
        oldFileContent = subprocess.Popen(oldCommand, shell=False, stdout=subprocess.PIPE, universal_newlines=True).stdout.readlines()
        newFileContent = subprocess.Popen(newCommand, shell=False, stdout=subprocess.PIPE, universal_newlines=True).stdout.readlines()
        fileName = os.path.basename(file)
        diffFileName = "changes_" + fileName + ".html"
        createDiffFile(oldFileContent, newFileContent, os.path.join(outputPath, diffFileName))
        filePath = file.replace(newSvnPath, "")
        xmlFile = ET.SubElement(xmlModified, "file", {"name" : filePath, "diff" : diffFileName})
    
    ET.ElementTree(xmlRoot).write(os.path.join(outputPath, "changes.xml"), encoding="UTF-8")
    return xmlRoot


def createReport(xmlRoot, fileName):
    file = open(fileName, "w")
    file.write(
'''<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>

<head>
    <meta http-equiv="Content-Type"
          content="text/html; charset=ISO-8859-1" />
    <title></title>
    <style type="text/css">
        table.diff {font-family:Consolas; font-size: 75%; border:medium;}
        td {vertical-align:top}
        .diff_header {background-color:#e0e0e0}
        td.diff_header {text-align:right}
        .diff_next {background-color:#c0c0c0}
        .diff_add {background-color:#aaffaa}
        .diff_chg {background-color:#ffff77}
        .diff_sub {background-color:#ffaaaa}
    </style>
</head>

<body>
<title>Change report</title>
''')
    
    file.write("<h2>Added files</h2>\n")
    file.write('<table cellspacing="0" cellpadding="0" border="0">\n')
    for addedFile in xmlRoot.find("added").findall("file"):
        file.write("<tr><td>" + addedFile.get("name") + "</td></tr>\n")
    file.write("</table>")
    
    file.write("<h2>Removed files</h2>\n")
    file.write('<table cellspacing="0" cellpadding="0" border="0">\n')
    for removedFile in xmlRoot.find("deleted").findall("file"):
        file.write("<tr><td>" + removedFile.get("name") + "</td></tr>\n")
    file.write("</table>")
    
    file.write("<h2>Moved / renamed files</h2>\n")
    file.write('<table cellspacing="0" cellpadding="0" border="0">\n')
    for movedFile in xmlRoot.find("moved").findall("file"):
        file.write("<tr><td>" + movedFile.get("source") + " -> " + movedFile.get("name") + "</td></tr>\n")
    file.write("</table>")
    
    file.write("<h2>Modified files</h2>\n")
    file.write('<table cellspacing="0" cellpadding="0" border="0">\n')
    for modifiedFile in xmlRoot.find("modified").findall("file"):
        file.write("<tr><td><a href=\"" + modifiedFile.get("diff") + "\">" + modifiedFile.get("name") + "</a></td></tr>\n")
    file.write("</table>")
    
    file.write(
'''
</body>
</html>
''')


if __name__ == "__main__":
    if len(sys.argv) > 2:
        print("Usage: changes.py [commonRevision]")
        sys.exit()
    elif len(sys.argv) == 2:
        commonRevision = sys.argv[1]
    else:
        commonRevision = None
    #gather_changes(os.path.join("..", ".."),
    #               "https://svn.wumpus.name/svn/products/tags/Kanzi/v2_0_alpha/Engine/sources",
    #               "https://svn.wumpus.name/svn/products/tags/Kanzi/v2_0_pre-beta/Engine/sources",
    #               commonRevision)
    outputPath = "output"
    kanzi.deleteDir(outputPath)
    os.makedirs(outputPath)
    
    
    changes = gatherChanges(os.path.join("..", ".."),
                            "https://svn.wumpus.name/svn/products/tags/Kanzi/v2_0_beta_2_2/Engine/sources",
                            "https://svn.wumpus.name/svn/products/branches/Kanzi/v2_0/Engine/sources", outputPath,
                            commonRevision)
    
    createReport(changes, os.path.join(outputPath, "changes.html"))


if __name__ == "__main2__":
    changes = ET.parse("output/changes.xml").getroot()
    createReport(changes, os.path.join("output/changes.html"))


if __name__ == "__main3__":
    oldCommand = "svn cat \"https://svn.wumpus.name/svn/products/tags/Kanzi/v2_0_pre-beta/Engine/sources/core_layer/src/core/util/math/kzc_ray.h\""
    newCommand = "svn cat \"https://svn.wumpus.name/svn/products/trunk/Kanzi/Engine/sources/core_layer/src/core/util/math/kzc_ray.h\""
    oldFileContent = subprocess.Popen(oldCommand, shell=False, stdout=subprocess.PIPE, universal_newlines=True).stdout.readlines()
    newFileContent = subprocess.Popen(newCommand, shell=False, stdout=subprocess.PIPE, universal_newlines=True).stdout.readlines()
    open("test1_old.txt", "w").writelines(oldFileContent)
    open("test1_new.txt", "w").writelines(newFileContent)
    createDiffFile(oldFileContent, newFileContent, "changes_test1.html")
