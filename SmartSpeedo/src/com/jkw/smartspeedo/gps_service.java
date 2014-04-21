package com.jkw.smartspeedo;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.location.GpsSatellite;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;
import android.widget.Toast;

public class gps_service implements android.location.GpsStatus.Listener, LocationListener {

	public static final String short_name = "gps_service";
	public static final String MESSAGE = "gps_message";
	private Handler mHandle;
	private Context mContext;
	
	// handle identifier kategorie
	public static final int GPS_SPEED_MSG = 1;
	public static final int GPS_SAT_MSG = 0;

	// broadcast identifier
	public static final String GPS_SPEED_BC = "speed update";
	public static final String GPS_SAT_BC = "sat upate";
	
	// sub kat
	public static final String GPS_SAT_INFIX = "in_fix";
	public static final String GPS_SAT_TOTAL = "total";
	public static final String GPS_SPEED = "speed";
	public static final String GPS_LAT = "LAT";
	public static final String GPS_LNG = "LNG";

	private LocationManager locationManager;
	

	public gps_service(Handler mGPShandle, Context context) {
		mHandle = mGPShandle;
		mContext = context;
		locationManager = (LocationManager) mContext.getSystemService(Context.LOCATION_SERVICE);                
		locationManager.addGpsStatusListener(this);
		locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 1000,	0, this);
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

		// Send the name of the connected device back to the UI Activity
        Message msg = mHandle.obtainMessage(GPS_SAT_MSG);
        Bundle bundle = new Bundle();
        bundle.putInt(GPS_SAT_INFIX, (int)SatellitesInFix);
        bundle.putInt(GPS_SAT_TOTAL, (int)Satellites);
        msg.setData(bundle);
        mHandle.sendMessage(msg);

	}

	@Override
	public void onLocationChanged(Location location) {		
		// Send the name of the connected device back to the UI Activity
        Message msg = mHandle.obtainMessage(GPS_SPEED_MSG);
        Bundle bundle = new Bundle();
        bundle.putInt(GPS_SPEED, (int)(location.getSpeed()*3.6));
        bundle.putDouble(GPS_LAT, location.getLatitude());
        bundle.putDouble(GPS_LNG, location.getLongitude());
        msg.setData(bundle);
        mHandle.sendMessage(msg);
	}

	@Override
	public void onProviderDisabled(String provider) { }

	@Override
	public void onProviderEnabled(String provider) { }

	@Override
	public void onStatusChanged(String provider, int status, Bundle extras) {	}

}
