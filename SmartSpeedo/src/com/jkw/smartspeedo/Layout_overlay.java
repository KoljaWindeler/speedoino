package com.jkw.smartspeedo;

import java.util.Calendar;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.PixelFormat;
import android.os.IBinder;
import android.support.v4.content.LocalBroadcastManager;
import android.view.Gravity;
import android.view.WindowManager;
import android.widget.Toast;

public class Layout_overlay extends Service {
    GaugeCustomView kmh_rpm;

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        kmh_rpm = new GaugeCustomView(this,600,600);
        WindowManager.LayoutParams params = new WindowManager.LayoutParams(
        		WindowManager.LayoutParams.FLAG_FULLSCREEN,
        		WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.TYPE_SYSTEM_OVERLAY,
                0,
//              WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE
//                      | WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE,
                PixelFormat.TRANSLUCENT);
   
        params.gravity = Gravity.RIGHT;
        params.horizontalMargin=0;
        params.verticalMargin=0;
        params.setTitle("Widgets");
        
        WindowManager wm = (WindowManager) getSystemService(WINDOW_SERVICE);
        wm.addView(kmh_rpm, params);
        
        kmh_rpm.setType(GaugeCustomView.TYPE_KMH_RPM);
        kmh_rpm.setLimits(0, 7000);
        kmh_rpm.setLayout(225, 255, 1000, 100);
        
        
        LocalBroadcastManager.getInstance(this).registerReceiver(mGPSMsgRcv, new IntentFilter(gps_service.short_name));
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Toast.makeText(getBaseContext(),"onDestroy", Toast.LENGTH_LONG).show();
        if(kmh_rpm != null)
        {
            ((WindowManager) getSystemService(WINDOW_SERVICE)).removeView(kmh_rpm);
            kmh_rpm = null;
        }
    }
    
    private BroadcastReceiver mGPSMsgRcv = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			Calendar c = Calendar.getInstance(); 
	        int seconds = c.get(Calendar.SECOND);
	        kmh_rpm.setValue(seconds*100);
		}
    };
    
    
}
