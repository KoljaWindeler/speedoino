package com.jkw.smartspeedo;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;
import android.view.Menu;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import android.view.View; 
import android.view.View.OnClickListener;

public class SmartSpeedoMain extends Activity implements OnClickListener {

	// surface
	GaugeCustomView speed;
	GaugeCustomView temp;
	GaugeCustomView gear;
	GaugeCustomView rpm;

	// to avoid shutdown
	PowerManager pm;
	WakeLock wl;

	// bluetooth
	private int bt_state=bluetooth_service.STATE_NONE;

	// activity codes
	private static final int REQUEST_CONNECT_DEVICE = 1;
	private static final int REQUEST_ENABLE_BT = 2;

	// Debug
	private static final String TAG = "JKW - SmartSpeedo";

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);

		// activate GPS 
		startService(new Intent(getBaseContext(), gps_service.class));
		LocalBroadcastManager.getInstance(this).registerReceiver(mGPSMsgRcv, new IntentFilter(gps_service.short_name));

		// activate BT
		startService(new Intent(getBaseContext(), bluetooth_service.class));
		LocalBroadcastManager.getInstance(this).registerReceiver(mBTMsgRcv, new IntentFilter(bluetooth_service.short_name));

		// buttons
		((Button)findViewById(R.id.button1)).setOnClickListener(this);
		((Button)findViewById(R.id.connect)).setOnClickListener(this);

		//views
		rpm=(GaugeCustomView)findViewById(R.id.rpm);
		speed=(GaugeCustomView)findViewById(R.id.speed);
		gear=(GaugeCustomView)findViewById(R.id.gear);
		temp=(GaugeCustomView)findViewById(R.id.temp);

		rpm.setLimits(0, 5500);
		rpm.setLayout(180, 270, 1000, 50);
		//		rpm.setLimits(0, 18000);
		//		rpm.setLayout(180, 270, 1000, 200);
		rpm.setValue(0);
		rpm.setType(GaugeCustomView.TYPE_RPM);


		speed.setLimits(0, 240);
		speed.setLayout(180, 270, 10, 2, 0,80);
		speed.setValue(0);
		speed.setType(GaugeCustomView.TYPE_KMH);

		gear.setLimits(0, 6);
		gear.setLayout(240, 240, 1, 0);
		gear.setValue(0);
		gear.setType(GaugeCustomView.TYPE_GEAR);

		temp.setLimits(40, 120);
		temp.setLayout(240, 240, 10, 2,60, 100);
		temp.setValue(40);
		temp.setType(GaugeCustomView.TYPE_TEMP);

		// let the scree stay on
		pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
		wl =  pm.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK, "My Tag");
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.smart_speedo_main, menu);
		return true;
	}

	@Override
	public void onClick(View v) {
		if(v.getId()==R.id.connect) {
			if(bt_state == bluetooth_service.STATE_NONE){
				// Launch the DeviceListActivity to see devices and do scan
				Intent serverIntent = new Intent(this, DeviceListActivity.class);
				startActivityForResult(serverIntent, REQUEST_CONNECT_DEVICE);
			} else {
				Intent intent = new Intent(bluetooth_service.to_name);
				intent.putExtra(bluetooth_service.BT_ACTION, bluetooth_service.BT_RESTART);
				LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
			}
		} else {
			speed.setValue(speed.getValue() + 10);
			temp.setValue(temp.getValue() + 5);
			gear.setValue(gear.getValue() + 1);
			rpm.setValue(rpm.getValue() + (int)(Math.random()*500));
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// Bluetooth startup & shutdown /////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////

	@Override
	public void onDestroy() {
		super.onDestroy();
		LocalBroadcastManager.getInstance(this).unregisterReceiver(mGPSMsgRcv);
		LocalBroadcastManager.getInstance(this).unregisterReceiver(mBTMsgRcv);
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// Bluetooth startup & shutdown /////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////// externe activitys steuern ///////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		Log.d(TAG, "onActivityResult " + resultCode + " " + requestCode);
		switch (requestCode) {
		case REQUEST_CONNECT_DEVICE:
			// When DeviceListActivity returns with a device to connect
			if (resultCode == Activity.RESULT_OK) {
				// Get the device MAC address
				String address = data.getExtras().getString(DeviceListActivity.EXTRA_DEVICE_ADDRESS);
				// Get the BLuetoothDevice object
				Intent intent = new Intent(bluetooth_service.to_name);
				intent.putExtra(bluetooth_service.BT_ACTION, bluetooth_service.BT_CONNECT);
				intent.putExtra(bluetooth_service.TARGET_ADDRESS, address);
				LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
			} 

			else {
				bt_state=bluetooth_service.STATE_NONE;
			}
			break;

			//		case REQUEST_SELECTED_DEVICE:
			//			if (resultCode == Activity.RESULT_OK) {
			//				// Get the device MAC address
			//				String address = data.getExtras().getString(
			//						DeviceListActivity.EXTRA_DEVICE_ADDRESS);
			//				// tell firmware update which device has been selected
			//				firmware_update(0,null,address);
			//			}
			//			break;

		case REQUEST_ENABLE_BT:
			// When the request to enable Bluetooth returns
			if (resultCode != Activity.RESULT_OK) {
				Log.d(TAG, "BT not enabled");
				Toast.makeText(this, "byby",Toast.LENGTH_LONG).show();
				finish();
			}
			break;
			//		case REQUEST_SHOW_MAP:
			//			Log.i(TAG, "Image converter hat was zurueckgegeben ");
			//			if (resultCode == RESULT_OK) {
			//				filePath = data.getStringExtra(FileDialog.RESULT_PATH);
			//				Log.i(TAG, "Der Resultcode war OK, der Pfad:" + filePath);
			//				intent = new Intent(getBaseContext(), RouteMap.class);
			//				intent.putExtra(RouteMap.INPUT_FILE_NAME, filePath);
			//				startActivityForResult(intent, REQUEST_SHOW_MAP_DONE);
			//			};
			//			break;
			//		case RESULT_CANCELED:
			//			Log.i(TAG, "File open abgebrochen");
			//			break;
		default:
			Log.i(TAG, "nicht gut, keine ActivityResultHandle gefunden");
			break;
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////// externe activitys steuern ///////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////// GPS steuern //////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
	private BroadcastReceiver mGPSMsgRcv = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			if(intent.getStringExtra(gps_service.MESSAGE)==gps_service.GPS_SPEED){
				speed.setValue(intent.getIntExtra(gps_service.GPS_SPEED, 0));
			}
			else if(intent.getStringExtra(gps_service.MESSAGE)==gps_service.GPS_SAT){
				String temp="GPS Status:";
				temp+=String.valueOf(intent.getIntExtra(gps_service.GPS_SAT_INFIX,0));
				temp+="/";
				temp+=String.valueOf(intent.getIntExtra(gps_service.GPS_SAT_TOTAL,0));

				((TextView)findViewById(R.id.sat)).setText(temp);
			}
		}
	};
	//////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////// GPS steuern //////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////// Bluetooth steuern ///////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
	private BroadcastReceiver mBTMsgRcv = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			// This is used to activate the bluetooth 
			if(intent.getStringExtra(bluetooth_service.BT_ACTION)==bluetooth_service.ENABLE_BT){
				Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
				startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
			}
			// update state
			else if(intent.getStringExtra(bluetooth_service.BT_ACTION)==bluetooth_service.BT_STATE_CHANGE){
				if(intent.getIntExtra(bluetooth_service.BT_STATE_CHANGE,0)==bluetooth_service.STATE_CONNECTED){
					Toast.makeText(getApplicationContext(),"Connected, Speedoino found", Toast.LENGTH_SHORT).show();
				} else if(intent.getIntExtra(bluetooth_service.BT_STATE_CHANGE,0)==bluetooth_service.STATE_CONNECTING){
					Toast.makeText(getApplicationContext(),"Connecting ...", Toast.LENGTH_SHORT).show();
				} else if(intent.getIntExtra(bluetooth_service.BT_STATE_CHANGE,0)==bluetooth_service.STATE_NONE){
					Toast.makeText(getApplicationContext(),"Connection closed...", Toast.LENGTH_SHORT).show();
				} else if(intent.getIntExtra(bluetooth_service.BT_STATE_CHANGE,0)==bluetooth_service.STATE_CONNECTED_AND_SEARCHING){
					Toast.makeText(getApplicationContext(),"Connected, scan for ID ...", Toast.LENGTH_SHORT).show();
				}
				bt_state=intent.getIntExtra(bluetooth_service.BT_STATE_CHANGE,0);
			}
			// update a sensor value
			else if(intent.getStringExtra(bluetooth_service.BT_ACTION)==bluetooth_service.BT_SENSOR_UPDATE){
				if(intent.getStringExtra(bluetooth_service.BT_SENSOR_UPDATE)==bluetooth_service.BT_SENSOR_WATER_TEMP_ANALOG){
					rpm.setValue(intent.getIntExtra(bluetooth_service.BT_SENSOR_VALUE, 0));
				}
			}
		}
	};
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////// Bluetooth steuern ///////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
}
