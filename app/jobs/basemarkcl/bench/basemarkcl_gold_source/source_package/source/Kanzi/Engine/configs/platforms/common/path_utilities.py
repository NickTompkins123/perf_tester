"""
Directory manipulation and handling utilities for Kanzi build flow.
"""

import os
import os.path
import glob

# Find leaf directories. If required_extension is not empty, the
# returned directory must contain at least one file with this extension
def find_leaf_directories(source_dir, exclude = [], required_extension = ".c"):
    leafs = []
    extension_len = len(required_extension)
    for path, dirs, files in os.walk(source_dir):
        dirs = [dir for dir in dirs if str(dir) != ".svn"] 
        if files and not dirs and path not in exclude:
            # If required file extension specified, check that this condition is fullfilled
            if extension_len > 0:
                spec_files = glob.glob(os.path.join(path, "*" + required_extension))
                if spec_files:
                    leafs.append(os.path.abspath(path))
            else:
                leafs.append(os.path.abspath(path))
    return leafs

def initialize_directory_path(path):
    """
    Create the directory hierarchy required to validate path. 
    If path exists, and is a directory, do nothing
    If path exists, and is a file, throw error
    """
    if os.path.exists(path):
        if os.path.isfile(path):
            raise Exception("Requested directory " + str(path) + " cannot be initialized as operation would overwrite an existing file")
    else:
        os.makedirs(path)

