package com.rightware.kanzi;

import java.io.IOException;
import java.io.InputStream;

import android.content.Context;
import android.util.Log;

public class KanziResourceFile
{
    private static final long KZS_RESOURCE_FILE_ERROR = -1;
	private Context context = null;
    private static KanziResourceFile instance = new KanziResourceFile();

    public void setContext(Context context)
    {
        this.context = context;
    }

    public static KanziResourceFile getInstance()
    {
        return instance;
    }

    public class ResourceFile
    {
        public byte[] data;
        public int bufferSize;
        public InputStream stream;
    }

    public ResourceFile APKOpenFile(String filename)
    {
        ResourceFile resourceFile = new ResourceFile();
        resourceFile.stream = null;
        resourceFile.bufferSize = 0;

        try
        {
            resourceFile.stream = context.getAssets().open(filename);
            resourceFile.bufferSize = 1024;
            resourceFile.data = new byte[resourceFile.bufferSize];
        }
        catch (IOException e)
        {
            Log.w("KanziResource", "APKOpenFile failed: " + filename);
            resourceFile = null;
        }
        return resourceFile;
    }

    public long APKReadFile(ResourceFile file, int size)
    {
        long bytesRead = 0;
        
        if (file == null)
        {
        	Log.e("KanziResource", "APKReadFile failed");
        	return KZS_RESOURCE_FILE_ERROR;
        }        
        
        if (size > file.bufferSize)
        {
            file.data = new byte[size];
            file.bufferSize = size;
        }
        try
        {
            bytesRead = file.stream.read(file.data, 0, size);
        }
        catch (IOException e)
        {
            Log.w("KanziResource", "APKReadFile failed: " + e.toString() + " caused by " + e.getCause().toString() + ": " + e.getLocalizedMessage().toString());
        }
        
        return bytesRead;
    }

    public long APKSkipFile(ResourceFile file, int offset)
    {
        long bytesSkipped = 0;

        try
        {
            bytesSkipped = file.stream.skip(offset);
        }
        catch (IOException e)
        {
            Log.w("KanziResource", "APKSkipFile failed");
        }

        return bytesSkipped;
    }

    public void APKCloseFile(ResourceFile file)
    {       
        try
        {
            file.stream.close();
        }
        catch (IOException e)
        {
            Log.w("KanziResource", "APKCloseFile failed");
        }
        file.data = null;
        file.stream = null;
    }
}
