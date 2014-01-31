import os
import glob
import shutil
import datetime
import re
import sys
import fileinput
import string

if __name__ == "__main__":

# Clear directories
 os.system ("tolua\clear_directories.bat")


# Copy the required headers to pkg folder
 headerFiles = [        
    "user_layer/src/user/scene_graph/kzu_scene",
    "user_layer/src/user/scene_graph/kzu_object",
    "user_layer/src/user/scene_graph/kzu_mesh",
    "user_layer/src/user/scene_graph/kzu_camera",
    "user_layer/src/user/scene_graph/kzu_light",
    "user_layer/src/user/scene_graph/bounding_volume/kzu_bounding_volume",
    "user_layer/src/user/renderer/kzu_composer",
    "user_layer/src/user/renderer/kzu_render_pass",
    "user_layer/src/user/renderer/kzu_renderer",
    "user_layer/src/user/properties/kzu_property",
    "user_layer/src/user/properties/kzu_bool_property",
    "user_layer/src/user/properties/kzu_texture_property",
    "user_layer/src/user/properties/kzu_color_property",
    "user_layer/src/user/properties/kzu_float_property",
    "user_layer/src/user/properties/kzu_light_property",
    "user_layer/src/user/properties/kzu_matrix2x2_property",
    "user_layer/src/user/properties/kzu_matrix3x3_property",
    "user_layer/src/user/properties/kzu_matrix4x4_property",
    "user_layer/src/user/properties/kzu_string_property",
    "user_layer/src/user/properties/kzu_vector2_property",
    "user_layer/src/user/properties/kzu_vector3_property",
    "user_layer/src/user/properties/kzu_vector4_property",    
    "user_layer/src/user/project/kzu_project",
    "user_layer/src/user/material/kzu_material",
    "user_layer/src/user/engine/kzu_engine",
    "user_layer/src/user/engine/kzu_engine_message",
    "user_layer/src/user/common/kzu_user",
    "user_layer/src/user/ui/kzu_ui_action",
    "user_layer/src/user/ui/kzu_ui_component",
    "user_layer/src/user/ui/kzu_ui_component_type",
    "user_layer/src/user/ui/kzu_ui_event",
    "user_layer/src/user/ui/kzu_ui_event_listener",
    "user_layer/src/user/ui/kzu_ui_screen"
    # Add all properties here
    # Add animation
    # Add filters    
 ]
 
 # These are required as they describe some used structs
 additionalIncludes = [        
    "#include <core/util/color/kzc_color.h>",
    "#include <core/util/math/kzc_vector2.h>",
    "#include <core/util/math/kzc_vector3.h>",
    "#include <core/util/math/kzc_vector4.h>",
    "#include <core/util/math/kzc_matrix2x2.h>",
    "#include <core/util/math/kzc_matrix3x3.h>",
    "#include <core/util/math/kzc_matrix4x4.h>",
    "#include <user/properties/kzu_property.h>",
    "#include <user/properties/kzu_property_base.h>",
    "#include <user/properties/kzu_fixed_properties.h>",
    "#include <user/engine/kzu_engine_message_queue.h>",
    "#include <core/util/collection/kzc_dynamic_array.h>",
  ]
    
 sourcePath = "../sources/"
 for i in headerFiles:
 
  splitString = i.split("/")
  fileBaseName = splitString[len(splitString)-1]
  
  sourceFile = "%s%s.h" % (sourcePath, i)
  targetFile = "tolua/pkg/%s.pkg" % (fileBaseName);
  print("Copying %s to %s" % (sourceFile, targetFile));
  os.system ("copy %s %s" % (sourceFile.replace("/", "\\"), targetFile.replace("/", "\\")))

  # Build the include path by splitting source path
  includePath = "#include <"
  startAppendingInclude = 0
  
  for n in splitString:    
   if startAppendingInclude > 0:
    if startAppendingInclude > 1:
     includePath = "%s/%s" % (includePath, n)  
    else:
     includePath = "%s%s" % (includePath, n)
    startAppendingInclude = startAppendingInclude + 1
   if n == "src":
    startAppendingInclude = 1

  includePath = "%s.h>\n" % (includePath)

  f = open(targetFile, 'r')
  fileContent = f.read()
  f.close()
  newFileContent = "%s%s" % (includePath, fileContent)
  for item in additionalIncludes:
   newFileContent = "%s\n%s" % (item, newFileContent) 
  f2 = open(targetFile, 'w')
  f2.write(newFileContent)
  f2.close()



# Preprocess PKG
 sourceTexts = [
    "#include",
    "/**<",
    "kzString",
    "kzFloat",
    "kzDouble",
    "kzInt",
    "kzUint",
    "kzChar",
 ]
 targetTexts = [
    "$#include",
    "/*",
    "char*",
    "float",
    "double",
    "int",
    "unsigned int",
    "char",
 ]
 removeLinesWhere = [
    "const KzuObjectType KZU",
    "extern const kzString",
    "extern const char*",
    "extern const Kzu",
    "typedef kzsError (*" # Function pointers of type kzsError
 ]  
  
 removeText = "#define"

 path = "tolua/pkg"
 dirList = os.listdir(path)

 for pkgFile in dirList:
  removedFirstDefine = 0
  file = os.path.join(path, pkgFile)
  if file.find(".svn") == -1:

   f = open(file, "r")
   content = f.read()    
   p = re.compile('(?<=\nstruct )(?P<first>[^;]*){.*}', re.DOTALL)
   content = re.sub(p, "\g<first>", content)
   f.close()   
   f2 = open(file, "w")
   f2.write(content)
   f2.close()   
   
   files = glob.glob(file)
   
   for line in fileinput.input(files,inplace=1):
    lineno = 0
    
    if removedFirstDefine == 0:
     if line.find(removeText) > -1:
       line = ""
       removedFirstDefine = 1
    
    for index, sourceText in enumerate(sourceTexts):
      targetText = targetTexts[index]
      p = line.find(sourceText)
      if p > -1:
        line = line.replace(sourceText, targetText)

    writeOut = 1
    
    for item in removeLinesWhere:
      if line.find(item) > -1:
       writeOut = 0
    
    if writeOut == 1:
     sys.stdout.write(line)


 

# Create bindings with tolua 
 os.system ("tolua\create_lua_binds.bat")

 
# Clear bindings as post step
 sourceTexts = [
    "sizeof(KzcDynamicArrayIterator)",
    " = (struct KzcDynamicArrayIterator)",
    "sizeof(KzuProjectObjectIterator)",
    " = (struct KzuProjectObjectIterator)",
    "sizeof(KzcMatrix4x4)",
    " = (struct KzcMatrix4x4)",
    "sizeof(KzcMatrix3x3)",
    " = (struct KzcMatrix3x3)",
    "sizeof(KzcMatrix2x2)",
    " = (struct KzcMatrix2x2)",
    "sizeof(KzcVector4)",
    " = (struct KzcVector4)",
    "sizeof(KzcVector3)",
    " = (struct KzcVector3)",
    "sizeof(KzcVector2)",
    " = (struct KzcVector2)",
    "sizeof(KzcColorRGBA)",
    " = (struct KzcColorRGBA)",
    "sizeof(KzuPropertyPriority)",
    "sizeof(KzuPropertyDataType)",
    "TOLUA_API",
    #"int",
    #"float",
    "};",
    "//#ifndef TOLUA_DISABLE",
    "tolua_constant(tolua_S,\"kzuProjectObjectIterate(iterator_param)\",kzuProjectObjectIterate(iterator_param));",
    "tolua_constant(tolua_S,\"kzuProjectObjectIteratorGetValue(iterator_param)\",kzuProjectObjectIteratorGetValue(iterator_param));",
    "typedef KzsError",
    " = (struct KzcRay)",
    "sizeof(KzcRay)",
    "struct const"
 ]
 targetTexts = [        
    "sizeof(struct KzcDynamicArrayIterator)",
    " = ",
    "sizeof(struct KzuProjectObjectIterator)",
    " = ",
    "sizeof(struct KzcMatrix4x4)",
    " = ",
    "sizeof(struct KzcMatrix3x3)",
    " = ",
    "sizeof(struct KzcMatrix2x2)",
    " = ",
    "sizeof(struct KzcVector4)",
    " = ",
    "sizeof(struct KzcVector3)",
    " = ",
    "sizeof(struct KzcVector2)",
    " = ",
    "sizeof(struct KzcColorRGBA)",
    " = ",
    "sizeof(enum KzuPropertyPriority)",
    "sizeof(enum KzuPropertyDataType)",
    "",
    #"kzInt",
    #"kzFloat",
    "}",
    "/* #ifndef TOLUA_DISABLE */",
    "",
    "",
    "KzsError",
    " = ",
    "sizeof(struct KzcRay)",
    "const struct"
 ]
 
 path = "tolua/c_to_lua"
 dirList = os.listdir(path)

 for pkgFile in dirList:
  file = os.path.join(path, pkgFile)
  if file.find(".svn") == -1:
   
   files = glob.glob(file)
   
   for line in fileinput.input(files,inplace=1):
    lineno = 0
       
    for index, sourceText in enumerate(sourceTexts):
      targetText = targetTexts[index]
      p = line.find(sourceText)
      if p > -1:
        line = line.replace(sourceText, targetText)

    sys.stdout.write(line)
 
 os.system ("@pause")