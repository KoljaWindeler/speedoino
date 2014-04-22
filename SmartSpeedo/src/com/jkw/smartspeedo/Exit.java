package com.jkw.smartspeedo;

import android.app.Activity;
import android.app.NotificationManager;
import android.os.Bundle;

public class Exit extends Activity {
	public static int notifiy_id;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		NotificationManager manager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE); 
		manager.cancel(notifiy_id); 
		System.exit(0);
	}
}
