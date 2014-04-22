package com.jkw.smartspeedo;

import android.os.Bundle;
import android.os.Handler;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.app.Activity;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.bluetooth.BluetoothAdapter;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Point;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;
import android.view.Menu;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import android.view.Display;
import android.view.KeyEvent;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View; 
import android.view.WindowManager;
import android.view.View.OnClickListener;

public class SmartSpeedoMain extends Activity implements OnClickListener {

	Layout GUI_map = new Layout_map();
	Layout GUI_big = new Layout_big();
	Layout GUI;

	// to avoid shutdown
	PowerManager pm;
	WakeLock wl;

	// bluetooth
	private int bt_state=bluetooth_service.STATE_NONE;

	//mapcontroll
	private boolean map_Zoom_changed=false;
	private int gui_mode=0;
	private int back_pushed=0;

	// activity codes
	private static final int REQUEST_CONNECT_DEVICE = 1;
	private static final int REQUEST_ENABLE_BT = 2;

	// Debug
	private static final String TAG = "JKW - SmartSpeedo";
	private Handler mTimerHandle = new Handler();
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		GUI = GUI_big;
		setContentView(GUI.get_layout());
		GUI.find_elements(this);


		// activate sensors service
		startService(new Intent(getBaseContext(), Sensors.class));
		LocalBroadcastManager.getInstance(this).registerReceiver(mSensorMsgRcv, new IntentFilter(Sensors.short_name));

		// let the scree stay on
		pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
		wl =  pm.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK, "My Tag");

		// remove on top view, if there is one
		Intent intent = new Intent(Layout_overlay.LAYOUT_OVERLAY_SERVICE);
		intent.putExtra(Layout_overlay.STOP_SERVICE, Layout_overlay.STOP_SERVICE);
		LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
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
				Intent intent = new Intent(Sensors.to_name);
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
				Intent intent = new Intent(Sensors.to_name);
				intent.putExtra(bluetooth_service.BT_ACTION, bluetooth_service.BT_RESTART);
				LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
				((Button)findViewById(R.id.connect)).setText("Connect");
			}
		} else {
			if(gui_mode==0){
				GUI.unregister_elements(this);
				GUI=GUI_map;
				gui_mode=2;
				setContentView(GUI.get_layout());
				GUI.find_elements(this);
			} else if(gui_mode==1){
				GUI.unregister_elements(this);
				GUI=GUI_big;
				gui_mode=2;
				setContentView(GUI.get_layout());
				GUI.find_elements(this);
			} else if(gui_mode==2){
				GUI.unregister_elements(this);
				GUI=GUI_map;
				gui_mode=0;


				Display display = getWindowManager().getDefaultDisplay();
				Point size = new Point();
				display.getSize(size);
				
				Intent service = new Intent(getBaseContext(), Layout_overlay.class);
				Layout_overlay.width=size.x;
				Layout_overlay.height=size.y;
								
				startService(service);
				finish();
			}

			//			if(GUI.equals(GUI_map)){
			//				//				startService(new Intent(getBaseContext(), Layout_overlay.class));
			//				GUI.unregister_elements(this);
			//				GUI=GUI_big;
			//			} else {
			//				GUI=GUI_map;
			//			}

		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// Bluetooth startup & shutdown /////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////

	@Override
	public void onDestroy() {
		super.onDestroy();
		LocalBroadcastManager.getInstance(this).unregisterReceiver(mSensorMsgRcv);
	}

	@Override
	public void onStop(){
//		stopService(sensor_intent); // todo
		super.onStop();
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
				Intent intent = new Intent(Sensors.to_name);
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
	///////////////////////////////// GPS / Bluetooth steuern ////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
	private BroadcastReceiver mSensorMsgRcv = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			Log.i("test","jetzt");
			if(intent.getStringExtra(gps_service.MESSAGE)==gps_service.GPS_SPEED){
				//				GUI.setSpeed(intent.getIntExtra(gps_service.GPS_SPEED, 0));

				mTimerHandle.removeCallbacks(mCheckResponseTimeTaskGPS);
				mTimerHandle.postDelayed(mCheckResponseTimeTaskGPS, 1100);

				GUI.setMap(intent.getDoubleExtra(gps_service.GPS_LAT, 0), intent.getDoubleExtra(gps_service.GPS_LNG, 0));
			}
			else if(intent.getStringExtra(gps_service.MESSAGE)==gps_service.GPS_SAT_BC){
				String temp="GPS:";
				temp+=String.valueOf(intent.getIntExtra(gps_service.GPS_SAT_INFIX,0));
				temp+="/";
				temp+=String.valueOf(intent.getIntExtra(gps_service.GPS_SAT_TOTAL,0));

				((TextView)findViewById(R.id.sat)).setText(temp);
				if(intent.getIntExtra(gps_service.GPS_SAT_INFIX,0)>2){
					GUI.setCTRL_GPS(true);
				} else {
					GUI.setCTRL_GPS(false);
				}
			}

			else if(intent.getStringExtra(bluetooth_service.BT_ACTION)==bluetooth_service.ENABLE_BT){
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
				//				mTimerHandle.postDelayed(mCheckResponseTimeTask, 2000);

				// set new values
				if(intent.getStringExtra(bluetooth_service.BT_SENSOR_UPDATE)==Sensors.SENSOR_ANALOG_RPM){
					GUI.setRPM(intent.getIntExtra(bluetooth_service.BT_SENSOR_VALUE, 0));
				}

				else if(intent.getStringExtra(bluetooth_service.BT_SENSOR_UPDATE)==Sensors.SENSOR_SPEED){
					GUI.setSpeed(intent.getIntExtra(bluetooth_service.BT_SENSOR_VALUE, 0));
				}

				else if(intent.getStringExtra(bluetooth_service.BT_SENSOR_UPDATE)==Sensors.FLASHER_L){
					GUI.setCTRL_left(intent.getBooleanExtra(bluetooth_service.BT_SENSOR_VALUE, false));
				}

				else if(intent.getStringExtra(bluetooth_service.BT_SENSOR_UPDATE)==Sensors.FLASHER_R){
					GUI.setCTRL_right(intent.getBooleanExtra(bluetooth_service.BT_SENSOR_VALUE, false));
				}

				else if(intent.getStringExtra(bluetooth_service.BT_SENSOR_UPDATE)==Sensors.SENSOR_GEAR){
					GUI.setGear(intent.getIntExtra(bluetooth_service.BT_SENSOR_VALUE, 0));
				}

				else if(intent.getStringExtra(bluetooth_service.BT_SENSOR_UPDATE)==Sensors.SENSOR_AIR_TEMP){
					GUI.setAirTemp(intent.getFloatExtra(bluetooth_service.BT_SENSOR_VALUE,0));
				}
			}
		}
	};

	// resetter
	private Runnable mCheckResponseTimeTask = new Runnable() {
		public void run() {
			GUI.setSpeed(0);
			GUI.setRPM(0);
			GUI.setGear(0);
		}
	};

	private Runnable mCheckResponseTimeTaskGPS = new Runnable() {
		public void run() {
		}
	};
	

	//////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////// GPS / Bluetooth steuern ////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
	
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if ((keyCode == KeyEvent.KEYCODE_BACK)) {
            if (back_pushed == 1) {
            	stopService(new Intent(getBaseContext(), Sensors.class));          // todo  	
                finish();
            } else {
                Toast.makeText(this,"push twice to exit",Toast.LENGTH_LONG).show();
                back_pushed = 1;
                // install guard, 2sec until check of receive
                mTimerHandle.removeCallbacks(mCheckDoublePushBack);
                mTimerHandle.postDelayed(mCheckDoublePushBack, 2000);
            }
        } else if (keyCode == KeyEvent.KEYCODE_HOME) {
            finish();
        } else {
            return super.onKeyDown(keyCode, event);
        }
        return true;
    }

    // just resetting the "back" pushed
    private Runnable mCheckDoublePushBack = new Runnable() {
        public void run() {
            back_pushed = 0;
        }
    };
}
