package com.rightware.kanzi;

import android.content.Context;
import java.io.*;


class SurfaceConfig
{
   static int[] values =
	 {
	 	  -1,
	 	  -1,
		 	-1,
	    -1,
	    -1,
	    -1,
	    -1,
	    -1,
	    -1,
	    -1,
	    -1,
	    -1
	 };   

	 static String[] lines = 
	 {
		 	"WindowResolutionX",
	    "WindowResolutionY",
	    "Fullscreen",
	    "SurfaceBitsRed",
	    "SurfaceBitsGreen",
	    "SurfaceBitsBlue",
	    "SurfaceBitsAlpha",
	    "SurfaceBitsStencil",
	    "SurfaceBitsDepthBuffer",
	    "SurfaceSamplesAntialiasing"
	 };
	 
	 static int getValue(int id)
	 {
	 		return values[id];	 	
	 }
	 
	 static int getValue(String name)
	 {
			for(int i=0; i<lines.length; i++)
			{
		 			if(lines[i].compareTo(name) == 0)
		 			{
		 					return values[i];
		 			}
			}
			return -1;
	 }

   static void parseLine(String line)
   {
       if(line != null && !line.startsWith("#"))
       {
       		for(int i=0; i<lines.length; i++)
       		{
         		String toFind = lines[i];
         		if(line.regionMatches(true, 0, toFind, 0, toFind.length()))
         		{
         			line = line.replaceAll(";.*", "");
         			line = line.replaceAll("[^0-9]", "");
         			values[i] = Integer.valueOf(line);
         		}
         	}
     	 }
   }

   /* Tries first load from filename. After that if first failed uses assetName. */
   static boolean readFile(String fileName, Context c, String assetName) 
   {
      BufferedReader br = null;
      try 
      {
   			 FileReader fr = new FileReader(fileName);
         br = new BufferedReader(fr);
      }
      catch (FileNotFoundException e) 
      {
         System.out.println("FileNotFoundException reading file '" + fileName + "'. Fallbacking to default configuration file.");
      }
      
      if(br == null)
      {
          try 
          {
             InputStream is = c.getAssets().open(assetName);
             InputStreamReader in= new InputStreamReader(is);
             br = new BufferedReader(in);
          }
          catch (IOException e)
          {
             System.out.println("IOException reading asset file '" + assetName + "'");
             return false;
          }
      }
      
      try 
      {
          String record = null;
          record = new String();
          while ((record = br.readLine()) != null) 
          {
         		 parseLine(record.trim());
          }
      }
      catch (IOException e) 
      {
         System.out.println("IOException reading configuration file.");
      }
      
      return true;
   }
}
