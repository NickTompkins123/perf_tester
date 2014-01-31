package com.rightware.kanzi;


import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.util.Log;
import android.widget.Toast;
import android.content.Context;

import java.io.IOException;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.Closeable;


class ErrorBox
{
    static interface onErrorOk
    {
        void onErrorOk();
    }
    
    static interface onWarningOk
    {
        void onWarningOk();
    }
	
    static void showError(final Activity activity, final CharSequence text)
    {        
          AlertDialog.Builder alertbox = new AlertDialog.Builder(activity);
          alertbox.setMessage(text);

          alertbox.setNeutralButton("Ok", new DialogInterface.OnClickListener() 
          {
              public void onClick(DialogInterface arg0, int arg1) 
              {
                ((onErrorOk)activity).onErrorOk();     	
              }
          });

          alertbox.show();
    }
    
    static void showWarning(final Activity activity, final CharSequence text)
    {        
          AlertDialog.Builder alertbox = new AlertDialog.Builder(activity);
          alertbox.setMessage(text);

          alertbox.setNeutralButton("Ok", new DialogInterface.OnClickListener() 
          {
              public void onClick(DialogInterface arg0, int arg1) 
              {
                ((onWarningOk)activity).onWarningOk();
              }
          });

          alertbox.show();
    }

}
