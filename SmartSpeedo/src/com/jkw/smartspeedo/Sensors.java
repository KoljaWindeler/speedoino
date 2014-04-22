package com.jkw.smartspeedo;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.List;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.location.Location;
import android.location.LocationManager;
import android.os.Environment;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;
import android.widget.Toast;

public class Sensors extends Service {

	public static final String PREFS_NAME = "SmartSpeedoSettings";
	public static final String short_name = "sensor_service";
	public static final String to_name = "to_sensor_service";
	SharedPreferences settings;

	// message id between sensor class and the main "layout" 
	public static final String SENSOR_ANALOG_RPM = "analog_rpm";
	public static final String SENSOR_SPEED = "speed";
	public static final String SENSOR_GEAR = "BT_SENSOR_GEAR";	
	public static final String SENSOR_WATER_TEMP = "water";
	public static final String SENSOR_AIR_ANALOG_TEMP = "air_analog";
	public static final String SENSOR_OIL_TEMP = "oil";
	public static final String SENSOR_VOLTAGE = "voltage";
	public static final String FLASHER_R ="BT_FLASHER_R";
	public static final String FLASHER_L ="BT_FLASHER_L";
	public static final String SENSOR_AIR_TEMP = "SENSOR_AIR_TEMP";

	@SuppressWarnings("unused")
	private gps_service gps;
	private bluetooth_service bluetooth;
	private Converter convert = new Converter();

	private int mCANSpeed;
	private int mReedSpeed;
	private int mRPM;
	private int mGear;
	private int mTempAirAnalog;

	private boolean CAN_active=false;

	private float[] gear_rations = new float[] { 0,0,0,0,0,0,0 };
	List<Location> gps_List = new ArrayList<Location>();

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		// activate GPS  
		gps = new gps_service(mGPShandle,getApplicationContext());
		// activate BT
		bluetooth = new bluetooth_service(mBluetoothHandle,getApplicationContext());

		// TODO
		//		settings = handle.getSharedPreferences(PREFS_NAME, 0);
		//		for(int i=0;i<7;i++){
		//			gear_rations[i]=settings.getFloat("G"+String.valueOf(i+1),0);
		//		}

		// the Receiver mMsgRcv is connected to the keyword bluetooth_service.to_name 
		LocalBroadcastManager.getInstance(this).registerReceiver(mMsgRcv, new IntentFilter(to_name));

		return Service.START_STICKY;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////// communication to us /////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////
	// listen to every input from the controlling activity, which ever app that might be
	private BroadcastReceiver mMsgRcv = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			// Restart bluetooth
			if(intent.getStringExtra(bluetooth_service.BT_ACTION)==bluetooth_service.BT_RESTART){
				bluetooth.restart();
			}

			// connect
			else if(intent.getStringExtra(bluetooth_service.BT_ACTION)==bluetooth_service.BT_CONNECT){
				String bt_adr=intent.getStringExtra(bluetooth_service.TARGET_ADDRESS);
				if(bt_adr!=""){
					bluetooth.connect(bt_adr);
				}
			}
		}
	};
	///////////////////////////////// communication to us /////////////////////////////////////////////
	///////////////////////////////// Bluetooth handling ///////////////////////////////////////////
	private final Handler mBluetoothHandle = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			if(msg.what==bluetooth_service.update){
				if(msg.getData().getInt(bluetooth_service.BT_COMMAND)==bluetooth_service.CMD_GET_WATER_TEMP_ANALOG){
					set_water(convert.water_r_to_t(msg.getData().getInt(bluetooth_service.BT_VALUE)));
				} else if(msg.getData().getInt(bluetooth_service.BT_COMMAND)==bluetooth_service.CMD_GET_AIR_TEMP_ANALOG){
					set_air_temp(msg.getData().getInt(bluetooth_service.BT_VALUE));
				} else if(msg.getData().getInt(bluetooth_service.BT_COMMAND)==bluetooth_service.CMD_GET_FREQ_RPM){
					set_rpm(convert.engine_freq_to_rpm(msg.getData().getInt(bluetooth_service.BT_VALUE)));
				}



			} else if (msg.getData().getString(bluetooth_service.BT_ACTION)==bluetooth.BT_STATE_CHANGE){
				set_bluetooth_state((int)msg.getData().getInt(bluetooth.BT_STATE_CHANGE));
			}

		}
	};
	///////////////////////////////// Bluetooth handling ///////////////////////////////////////////
	///////////////////////////////// gps handling ///////////////////////////////////////////
	private final Handler mGPShandle = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			if(msg.what==gps_service.GPS_SPEED_MSG){
				set_speed(msg.getData().getInt(gps_service.GPS_SPEED));
				set_gps_pos(msg.getData().getDouble(gps_service.GPS_LAT),msg.getData().getDouble(gps_service.GPS_LNG));
			} else if(msg.what == gps_service.GPS_SAT_MSG){
				set_sats(msg.getData().getInt(gps_service.GPS_SAT_INFIX),msg.getData().getInt(gps_service.GPS_SAT_TOTAL));
			}
		}
	};
	///////////////////////////////// gps handling ///////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// setter & getter //////////////////////////////////////////

	public void set_water(float temp) {
		// hier die best fit umrechnung 
		Intent intent = new Intent(short_name);
		intent.putExtra(bluetooth_service.BT_ACTION, bluetooth_service.BT_SENSOR_UPDATE);
		intent.putExtra(bluetooth_service.BT_SENSOR_UPDATE, SENSOR_WATER_TEMP);
		intent.putExtra(bluetooth_service.BT_SENSOR_VALUE, temp);
		LocalBroadcastManager.getInstance(this).sendBroadcast(intent);		
	}

	protected void set_bluetooth_state(int state) {
		Intent intent = new Intent(short_name);
		intent.putExtra(bluetooth.BT_ACTION, bluetooth.BT_STATE_CHANGE);
		intent.putExtra(bluetooth.BT_STATE_CHANGE, state);
		LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
	}

	protected void set_sats(int fix, int total) {
		Intent intent = new Intent(short_name);
		intent.putExtra(gps_service.MESSAGE, gps_service.GPS_SAT_BC);
		intent.putExtra(gps_service.GPS_SAT_INFIX, (int)fix);
		intent.putExtra(gps_service.GPS_SAT_TOTAL, (int)total);
		LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
	}

	protected void set_gps_pos(double lati, double longi) {
		Location location = new Location("");
		location.setLatitude(lati);
		location.setLongitude(longi);
		gps_List.add(location);
		if(gps_List.size()>60){
			save_gps_pos_to_file();
		}
	}

	public void set_flasher_left(int value) {
		boolean post_value;
		if(value==1){
			post_value=true;
		} else {
			post_value =false;
		}
		Intent intent = new Intent(short_name);
		intent.putExtra(bluetooth_service.BT_ACTION, bluetooth_service.BT_SENSOR_UPDATE);
		intent.putExtra(bluetooth_service.BT_SENSOR_UPDATE, FLASHER_L);
		intent.putExtra(bluetooth_service.BT_SENSOR_VALUE, post_value);
		LocalBroadcastManager.getInstance(this).sendBroadcast(intent);		
	}

	public void set_flasher_right(int value) {
		boolean post_value;
		if(value==1){
			post_value=true;
		} else {
			post_value =false;
		}
		Intent intent = new Intent(short_name);
		intent.putExtra(bluetooth_service.BT_ACTION, bluetooth_service.BT_SENSOR_UPDATE);
		intent.putExtra(bluetooth_service.BT_SENSOR_UPDATE, FLASHER_R);
		intent.putExtra(bluetooth_service.BT_SENSOR_VALUE, post_value);
		LocalBroadcastManager.getInstance(this).sendBroadcast(intent);		
	}

	public void set_rpm(int i) {
		Intent intent = new Intent(short_name);
		intent.putExtra(bluetooth_service.BT_ACTION, bluetooth_service.BT_SENSOR_UPDATE);
		intent.putExtra(bluetooth_service.BT_SENSOR_UPDATE, SENSOR_ANALOG_RPM);
		intent.putExtra(bluetooth_service.BT_SENSOR_VALUE, i);
		LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
		mRPM=i;
		calc_gear();
	}

	public void set_speed(int i) {
		//CAN_active=false;
		// this function will be called by bluetooth reed sensor, bluetooth can and gps class
		// conversion must be done in the upper class
		Intent intent = new Intent(short_name);
		intent.putExtra(bluetooth_service.BT_ACTION, bluetooth_service.BT_SENSOR_UPDATE);
		intent.putExtra(bluetooth_service.BT_SENSOR_UPDATE, SENSOR_SPEED);
		intent.putExtra(bluetooth_service.BT_SENSOR_VALUE, i);
		LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
		mReedSpeed=i;
		calc_gear();
	}

	public void set_air_temp(int i){
		Intent intent = new Intent(short_name);
		intent.putExtra(bluetooth_service.BT_ACTION, bluetooth_service.BT_SENSOR_UPDATE);
		intent.putExtra(bluetooth_service.BT_SENSOR_UPDATE, SENSOR_AIR_TEMP);
		intent.putExtra(bluetooth_service.BT_SENSOR_VALUE, convert.air(i));
		LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
		mTempAirAnalog=i;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// extendet functions //////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////


	public void calc_gear(){
		int temp_gear=-9;
		float temp_distance=999;

		float value=0;
		if(CAN_active){
			if(mCANSpeed!=0){
				value=mRPM/mCANSpeed;
			}
		} else {
			if(mReedSpeed!=0){
				value=mRPM/mReedSpeed;
			}
		} 


		for(int i=0;i<7;i++){
			if(Math.abs(value-gear_rations[i])<temp_distance){
				temp_distance=Math.abs(value-gear_rations[i]);
				temp_gear=i+1;
			}
		}

		if(temp_gear==7){
			temp_gear=-1;
		}

		mGear=temp_gear;

		Intent intent = new Intent(short_name);
		intent.putExtra(bluetooth_service.BT_ACTION, bluetooth_service.BT_SENSOR_UPDATE);
		intent.putExtra(bluetooth_service.BT_SENSOR_UPDATE, SENSOR_GEAR);
		intent.putExtra(bluetooth_service.BT_SENSOR_VALUE, mGear);
		LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
	}


	public void learn_gear(final int gear){
		if(gear<1 || gear>7){
			return;
		}

		float value=mReedSpeed;
		if(mCANSpeed!=0){
			value=mCANSpeed;
		}
		value=mRPM/value;
		gear_rations[gear-1]=value;

		SharedPreferences.Editor editor = settings.edit();
		editor.putFloat("G"+String.valueOf(gear), value);
		editor.commit();
	}


	public void save_gps_pos_to_file(){
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
	
	/////////////////////////////// extendet functions //////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////

	@Override
	public IBinder onBind(Intent arg0) {
		// TODO Auto-generated method stub
		return null;
	}

}
