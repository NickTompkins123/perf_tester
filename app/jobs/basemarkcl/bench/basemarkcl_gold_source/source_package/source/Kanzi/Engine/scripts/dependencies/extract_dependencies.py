# Script to extract dependency information from Kanzi engine source codes.

# TODO: Extract also struct dependencies
import os
import sys
import re
import xml.etree.ElementTree as ET
from dot import *

ignorePatterns = set([".svn"])

sourcesPath = os.path.normpath("../../sources")
modules = ["User", "Core", "Framework", "System"]
moduleDirs = {
    "Framework" : "application_framework/common/src",
    "User"      : "user_layer/src",
    "Core"      : "core_layer/src",
    "System"    : "system_layer/common/src"
}

outputPath = os.path.normpath("../../output/dependencies")

outputDuplicateReferencesFile = os.path.join(outputPath, "duplicate_references.txt")
outputDependenciesFile = os.path.join(outputPath, "dependencies")
outputInternalDependenciesFile = os.path.join(outputPath, "internal_dependencies")
outputCyclicDependenciesFile = os.path.join(outputPath, "cyclic_dependencies")

fileTypes = {".c" : "source", ".h" : "header"}

individualClusterLinkNodes = False


# Dependency extraction to XML

def processFile(filePath, element, relativeFileName, relativeDirName):
    with open(filePath, "U") as file:
        for line in file:
            if "#include" in line:
                referencedFile = None
                matchesAbsolute = re.search("\<(.*).h\>", line)
                matchesRelative = re.search("\"(.*).h\"", line)
                if matchesAbsolute:
                    referencedFile = matchesAbsolute.group(1)
                elif matchesRelative:
                    referencedFile = relativeDirName + "/" + matchesRelative.group(1)
                if referencedFile:
                    ET.SubElement(element, "reference", {"target" : referencedFile})

def processDir(dirPath, element, parentDir = ""):
    for name in os.listdir(dirPath):
        if name in ignorePatterns:
            continue
        
        if parentDir == "":
            fullName = name
        else:
            fullName = parentDir + "/" + name
        
        path = os.path.join(dirPath, name)
        if os.path.isdir(path):
            childElement = ET.SubElement(element, "folder", {"name" : name})
            processDir(path, childElement, fullName)
        else:
            (name, type) = os.path.splitext(name)
            if type in fileTypes:
                type = fileTypes[type]
                childElement = ET.SubElement(element, "file", {"name" : name, "type" : type})
                processFile(path, childElement, fullName, parentDir)

def createXml(modules, moduleDirs):
    root = ET.Element("dependencies")
    for moduleName in modules:
        moduleDir = moduleDirs[moduleName]
        moduleSourcePath = os.path.join(sourcesPath, moduleDir)
        
        processDir(moduleSourcePath, root)
    return root


# XML filtering

def tagFolderPaths(element, parentPath):
    for folder in element.findall("folder"):
        folder.set("path", parentPath + folder.get("name"))
        tagFolderPaths(folder, parentPath + folder.get("name") + "/")
    
    for file in element.findall("file"):
        file.set("path", parentPath + file.get("name"))
    
def tagPaths(root):
    root.set("path", "")
    tagFolderPaths(root, "")
    
def merge(targetElement, element):
    for subElement in element.getchildren():
        targetElement.append(subElement)
        element.remove(subElement)

def mergeDependencies(element, outputFileName):
    # Merge duplicate folder names
    processedFolders = {}
    for folder in element.findall("folder"):
        folderName = folder.get("name")
        if folderName in processedFolders:
            print("Warning: Duplicate folder " + folderName)
            merge(processedFolders[folderName], folder)
            element.remove(folder)
        else:
            processedFolders[folderName] = folder
    
    # Process subfolders
    for folder in element.findall("folder"):
        mergeDependencies(folder, outputFileName)
    
    # Merge duplicate file names
    processedFiles = {}
    for file in element.findall("file"):
        fileName = file.get("name")
        del file.attrib["type"]
        if fileName in processedFiles:
            merge(processedFiles[fileName], file)
            element.remove(file)
        else:
            processedFiles[fileName] = file
    
    # Merge duplicate references in files
    for file in element.findall("file"):
        processedTargets = set()
        for reference in file.findall("reference"):
            targetName = reference.get("target")
            if targetName in processedTargets:
                with open(outputFileName, "a") as outputFile:
                    print("Duplicate reference in " + file.get("name") + " to " + targetName, file=outputFile)
                file.remove(reference)
            else:
                processedTargets.add(targetName)
    
    # Remove self-references
    for file in element.findall("file"):
        filePath = file.get("path")
        for reference in file.findall("reference"):
            if reference.get("target") == filePath:
                file.remove(reference)


# Dependency XML -> DOT XML

# Internal dependencies

def processFolderInternalDependencies(sourceFolder, targetFolder):
    processedClusters = set()
    topLevelLinks = []
    processedLinks = set()
    clusters = set()
    links = []
    for folder in sourceFolder.findall("folder"):
        clusterId = folder.get("path")
        if clusterId in processedClusters:
            raise RuntimeError("Duplicate folders encountered")
        else:
            processedClusters.add(clusterId)
            clusterName = folder.get("name")
            subFolder = ET.SubElement(targetFolder, "cluster", {"id" : clusterId, "name" : clusterName})
            clusters.add(clusterId)
            if not individualClusterLinkNodes:
                ET.SubElement(subFolder, "node", {"id" : clusterId, "name" : "", "shape" : "point", "style" : "invis"})
        collectedLinks = processFolderInternalDependencies(folder, subFolder)
    
        for link in collectedLinks:
            parentId = sourceFolder.get("path")
            if parentId != "":
                parentId = parentId + "/"
            if link.find(parentId) == 0:
                targetId = parentId + re.search("([^/]*).*", link[len(parentId):]).group(1)
                sourceId = clusterId
                linkId = sourceId + "&" + targetId
                if linkId not in processedLinks:
                    processedLinks.add(linkId)
                    links.append([sourceId, targetId])
            else:
                topLevelLinks.append(link)
        
    processedNodes = {}
    for file in sourceFolder.findall("file"):
        nodeId = file.get("path")
        if nodeId in processedNodes:
            node = processedNodes[nodeId]
        else:
            nodeName = file.get("name")
            node = ET.SubElement(targetFolder, "node", {"id" : nodeId, "name" : nodeName, "shape" : "box"})
            processedNodes[nodeId] = node
            
        for reference in file.findall("reference"):
            target = reference.get("target")
            parentId = sourceFolder.get("path")
            if parentId != "":
                parentId = parentId + "/"
            if target.find(parentId) == 0:
                targetId = parentId + re.search("([^/]*).*", target[len(parentId):]).group(1)
                if nodeId != targetId:
                    linkId = nodeId + "&" + targetId
                    if linkId not in processedLinks:
                        processedLinks.add(linkId)
                        links.append([nodeId, targetId])
            else:
                topLevelLinks.append(target)
    
    for link in links:
        sourceId = link[0]
        targetId = link[1]
        linkNode = ET.SubElement(targetFolder, "link", {"source-id" : sourceId, "target-id" : targetId})
        if individualClusterLinkNodes:
            for sourceIdFolder in targetFolder.findall("cluster"):
                if sourceIdFolder.get("id") == sourceId:
                    ET.SubElement(sourceIdFolder, "node", {"id" : "out_" + sourceId + "_" + targetId, "name" : "", "shape" : "point", "style" : "invis"})
            for targetIdFolder in targetFolder.findall("cluster"):
                if targetIdFolder.get("id") == targetId:
                    ET.SubElement(targetIdFolder, "node", {"id" : "in_" + sourceId + "_" + targetId, "name" : "", "shape" : "point", "style" : "invis"})
            if targetId in clusters:
                linkNode.set("lhead", targetId)
                linkNode.set("target-id", "in_" + sourceId + "_" + targetId)
            if sourceId in clusters:
                linkNode.set("ltail", sourceId)
                linkNode.set("source-id", "out_" + sourceId + "_" + targetId)
        else:
            if targetId in clusters:
                linkNode.set("lhead", targetId)
                linkNode.set("target-id", targetId)
            if sourceId in clusters:
                linkNode.set("ltail", sourceId)
                linkNode.set("source-id", sourceId)
        if targetId in clusters or sourceId in clusters:
            linkNode.set("weight", "100")
    
    return topLevelLinks

def createInternalDependencies(root):
    newRoot = ET.Element("internalDependencies", {"rankdir" : "LR", "compound" : "true", "concentrate" : "true", "remincross" : "true", "searchsize" : "1000"})
    # Create internal dependencies from all dependencies
    processFolderInternalDependencies(root, newRoot)
    return newRoot

# Cyclic dependencies

def processDependencyGraphFolder(nodes, folder):
    for subFolder in folder.findall("folder"):
        processDependencyGraphFolder(nodes, subFolder)
    
    for file in folder.findall("file"):
        linkSource = file.get("path")
        for reference in file.findall("reference"):
            linkTarget = reference.get("target")
            if linkSource in nodes:
                linkSourceNode = nodes[linkSource]
            else:
                linkSourceNode = set()
                nodes[linkSource] = linkSourceNode
            if linkTarget in nodes:
                linkTargetNode = nodes[linkTarget]
            else:
                linkTargetNode = set()
                nodes[linkTarget] = linkTargetNode
            linkSourceNode.add(linkTarget)
    
def createDependencyGraph(root):
    nodes = {}
    processDependencyGraphFolder(nodes, root)
    return nodes

def tarjan(graph, newGraph, node, nodeAttributes, stack, index):
    nodeAttributes[node] = [index[0], index[0]]
    index[0] = index[0] + 1
    
    stack.append(node)
    for target in graph[node]:
        if target not in nodeAttributes:
            tarjan(graph, newGraph, target, nodeAttributes, stack, index)
            nodeAttributes[node][1] = min(nodeAttributes[node][1], nodeAttributes[target][1])
        elif target in stack:
            nodeAttributes[node][1] = min(nodeAttributes[node][1], nodeAttributes[target][0])
    if nodeAttributes[node][1] == nodeAttributes[node][0]:
        newNode = None
        strongNodes = []
        while newNode != node:
            newNode = stack.pop()
            strongNodes.append(newNode)
        if len(strongNodes) == 0:
            raise RuntimeError("Empty list")
        elif len(strongNodes) > 1:
            for strongNode in strongNodes:
                newGraph[strongNode] = strongNodes

def filterStronglyConnectedComponents(graph):
    nodeAttributes = {}
    newGraph = {}
    for node in graph:
        if node not in nodeAttributes:
            tarjan(graph, newGraph, node, nodeAttributes, [], [0])
    
    return newGraph

def processFolderCyclicDependencies(sourceFolder, targetFolder, graph):
    links = []
    for folder in sourceFolder.findall("folder"):
        clusterId = folder.get("path")
        clusterName = folder.get("name")
        subFolder = ET.SubElement(targetFolder, "cluster", {"id" : clusterId, "name" : clusterName})
        links.extend(processFolderCyclicDependencies(folder, subFolder, graph))
        
    processedNodes = {}
    for file in sourceFolder.findall("file"):
        nodeId = file.get("path")
        if (nodeId in graph):
            nodeName = file.get("name")
            node = ET.SubElement(targetFolder, "node", {"id" : nodeId, "name" : nodeName, "shape" : "box"})
            for reference in file.findall("reference"):
                target = reference.get("target")
                if nodeId in graph and target in graph[nodeId]:
                    links.append([nodeId, target])
    
    parentId = sourceFolder.get("path")
    if parentId != "":
        parentId = parentId + "/"
    topLevelLinks = []
    for link in links:
        sourceId = link[0]
        targetId = link[1]
        if targetId.find(parentId) == 0:
            linkNode = ET.SubElement(targetFolder, "link", {"source-id" : sourceId, "target-id" : targetId, "color" : "\"#3F0000\""})
        else:
            topLevelLinks.append(link)
    
    return topLevelLinks

def createCyclicDependencies(root):
    newRoot = ET.Element("cyclicDependencies", {"rankdir" : "LR", "compound" : "false", "concentrate" : "true"})
    graph = createDependencyGraph(root)
    graph = filterStronglyConnectedComponents(graph)
    #print(graph)
    # Create cyclic dependencies from all dependencies
    processFolderCyclicDependencies(root, newRoot, graph)
    return newRoot


# Program starts here

dependencies = None
if len(sys.argv) > 1:
    argument = sys.argv[1]
    if argument == "/?":
        print("Usage: extract_dependencies.py [input_xml_file_name]")
        sys.exit()
    else:
        # Load existing XML tree
        dependencies = ET.parse(argument).getroot()

if dependencies == None:
    # Fill the XML tree
    dependencies = createXml(modules, moduleDirs)

tagPaths(dependencies)

if not os.path.exists(outputPath):
    os.makedirs(outputPath)

mergeDependencies(dependencies, outputDuplicateReferencesFile)
ET.ElementTree(dependencies).write(outputDependenciesFile + ".xml", encoding="UTF-8")

# Create internal dependencies from the unprocessed dependency tree
finalize(createInternalDependencies(dependencies), outputInternalDependenciesFile)

# Create cyclic dependencies from the unprocessed dependency tree
finalize(createCyclicDependencies(dependencies), outputCyclicDependenciesFile)
