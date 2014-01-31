# Script to extract dependency information from Kanzi engine source codes.

# TODO: Extract also struct dependencies
import os
import sys
import re
import fnmatch
import xml.etree.ElementTree as ET

projectsPath = os.path.normpath("../../configs/platforms/win32")

ignoredFilters = ("common", "win32", "win32_egl", "win32_openvg_ri", "egl")

projectFolders = {
    "system" : ["sources/system_layer/common/src/system",
                "sources/system_layer/platforms/win32_egl/src/system",
                "sources/system_layer/platforms/win32_openvg_ri/src/system",
                "sources/system_layer/environments/win32/src/system",
                "sources/system_layer/environments/egl/src/system"],
    "core" : ["sources/core_layer/src/core"],
    "user" : ["sources/user_layer/src/user"],
    "application_framework" : ["sources/application_framework/common/src/application",
                               "sources/application_framework/platforms/win32/src/application"],
    "managed_wrapper" : ["sources/managed_wrapper/src"],
    "default_tool_module" : ["applications/default_application/src", "applications/generic/tool_module/src"],
    "binary_viewer" : ["applications/binary_viewer/src"],
    "unit_tests_generic" : ["tests/unit_tests/generic/src",
                            "tests/test_framework/src"],
    "unit_tests_es1" : ["tests/unit_tests/es1/src",
                        "tests/test_framework/src"],
    "unit_tests_es2" : ["tests/unit_tests/es2/src",
                        "tests/test_framework/src"],
    "unit_tests_vg" : ["tests/unit_tests/vg/src",
                       "tests/test_framework/src"],
}

def processFilter(element, allowedPaths, relativePath):
    filterName = element.get("Name")
    if filterName in ignoredFilters:
        newPath = relativePath
    else:
        newPath = relativePath + filterName + "/"
    processFiles(element, allowedPaths, newPath)

def processFiles(element, allowedPaths, relativePath = ""):
    for file in element.findall("File"):
        fileRelativePath = file.get("RelativePath").replace("\\", "/")
        fileRelativeFolder = fileRelativePath[0:fileRelativePath.rindex("/") + 1]
        found = False
        for testPath in allowedPaths:
            testWholePath = testPath + "/" + relativePath
            if fileRelativeFolder.endswith(testWholePath):
                found = True
        if not found:
            print("File path and filter do not match: " + fileRelativePath)
    
    for filter in element.findall("Filter"):
        processFilter(filter, allowedPaths, relativePath)

def processProjectFile(filePath):
    print()
    print("Processing " + os.path.basename(filePath) + ":")
    root = ET.parse(filePath).getroot()
    projectName = root.get("Name")
    files = root.find("Files")
    processFiles(files, projectFolders[projectName])

def processProjects():
    for file in fnmatch.filter(os.listdir(projectsPath), '*.vcproj'):
        processProjectFile(os.path.join(projectsPath, file))

# Program starts here

processProjects()
#        # Load existing XML tree
#        dependencies = ET.parse(argument).getroot()
