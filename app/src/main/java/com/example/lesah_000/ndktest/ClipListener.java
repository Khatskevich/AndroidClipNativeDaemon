package com.example.lesah_000.ndktest;

import android.app.IntentService;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.widget.Toast;

/**
 * Created by lesah_000 on 3/21/2015.
 */

public class ClipListener extends IntentService {
    public ClipboardManager mClipboard;
    public Context context;
    private Handler handler = new Handler();
    public ClipListener() {
        super("Name for Service");
    }

    @Override
    protected void onHandleIntent(Intent workIntent) {
        System.out.println("Clip Listener working!");
        context = getApplicationContext();
        mClipboard = (ClipboardManager) getSystemService(CLIPBOARD_SERVICE);
        mClipboard.addPrimaryClipChangedListener(new ClipboardListener() );
        while(1>0) {
            try {
                System.out.println("Clip Listener working!");
                Thread.sleep(4000);
            } catch (Exception e) {
                e.getLocalizedMessage();
            }

        }
    }

    class ClipboardListener implements ClipboardManager.OnPrimaryClipChangedListener
    {
        public void onPrimaryClipChanged()
        {
            ClipData abc = mClipboard.getPrimaryClip();
            ClipData.Item item = abc.getItemAt(0);
            final String text = item.getText().toString();
            System.out.println(text);

            handler.post(new Runnable() {
                @Override
                public void run() {
                    Toast.makeText(context, "Clip provides data:" + text, Toast.LENGTH_SHORT).show();
                }
            });
            DataAvailableJNI();
        }
    }

    public native int DataAvailableJNI();
};