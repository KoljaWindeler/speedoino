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
import android.os.IBinder;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;
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
	public static final String GPS_LAT = "LAT";
	public static final String GPS_LNG = "LNG";

	private LocationManager locationManager;
	List<Location> gps_List = new ArrayList<Location>();

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		locationManager = (LocationManager) getSystemService(Context.LOCATION_SERVICE);                
		locationManager.addGpsStatusListener(this);
		locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 400,	0, this);

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

		intent.putExtra(GPS_LAT, location.getLatitude());
		intent.putExtra(GPS_LNG, location.getLongitude());

		LocalBroadcastManager.getInstance(this).sendBroadcast(intent);

		gps_List.add(location);
		if(gps_List.size()>60){
			save_to_file();
		}
	}

	public void save_to_file(){
		Log.d("update", "onActivityResult ");

		//////////////////// FOLDER //////////////////////////////////
		// create dir if not existent
		File sdCard = Environment.getExternalStorageDirectory(); //
		File dir = new File(sdCard.getAbsolutePath() + "/SmartSpeedo"); // /mnt/sdcard/Download/
		String dl_basedir;
		if (!dir.exists()) {
			if (dir.mkdir()) {
				File temp_dir = new File(dir.getAbsolutePath() + "/GPS");
				temp_dir.mkdir();
				dl_basedir = dir.getAbsolutePath() + "/";
			} else {
				dl_basedir = sdCard.getAbsolutePath() + "/";
				Toast.makeText(this,"Can't create directory on SD card", Toast.LENGTH_LONG).show();
			}
		} else {
			dl_basedir = dir.getAbsolutePath() + "/";
		}
		//////////////////// FOLDER //////////////////////////////////
		//////////////////// FILE //////////////////////////////////

		Calendar c = Calendar.getInstance();
		SimpleDateFormat df = new SimpleDateFormat("yyyy_MM_dd");
		String formattedDate = df.format(c.getTime())+".gps";
		File output = new File(dl_basedir+"GPS/"+formattedDate);

		////////////////////FILE //////////////////////////////////

		FileOutputStream out = null;
		try {
			out = new FileOutputStream(output,true);
			for(int i=0; i<gps_List.size(); i++){
				out.write((String.valueOf(gps_List.get(i).getLatitude())+","+String.valueOf(gps_List.get(i).getLongitude())).getBytes());
				out.write("\r\n".getBytes());
			}
			out.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		//////////////
		gps_List.clear();
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
