package com.rightware.kanzi.kzbplayer;

import android.app.Activity;
import android.os.Bundle;
import com.rightware.kanzi.*;

public class KZBPlayer extends Activity
{
    KanziView mView;

    @Override protected void onCreate(Bundle icicle)
    {
        super.onCreate(icicle);
        mView = new KanziView(getApplication());
        setContentView(mView);
    }

    @Override protected void onPause()
    {
        super.onPause();
        mView.onPause();
    }

    @Override protected void onResume()
    {
        super.onResume();
        mView.onResume();
    }
}
