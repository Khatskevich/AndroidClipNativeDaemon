package com.example.lesah_000.ndktest;

import android.app.Activity;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Toast;

public class MainActivity extends Activity {
        private static Intent serviceValera;
        private static Intent serviceClipListener;
        public static ClipboardManager mClipboard;
        public Context context;
        private Handler handler = new Handler();
        @Override
        protected void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);
            setContentView(R.layout.activity_main);
            context = this;
            context = getApplicationContext();
            mClipboard = (ClipboardManager) getSystemService(CLIPBOARD_SERVICE);

            if ( ! ServiceTools.isServiceRunning(context, "Valera") ) {
                MainActivity.serviceValera= new Intent(context, Valera.class);
                context.startService(MainActivity.serviceValera  );
            }
            /*if ( ! ServiceTools.isServiceRunning(context, "ClipListener") ) {
                MainActivity.serviceClipListener = new Intent(context, ClipListener.class);
                context.startService(MainActivity.serviceClipListener  );
            }*/



            mClipboard.addPrimaryClipChangedListener(new ClipboardListener() );

        }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }
    // valera
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
    static
    {
        System.loadLibrary("HelloJNI");
    }


    class ClipboardListener implements ClipboardManager.OnPrimaryClipChangedListener
    {
        public void onPrimaryClipChanged()
        {
            ClipData abc = mClipboard.getPrimaryClip();



            try {
                final String label = abc.getDescription().getLabel().toString();
                if ( label.contentEquals("VBOX_CLIP_DATA"))
                {
                    return;
                }
            }catch (Exception e){
            }
            final int ret = DataAvailableJNI();
            handler.post(new Runnable() {
                @Override
                public void run() {
                    Toast.makeText(context, "Clip provides data ret = "+ ret , Toast.LENGTH_SHORT).show();
                }
            });
            System.out.println( "DataAvailableJNI" );
        }
    }

    public native int DataAvailableJNI();
}
