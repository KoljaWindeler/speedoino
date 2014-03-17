package com.jkw.smartspeedo;

import com.google.android.gms.internal.ct;
import com.google.android.gms.maps.CameraUpdate;
import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.GoogleMap.OnCameraChangeListener;
import com.google.android.gms.maps.MapFragment;
import com.google.android.gms.maps.model.CameraPosition;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.MarkerOptions;

import android.os.Bundle;
import android.os.Handler;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;
import android.view.Menu;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View; 
import android.view.View.OnClickListener;

public class SmartSpeedoMain extends Activity implements OnClickListener {

	// surface
	GaugeCustomView speed;
	GaugeCustomView temp;
	GaugeCustomView gear;
	GaugeCustomView rpm;

	// surface 2
	ControllCustomView ctrl;
	
	// to avoid shutdown
	PowerManager pm;
	WakeLock wl;

	// bluetooth
	private int bt_state=bluetooth_service.STATE_NONE;

	//map
	private boolean map_Zoom_changed=false;
	private GoogleMap map;

	// activity codes
	private static final int REQUEST_CONNECT_DEVICE = 1;
	private static final int REQUEST_ENABLE_BT = 2;

	// Debug
	private static final String TAG = "JKW - SmartSpeedo";
	private Handler mTimerHandle = new Handler();

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		map = ((MapFragment)getFragmentManager().findFragmentById(R.id.map)).getMap();
		map.setMapType(GoogleMap.MAP_TYPE_NORMAL);
		map.setMyLocationEnabled(true);
		map.setTrafficEnabled(true);

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
		ctrl=((ControllCustomView)findViewById(R.id.ctrl));
		
//		rpm.getLayoutParams().height *= 1.8;
//		rpm.getLayoutParams().width *= 1.8;
//		
//		speed.getLayoutParams().width *= 1.8;
//		speed.getLayoutParams().height *= 1.8;
//		
//		gear.getLayoutParams().width *= 1.8;
//		gear.getLayoutParams().height *= 1.8;
//		
//		temp.getLayoutParams().width *= 1.8;
//		temp.getLayoutParams().height *= 1.8;

		rpm.setLimits(0, 5500);
		rpm.setLayout(270, 270, 1000, 50);
		//		rpm.setLimits(0, 18000);
		//		rpm.setLayout(180, 270, 1000, 200);
		rpm.setValue(0);
		rpm.setType(GaugeCustomView.TYPE_RPM);


		speed.setLimits(0, 240);
		speed.setLayout(0, 270, 10, 2, 0,80);
		speed.setValue(0);
		speed.setType(GaugeCustomView.TYPE_KMH);

		gear.setLimits(0, 6);
		gear.setLayout(240, 240, 1, 0);
		gear.setValue(0);
		gear.setType(GaugeCustomView.TYPE_GEAR);

		temp.setLimits(40, 120);
		temp.setLayout(240, 240, 10, 2,60, 100);
		temp.setValue(40);
		temp.setSecondValue(40);
		temp.setType(GaugeCustomView.TYPE_TEMP);
//		temp.setValueCount(2);

		// let the scree stay on
		pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
		wl =  pm.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK, "My Tag");
	}


	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		//		menu.add(1, Menu.FIRST, Menu.FIRST, "");
		MenuInflater menuInflater = getMenuInflater();
		menuInflater.inflate(R.menu.smart_speedo_main, menu);
		return true;
	}

	@Override 
	public boolean onOptionsItemSelected(MenuItem item) {
		if(item.getItemId()==1) {
			if(bt_state == bluetooth_service.STATE_NONE){
				// Launch the DeviceListActivity to see devices and do scan
				Intent serverIntent = new Intent(this, DeviceListActivity.class);
				startActivityForResult(serverIntent, REQUEST_CONNECT_DEVICE);
			} else {
				Intent intent = new Intent(bluetooth_service.to_name);
				intent.putExtra(bluetooth_service.BT_ACTION, bluetooth_service.BT_RESTART);
				LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
				((Button)findViewById(R.id.connect)).setText("Connect");
			}
		}
		return super.onOptionsItemSelected(item);
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
				((Button)findViewById(R.id.connect)).setText("Connect");
			}
		} else {
			speed.setValue(speed.getValue() + 10);
			temp.setValue(temp.getValue() + 5);
//			temp.setSecondValue(temp.getValue() - 5);
			gear.setValue(gear.getValue() + 1);
			rpm.setValue(rpm.getValue() + (int)(Math.random()*500));
			
			ctrl.set_left(!ctrl.get_left());
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

				((Button)findViewById(R.id.connect)).setText("Disconnect");
			} 

			else {
				bt_state=bluetooth_service.STATE_NONE;
			}
			break;

		case REQUEST_ENABLE_BT:
			// When the request to enable Bluetooth returns
			if (resultCode != Activity.RESULT_OK) {
				Log.d(TAG, "BT not enabled");
				Toast.makeText(this, "byby",Toast.LENGTH_LONG).show();
				finish();
			}
			break;

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
				mTimerHandle.removeCallbacks(mCheckResponseTimeTaskGPS);
				mTimerHandle.postDelayed(mCheckResponseTimeTaskGPS, 1000);

				LatLng pos = new LatLng(intent.getDoubleExtra(gps_service.GPS_LAT, 0), intent.getDoubleExtra(gps_service.GPS_LNG, 0));
				float zoom;
				if(map_Zoom_changed){
					zoom=map.getCameraPosition().zoom;
				} else {
					zoom=13;
				}
				CameraUpdate update = CameraUpdateFactory.newLatLngZoom(pos,zoom);
				map.animateCamera(update);
			}
			else if(intent.getStringExtra(gps_service.MESSAGE)==gps_service.GPS_SAT){
				String temp="GPS Status:";
				temp+=String.valueOf(intent.getIntExtra(gps_service.GPS_SAT_INFIX,0));
				temp+="/";
				temp+=String.valueOf(intent.getIntExtra(gps_service.GPS_SAT_TOTAL,0));

				((TextView)findViewById(R.id.sat)).setText(temp);
				
				if(intent.getIntExtra(gps_service.GPS_SAT_INFIX,0)>2){
					ctrl.set_GPS(true);
				} else {
					ctrl.set_GPS(false);
				}
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
				// reset time
				mTimerHandle.removeCallbacks(mCheckResponseTimeTask);
				mTimerHandle.postDelayed(mCheckResponseTimeTask, 2000);

				// set new values
				if(intent.getStringExtra(bluetooth_service.BT_SENSOR_UPDATE)==bluetooth_service.BT_SENSOR_RPM){
					rpm.setValue(intent.getIntExtra(bluetooth_service.BT_SENSOR_VALUE, 0));
				}

				if(intent.getStringExtra(bluetooth_service.BT_SENSOR_UPDATE)==bluetooth_service.BT_FLASHER_L){
					ctrl.set_left(intent.getBooleanExtra(bluetooth_service.BT_SENSOR_VALUE, false));
				}

				if(intent.getStringExtra(bluetooth_service.BT_SENSOR_UPDATE)==bluetooth_service.BT_FLASHER_R){
					ctrl.set_right(intent.getBooleanExtra(bluetooth_service.BT_SENSOR_VALUE, false));
				}
				
				if(intent.getStringExtra(bluetooth_service.BT_SENSOR_UPDATE)==bluetooth_service.BT_SENSOR_GEAR){
					gear.setValue(intent.getIntExtra(bluetooth_service.BT_SENSOR_VALUE, 0));
				}
			}
		}
	};

	// resetter
	private Runnable mCheckResponseTimeTask = new Runnable() {
		public void run() {
			rpm.setValue(0);
			temp.setValue(0);
			gear.setValue(0);
		}
	};
	
	private Runnable mCheckResponseTimeTaskGPS = new Runnable() {
		public void run() {
			speed.setValue(0);
		}
	};

	//////////////////////////////////// Bluetooth steuern ///////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
}
