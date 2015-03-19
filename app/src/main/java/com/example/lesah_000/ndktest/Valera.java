package com.example.lesah_000.ndktest;

/**
 * Created by lesah_000 on 3/19/2015.
 */

import android.app.IntentService;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.widget.Toast;


public class Valera extends IntentService {
    public Valera() {
        super("Name for Service");
    }

    @Override
    protected void onHandleIntent(Intent workIntent) {

        String dataString = workIntent.getDataString();
        while(1>0) {
            ClipboardManager clipboard = (ClipboardManager) this.getApplicationContext().getSystemService(Context.CLIPBOARD_SERVICE);
            ClipData clip = ClipData.newPlainText("simple text", HelloJNI()  );
            clipboard.setPrimaryClip(clip);
        }

    }
    private native String HelloJNI();
};