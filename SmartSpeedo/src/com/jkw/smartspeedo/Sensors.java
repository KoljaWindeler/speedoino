package com.jkw.smartspeedo;

import android.content.Intent;
import android.content.SharedPreferences;
import android.support.v4.content.LocalBroadcastManager;

public class Sensors {

	public static final String PREFS_NAME = "SmartSpeedoSettings";
	private bluetooth_service mBTservice;
	SharedPreferences settings;

	private int mCANSpeed;
	private int mReedSpeed;
	private int mRPM;
	private int mGear;

	private boolean CAN_active=false;

	private float[] gear_rations = new float[] { 0,0,0,0,0,0,0 };

	public Sensors(bluetooth_service handle) {
		mBTservice=handle;

		settings = handle.getSharedPreferences(PREFS_NAME, 0);
		for(int i=0;i<7;i++){
			gear_rations[i]=settings.getFloat("G"+String.valueOf(i+1),0);
		}

	}

	public void set_water_analog(int resistance) {
		
		// hier die best fit umrechnung 
		int value = resistance;
		
		
		Intent intent = new Intent(bluetooth_service.short_name);
		intent.putExtra(bluetooth_service.BT_ACTION, bluetooth_service.BT_SENSOR_UPDATE);
		intent.putExtra(bluetooth_service.BT_SENSOR_UPDATE, bluetooth_service.BT_SENSOR_WATER_TEMP_ANALOG);
		intent.putExtra(bluetooth_service.BT_SENSOR_VALUE, value);
		LocalBroadcastManager.getInstance(mBTservice).sendBroadcast(intent);		
	}

	public void set_flasher_left(int value) {
		boolean post_value;
		if(value==1){
			post_value=true;
		} else {
			post_value =false;
		}
		Intent intent = new Intent(bluetooth_service.short_name);
		intent.putExtra(bluetooth_service.BT_ACTION, bluetooth_service.BT_SENSOR_UPDATE);
		intent.putExtra(bluetooth_service.BT_SENSOR_UPDATE, bluetooth_service.BT_FLASHER_L);
		intent.putExtra(bluetooth_service.BT_SENSOR_VALUE, post_value);
		LocalBroadcastManager.getInstance(mBTservice).sendBroadcast(intent);		
	}

	public void set_flasher_right(int value) {
		boolean post_value;
		if(value==1){
			post_value=true;
		} else {
			post_value =false;
		}
		Intent intent = new Intent(bluetooth_service.short_name);
		intent.putExtra(bluetooth_service.BT_ACTION, bluetooth_service.BT_SENSOR_UPDATE);
		intent.putExtra(bluetooth_service.BT_SENSOR_UPDATE, bluetooth_service.BT_FLASHER_R);
		intent.putExtra(bluetooth_service.BT_SENSOR_VALUE, post_value);
		LocalBroadcastManager.getInstance(mBTservice).sendBroadcast(intent);		
	}

	public void set_rpm(int i) {
		Intent intent = new Intent(bluetooth_service.short_name);
		intent.putExtra(bluetooth_service.BT_ACTION, bluetooth_service.BT_SENSOR_UPDATE);
		intent.putExtra(bluetooth_service.BT_SENSOR_UPDATE, bluetooth_service.BT_SENSOR_RPM);
		intent.putExtra(bluetooth_service.BT_SENSOR_VALUE, i);
		LocalBroadcastManager.getInstance(mBTservice).sendBroadcast(intent);
		mRPM=i;
		calc_gear();
	}

	public void set_speed_reed(int i) {
		CAN_active=false;
		Intent intent = new Intent(bluetooth_service.short_name);
		intent.putExtra(bluetooth_service.BT_ACTION, bluetooth_service.BT_SENSOR_UPDATE);
		intent.putExtra(bluetooth_service.BT_SENSOR_UPDATE, bluetooth_service.BT_SENSOR_SPEED_REED);
		intent.putExtra(bluetooth_service.BT_SENSOR_VALUE, i);
		LocalBroadcastManager.getInstance(mBTservice).sendBroadcast(intent);
		mReedSpeed=i;
		calc_gear();
	}

	public void set_speed_CAN(int i) {
		CAN_active=true;
		Intent intent = new Intent(bluetooth_service.short_name);
		intent.putExtra(bluetooth_service.BT_ACTION, bluetooth_service.BT_SENSOR_UPDATE);
		intent.putExtra(bluetooth_service.BT_SENSOR_UPDATE, bluetooth_service.BT_SENSOR_SPEED);
		intent.putExtra(bluetooth_service.BT_SENSOR_VALUE, i);
		LocalBroadcastManager.getInstance(mBTservice).sendBroadcast(intent);	
		mCANSpeed=i;
		calc_gear();
	}
	

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
		
		Intent intent = new Intent(bluetooth_service.short_name);
		intent.putExtra(bluetooth_service.BT_ACTION, bluetooth_service.BT_SENSOR_UPDATE);
		intent.putExtra(bluetooth_service.BT_SENSOR_UPDATE, bluetooth_service.BT_SENSOR_GEAR);
		intent.putExtra(bluetooth_service.BT_SENSOR_VALUE, mGear);
		LocalBroadcastManager.getInstance(mBTservice).sendBroadcast(intent);
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

}
