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
    public ClipboardManager mClipboard;
    public Context context;
    private Handler handler = new Handler();
    public Valera() {
        super("Name for Service");
    }

    @Override
    protected void onHandleIntent(Intent workIntent) {
        System.out.println("Valera working!");
        context = getApplicationContext();
        mClipboard = (ClipboardManager) getSystemService(CLIPBOARD_SERVICE);

        while(1>0) {

            final String mesg = HelloJNI();
            System.out.println("HelloJniMesg = " + mesg);
            System.out.println("HelloJniMesgLen = "+ mesg.length());
            if ( mesg.length()!=0 ) {
                ClipData clip = ClipData.newPlainText("simple text", mesg);
                mClipboard.setPrimaryClip(clip);
                handler.post(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(context, "Clip gets data!: " + mesg, Toast.LENGTH_SHORT).show();
                    }
                });
            }else{
                ClipData abc = mClipboard.getPrimaryClip();
                ClipData.Item item = abc.getItemAt(0);
                final String text = item.getText().toString();
                final int ret =DataSendToHostJNI(text);

                handler.post(new Runnable() {
                    @Override
                    public void run() {
                        Toast.makeText(context, "Clip send data: "+text+"ret = " + ret, Toast.LENGTH_SHORT).show();
                    }
                });
            }
            try {
                Thread.sleep(8000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
    private native String HelloJNI();
    public native int DataAvailableJNI();
    public native int DataSendToHostJNI( String mesg);
};