package com.jkw.smartspeedo;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.location.GpsSatellite;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.support.v4.content.LocalBroadcastManager;
import android.widget.Toast;

public class gps_service extends Service implements android.location.GpsStatus.Listener, LocationListener {

	public static final String short_name = "gps_service";
	public static final String MESSAGE = "gps_message";
	
	// kategorie
	public static final String GPS_SPEED = "speed";
	public static final String GPS_SAT = "sat";
	
	// sub kat
	public static final String GPS_SAT_INFIX = "in_fix";
	public static final String GPS_SAT_TOTAL = "total";

	private LocationManager locationManager;


	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		locationManager = (LocationManager) getSystemService(Context.LOCATION_SERVICE);                
		locationManager.addGpsStatusListener(this);
		locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 0,	0, this);

		return Service.START_STICKY;
	}


	@Override
	public void onGpsStatusChanged(int event) {
		int Satellites = 0;
		int SatellitesInFix = 0;

		for (GpsSatellite sat : locationManager.getGpsStatus(null).getSatellites()) {
			if(sat.usedInFix()) {
				SatellitesInFix++;              
			}
			Satellites++;
		}

		Intent intent = new Intent(short_name);

		intent.putExtra(MESSAGE, GPS_SAT);
		intent.putExtra(GPS_SAT_INFIX, (int)SatellitesInFix);
		intent.putExtra(GPS_SAT_TOTAL, (int)Satellites);

		LocalBroadcastManager.getInstance(this).sendBroadcast(intent);

	}

	@Override
	public void onLocationChanged(Location location) {
		Intent intent = new Intent(short_name);

		intent.putExtra(MESSAGE, GPS_SPEED);
		intent.putExtra(GPS_SPEED, (int)(location.getSpeed()*3.6));

		LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
	}

	@Override
	public void onProviderDisabled(String provider) { }

	@Override
	public void onProviderEnabled(String provider) { }

	@Override
	public void onStatusChanged(String provider, int status, Bundle extras) {	}


	@Override
	public IBinder onBind(Intent arg0) {		return null;	}

	@Override
	public void onDestroy() {
		super.onDestroy();
//		Toast.makeText(this, "Service Destroyed", Toast.LENGTH_LONG).show();
	}
}
