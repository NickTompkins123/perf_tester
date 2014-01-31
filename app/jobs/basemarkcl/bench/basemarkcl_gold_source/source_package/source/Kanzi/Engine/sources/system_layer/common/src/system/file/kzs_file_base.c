/**
* \file
* Base functions for file related operations.
* 
* Copyright 2008-2011 by Rightware. All rights reserved.
*/
#include "kzs_file_base.h"


static kzString g_kzsResourceDirectory = KZ_NULL;


void kzsFileBaseSetResourceDirectory(kzString path)
{
    g_kzsResourceDirectory = path;
}

kzString kzsFileBaseGetResourceDirectory()
{
    return g_kzsResourceDirectory;
}
