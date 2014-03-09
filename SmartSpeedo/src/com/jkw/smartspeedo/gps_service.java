package com.jkw.smartspeedo;

import android.content.Context;
import android.location.GpsSatellite;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;

public class gps_service implements android.location.GpsStatus.Listener, LocationListener {

	public static final int MESSAGE_SPEED = 0;
	public static final int MESSAGE_SATS = 1;

	public static final String GPS_SPEED = "speed";
	public static final String GPS_SAT_INFIX = "in_fix";
	public static final String GPS_SAT_TOTAL = "total";



	private LocationManager locationManager;
	private Handler mHandler;

	public gps_service(Context context, Handler handler) {
		mHandler = handler;
		locationManager = (LocationManager) context.getSystemService(Context.LOCATION_SERVICE);                
		locationManager.addGpsStatusListener(this);
		locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 0,	0, this);
	}


	@Override
	public void onGpsStatusChanged(int event) {
		int Satellites = 0;
		int SatellitesInFix = 0;
		//	    int timetofix = locationManager.getGpsStatus(null).getTimeToFirstFix();
		//	    Log.i(TAG, "Time to first fix = "+String.valueOf(timetofix));
		for (GpsSatellite sat : locationManager.getGpsStatus(null).getSatellites()) {
			if(sat.usedInFix()) {
				SatellitesInFix++;              
			}
			Satellites++;
		}

		Message msg = mHandler.obtainMessage(MESSAGE_SATS);
		Bundle bundle = new Bundle();
		bundle.putInt(GPS_SAT_INFIX, (int)SatellitesInFix);
		bundle.putInt(GPS_SAT_TOTAL, (int)Satellites);
		msg.setData(bundle);
		mHandler.sendMessage(msg);
	}

	@Override
	public void onLocationChanged(Location location) {
		// Send the name of the connected device back to the UI Activity
		Message msg = mHandler.obtainMessage(MESSAGE_SPEED);
		Bundle bundle = new Bundle();
		bundle.putInt(GPS_SPEED, (int)location.getSpeed());
		msg.setData(bundle);
		mHandler.sendMessage(msg);
	}

	@Override
	public void onProviderDisabled(String provider) { }

	@Override
	public void onProviderEnabled(String provider) { }

	@Override
	public void onStatusChanged(String provider, int status, Bundle extras) {	}
}
