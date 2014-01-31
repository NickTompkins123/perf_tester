import os
import xml.etree.ElementTree as ET

availableGraphAttributes = set(["concentrate", "rankdir", "compound", "remincross", "searchsize"])
availableNodeAttributes = set(["shape", "style"])
availableEdgeAttributes = set(["constraint", "minlen", "weight", "color"])

# DOT XML -> DOT
def escapeClusterId(id):
    return id.replace("/", "_")

def escapeNodeId(id):
    return "\"" + id + "\""

def processDotCluster(cluster, indent, outputFile):
    for subCluster in cluster.findall("cluster"):
        subClusterId = "cluster_" + escapeClusterId(subCluster.get("id"))
        subClusterName = subCluster.get("name")
        print(indent + "subgraph " + subClusterId + " {", file=outputFile)
        newIndent = indent + "\t"
        print(newIndent + "label=\"" + subClusterName + "\";", file=outputFile)
        processDotCluster(subCluster, newIndent, outputFile)
        print(indent + "}", file=outputFile)
    
    for node in cluster.findall("node"):
        nodeId = escapeNodeId(node.get("id"))
        nodeName = node.get("name")
        nodeAttributes = ""
        for attribute in availableNodeAttributes:
            attributeValue = node.get(attribute)
            if attributeValue:
                nodeAttributes += "," + attribute + "=" + attributeValue
        print(indent + nodeId + " [label=\"" + nodeName + "\"" + nodeAttributes + "];", file=outputFile)
    
    for link in cluster.findall("link"):
        sourceId = escapeNodeId(link.get("source-id"))
        targetId = escapeNodeId(link.get("target-id"))
        attributes = ""
        lhead = link.get("lhead")
        for attribute in availableEdgeAttributes:
            attributeValue = link.get(attribute)
            if attributeValue:
                attributes += "," + attribute + "=" + attributeValue
        if lhead:
            attributes += ",lhead=cluster_" + escapeClusterId(lhead)
        ltail = link.get("ltail")
        if ltail:
            attributes += ",ltail=cluster_" + escapeClusterId(ltail)
        if len(attributes) > 0:
            attributes = attributes[1:]
        print(indent + sourceId + " -> " + targetId + " [" + attributes + "];", file=outputFile)

def createDot(root, outputFile):
    print("digraph G {", file=outputFile)
    for attribute in availableGraphAttributes:
        attributeValue = root.get(attribute)
        if attributeValue:
            print("\t" + attribute + "=" + attributeValue, file=outputFile)
    processDotCluster(root, "\t", outputFile)
    print("}", file=outputFile)


# File writing
def finalize(root, filePath):
    (folders, fileName) = os.path.split(filePath)
    if not os.path.exists(folders):
        os.makedirs(folders)
    ET.ElementTree(root).write(filePath + ".xml", encoding="UTF-8")
    with open(filePath + ".dot", "w") as outputFile:
        createDot(root, outputFile)
    os.system("..\\..\\tools\\thirdparty\\graphviz\\bin\\dot -Tpdf " + filePath + ".dot -o " + filePath + ".pdf")
