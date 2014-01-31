# Script to filter dependency information from Kanzi engine source codes.
import os
import sys
import re
import xml.etree.ElementTree as ET
from dot import *

outputPath = os.path.normpath("../../output/dependencies")

individualClusterLinkNodes = False


# Dependency XML -> DOT XML

# All dependencies

def processFolderDependencies(sourceFolder, targetFolder):
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
        collectedLinks = processFolderDependencies(folder, subFolder)
    
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

def processFolderDetailedDependencies(sourceFolder, targetFolder):
    links = []
    for folder in sourceFolder.findall("folder"):
        clusterId = folder.get("path")
        clusterName = folder.get("name")
        subFolder = ET.SubElement(targetFolder, "cluster", {"id" : clusterId, "name" : clusterName})
        links.extend(processFolderDetailedDependencies(folder, subFolder))
        
    processedNodes = {}
    for file in sourceFolder.findall("file"):
        nodeId = file.get("path")
        nodeName = file.get("name")
        node = ET.SubElement(targetFolder, "node", {"id" : nodeId, "name" : nodeName, "shape" : "box"})
        for reference in file.findall("reference"):
            target = reference.get("target")
            links.append([nodeId, target])
    
    parentId = sourceFolder.get("path")
    if parentId != "":
        parentId = parentId + "/"
    topLevelLinks = []
    for link in links:
        sourceId = link[0]
        targetId = link[1]
        if targetId.find(parentId) == 0:
            linkNode = ET.SubElement(targetFolder, "link", {"source-id" : sourceId, "target-id" : targetId})
        else:
            topLevelLinks.append(link)
    
    return topLevelLinks

def createDependencies(root):
    newRoot = ET.Element("singleDependencies", {"rankdir" : "LR", "compound" : "true", "concentrate" : "true", "remincross" : "true", "searchsize" : "1000"})
    # Create dependencies from all dependencies
    processFolderDependencies(root, newRoot)
    return newRoot

# Internal dependencies

def gatherTargets(cluster, filterString):
    keep = set()
    
    for subCluster in cluster.findall("cluster"):
        keep |= (gatherTargets(subCluster, filterString))
    
    for link in cluster.findall("link"):
        sourceId = link.get("source-id")
        targetId = link.get("target-id")
        if filterString in sourceId or filterString in targetId:
            if sourceId.count("/") < filterString.count("/") + 2:
                targetId = link.get("target-id")
                keep.add(sourceId)
                keep.add(targetId)
    
    return keep
    
def filterClusterDependencies(sourceCluster, targetCluster, keep, filterString):
    retval = False
    for sourceSubCluster in sourceCluster.findall("cluster"):
        targetSubCluster = ET.SubElement(targetCluster, sourceSubCluster.tag, sourceSubCluster.attrib)
        needed = filterClusterDependencies(sourceSubCluster, targetSubCluster, keep, filterString)
        if not needed:
            targetCluster.remove(targetSubCluster)
        else:
            retval = True;
    
    for node in sourceCluster.findall("node"):
        nodeId = node.get("id")
        if nodeId in keep:
            ET.SubElement(targetCluster, node.tag, node.attrib)
            retval = True
    
    for link in sourceCluster.findall("link"):
        sourceId = link.get("source-id")
        targetId = link.get("target-id")
        if filterString == sourceId or filterString == targetId:
            ET.SubElement(targetCluster, link.tag, link.attrib)
    
    return retval

def filterDependencies(root, filterString):
    keep = gatherTargets(root, filterString)
    newRoot = ET.Element(root.tag, root.attrib)
    filterClusterDependencies(root, newRoot, keep, filterString)
    return newRoot

def makeDependencies(dependencies, filterString):
    filteredDependencies = filterDependencies(dependencies, filterString)
    finalize(filteredDependencies, os.path.join(outputPath, "Components", filterString, "Dependencies"))

def makeAllClusterDependencies(dependencies, cluster):
    filterString = cluster.get("id")
    makeDependencies(dependencies, filterString)
    
    for subCluster in cluster.findall("cluster"):
        makeAllClusterDependencies(dependencies, subCluster)

def makeAllDependencies(dependencies):
    for subCluster in dependencies.findall("cluster"):
        makeAllClusterDependencies(dependencies, subCluster)
# Program starts here

dependencies = None
if len(sys.argv) < 2 or sys.argv[1] == "/?":
    print("Usage: single_dependencies.py <input_xml_file_name> [filter-string]")
    sys.exit()

# Load existing XML tree
root = ET.parse(sys.argv[1]).getroot()
dependencies = createDependencies(root)

if len(sys.argv) == 3:
    makeDependencies(dependencies, sys.argv[2])
else:
    makeAllDependencies(dependencies)