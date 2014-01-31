#!/usr/bin/python

from sys import argv, exit
from os import system, walk

# Script to set up the Android build environment



#0: Parse arguments

usage_string = "Usage: setup_android_project.py ProjectName"

if len(argv) < 2:
    print(usage_string)
    exit(1)

project = argv[1]
packagename = project.lower()    
project_dir = "."

if "PACKAGENAME" in project or "PROJECT" in project:
    print("hax")
    exit(1)


#1: Create project if not created

#system("android create project ...")
# or
#system("android update project ...")



#2: Copy the files from  trunk\Kanzi\Engine\sources\application_framework\platforms\android\java -> src

#system("cp -R ...")



#3: Walk the directories, replace "PROJECT" -> project and "PACKAGENAME" -> packagename in 
#     dirnames, filenames and contents

source_extensions = [".xml", ".java", ".c", ".h", ".cpp", ".properties"]

for root, dirs, files in walk(project_dir):
    
    for d in dirs:
        if "PROJECT" in d:
            system("mv " + d + " " + d.replace("PROJECT", project))
            dirs.remove(d)
            dirs.append(d.replace("PROJECT", project))
        if "PACKAGENAME" in d:
            system("mv " + d + " " + d.replace("PACKAGENAME", packagename))
            dirs.remove(d)
            dirs.append(d.replace("PACKAGENAME", packagename))
            
    for f in files:
        if "PROJECT" in f:
            system("mv " + f + " " + f.replace("PROJECT", project))
            files.remove(f)
            files.append(f.replace("PROJECT", project))
        if "PACKAGENAME" in f:
            system("mv " + f + " " + f.replace("PACKAGENAME", packagename))
            files.remove(f)
            files.append(f.replace("PACKAGENAME", packagename))
            
    for name in files:
        if True in [name.endswith(ext) for ext in source_extensions]:

            # read in the file and replace line by line
            f = file(name, 'r')
            lines = []
            for line in f:
                lines.append(line.replace("PROJECT", project).replace("PACKAGENAME", packagename))
            f.close()
            
            # write file
            f = file(name, 'w')
            f.writelines(lines)
            f.close()
            
