package com.jkw.smartspeedo;

import java.util.Calendar;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.PixelFormat;
import android.graphics.Point;
import android.os.Bundle;
import android.os.IBinder;
import android.support.v4.content.LocalBroadcastManager;
import android.view.Display;
import android.view.Gravity;
import android.view.WindowManager;
import android.widget.Toast;

public class Layout_overlay extends Service {
	GaugeCustomView kmh_rpm;
	public static int width; 
	public static int height;

	public static final String STOP_SERVICE = "STOP_SERICE";	// argument for BT action
	public static final String LAYOUT_OVERLAY_SERVICE = "STOP_SERICE";	// argument for BT action

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		//TODO do something useful
		return Service.START_NOT_STICKY;
	}


	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}


	@Override
	public void onCreate() {
		super.onCreate();

		kmh_rpm = new GaugeCustomView(this,Math.max(width, height)/3,Math.max(width, height)/3);


		WindowManager.LayoutParams params = new WindowManager.LayoutParams();
		params.width=Math.max(width, height)/3;
		params.height=Math.max(width, height)/3;
		params.format=PixelFormat.TRANSLUCENT;
		params.type=WindowManager.LayoutParams.TYPE_SYSTEM_OVERLAY;
		params.gravity = Gravity.RIGHT;
		params.horizontalMargin=0;
		params.verticalMargin=0;
		params.setTitle("SmartSpeedoWidgets");

		WindowManager wm = (WindowManager) getSystemService(WINDOW_SERVICE);
		wm.addView(kmh_rpm, params);

		kmh_rpm.setType(GaugeCustomView.TYPE_KMH_RPM);
		kmh_rpm.setLimits(0, 7000);
		kmh_rpm.setLayout(225, 255, 1000, 100);


		LocalBroadcastManager.getInstance(this).registerReceiver(mSensorMsgRcv, new IntentFilter(Sensors.short_name));
		LocalBroadcastManager.getInstance(this).registerReceiver(mShutdownRcv, new IntentFilter(LAYOUT_OVERLAY_SERVICE));
		createNotification();
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		if(kmh_rpm != null)
		{
			((WindowManager) getSystemService(WINDOW_SERVICE)).removeView(kmh_rpm);
			kmh_rpm = null;
		}
	}

	private BroadcastReceiver mShutdownRcv = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			LocalBroadcastManager.getInstance(context).unregisterReceiver(mSensorMsgRcv);
			LocalBroadcastManager.getInstance(context).unregisterReceiver(mShutdownRcv);
			stopSelf();			
		}
	};

	private BroadcastReceiver mSensorMsgRcv = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			if(intent.getStringExtra(bluetooth_service.BT_SENSOR_UPDATE)==Sensors.SENSOR_SPEED){
				kmh_rpm.setSecondValue(intent.getIntExtra(bluetooth_service.BT_SENSOR_VALUE, 0), 2);
			}

			if(intent.getStringExtra(gps_service.MESSAGE)==gps_service.GPS_SAT_BC){
				kmh_rpm.setSecondValue(intent.getIntExtra(gps_service.GPS_SAT_INFIX,0), 3);
			} 


			Calendar c = Calendar.getInstance(); 
			int seconds = c.get(Calendar.SECOND);
			kmh_rpm.setValue(seconds*100);

		}
	};

	public void createNotification() {
		// Prepare intent which is triggered if the
		// notification is selected
		Intent intent = new Intent(this, SmartSpeedoMain.class);
		Intent intent_exit = new Intent(this, Exit.class);
		Exit.notifiy_id=0;

		PendingIntent pIntent = PendingIntent.getActivity(this, 0, intent, 0);
		PendingIntent pIntentExit = PendingIntent.getActivity(this, 0, intent_exit, 0);

		// Build notification
		// Actions are just fake
		Notification noti = new Notification.Builder(this)
		.setContentTitle("SmartSpeedo running in Backgroud")
		.setContentText("Tab to resume").setSmallIcon(R.drawable.icon)
		.setContentIntent(pIntent)
		.addAction(R.drawable.icon, "Exit SmartSpeedo", pIntentExit)
		.build();

		NotificationManager notificationManager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
		// hide the notification after its selected
		noti.flags |= Notification.FLAG_AUTO_CANCEL;

		notificationManager.cancel(0);
		notificationManager.notify(0, noti);
	}


}
