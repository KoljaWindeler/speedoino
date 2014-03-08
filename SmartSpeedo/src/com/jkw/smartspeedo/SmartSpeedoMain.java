package com.jkw.smartspeedo;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.view.Menu;
import android.widget.Button;
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

	// Bluetooth
	public BluetoothAdapter mBluetoothAdapter = null;
	public static BluetoothSerialService mSerialService = null;

	// open public visible vars for the bluetooth class to write in
	public static final String DEVICE_NAME = "device_name";
	public static final String TOAST = "toast";
	public static final String result = "result";
	
	public static final String SENSOR_VALUE = "value";
	public static final String SENSOR_TYPE = "sensor";	

	private static final int REQUEST_CONNECT_DEVICE = 1;
	private static final int REQUEST_ENABLE_BT = 2;


	// Debug
	private static final String TAG = "JKW - SmartSpeedo";

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);

		((Button)findViewById(R.id.button1)).setOnClickListener(this);
		((Button)findViewById(R.id.connect)).setOnClickListener(this);

		rpm=(GaugeCustomView)findViewById(R.id.rpm);
		speed=(GaugeCustomView)findViewById(R.id.speed);
		gear=(GaugeCustomView)findViewById(R.id.gear);
		temp=(GaugeCustomView)findViewById(R.id.temp);

		rpm.setLimits(0, 18000);
		rpm.setLayout(180, 270, 1000, 200, 18000);
		rpm.setValue(0);
		rpm.setType(GaugeCustomView.TYPE_RPM);
		

		speed.setLimits(0, 300);
		speed.setLayout(180, 270, 10, 2, 100);
		speed.setValue(0);
		speed.setType(GaugeCustomView.TYPE_KMH);

		gear.setLimits(0, 6);
		gear.setLayout(240, 240, 1, 0, 6);
		gear.setValue(0);
		gear.setType(GaugeCustomView.TYPE_GEAR);

		temp.setLimits(40, 120);
		temp.setLayout(240, 240, 10, 2, 100);
		temp.setValue(70);
		temp.setType(GaugeCustomView.TYPE_TEMP);

		// let the scree stay on
		pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
		wl =  pm.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK, "My Tag");

		// prepare bluetooth
		mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
		// If the adapter is null, then Bluetooth is not supported
		if (mBluetoothAdapter == null) {
			Toast.makeText(this, "Bluetooth is not available",Toast.LENGTH_LONG).show();
			return;
		}
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.smart_speedo_main, menu);
		return true;
	}

	@Override
	public void onClick(View v) {
		if(v.getId()==R.id.connect) {
			if (mSerialService.getState() == BluetoothSerialService.STATE_NONE) {
				// Launch the DeviceListActivity to see devices and do scan
				Intent serverIntent = new Intent(this, DeviceListActivity.class);
				startActivityForResult(serverIntent, REQUEST_CONNECT_DEVICE);
			} else {
				mSerialService.stop();
				mSerialService.start();
			}
		} else {
			// TODO Auto-generated method stub
			speed.setValue(speed.getValue() + 10);
			temp.setValue(temp.getValue() + 5);
			gear.setValue(gear.getValue() + 1);
			rpm.setValue(rpm.getValue() + (int)(Math.random()*500)-200);
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// Bluetooth startup & shutdown /////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
	@Override
	public void onStart() {
		super.onStart();
		// If BT is not on, request that it be enabled.
		// setupChat() will then be called during onActivityResult
		if (mBluetoothAdapter != null) {
			if (!mBluetoothAdapter.isEnabled()) {
				Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
				startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
				// Otherwise, setup the chat session
			} else {
				if (mSerialService == null){
					setupBT();
				}
			}
		}
	}

	private void setupBT() {
		Log.d(TAG, "setupBT()");
		// Initialize the BluetoothChatService to perform bluetooth connections
		mSerialService = new BluetoothSerialService(this, mHandlerBT);
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		if (mSerialService != null)
			mSerialService.stop();
		Log.e(TAG, "--- ON DESTROY ---");

	}

	// The Handler that gets information back from the BluetoothService
	private final Handler mHandlerBT = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {

			// state switch
			case BluetoothSerialService.MESSAGE_SENSOR_VALUE:
				rpm.setValue(msg.getData().getInt(SENSOR_VALUE));
				break;
			
			case BluetoothSerialService.MESSAGE_STATE_CHANGE:
				Log.i(TAG, "MESSAGE_STATE_CHANGE: " + msg.arg1);

				switch (msg.arg1) {
				case BluetoothSerialService.STATE_CONNECTED:
					//					if (mStatus != null) {
					//						mStatus.setText("Connected,Speedoino found");
					//					};
					Toast.makeText(getApplicationContext(),"Connected, Speedoino found", Toast.LENGTH_SHORT).show();
					//mTimerHandle.postDelayed(mCheckVer, 500);
					break;

				case BluetoothSerialService.STATE_CONNECTING:
					Toast.makeText(getApplicationContext(),"Connecting ...", Toast.LENGTH_SHORT).show();
					break;

				case BluetoothSerialService.STATE_NONE:
					Toast.makeText(getApplicationContext(),"Connection closed...", Toast.LENGTH_SHORT).show();
					break;

				case BluetoothSerialService.STATE_CONNECTED_AND_SEARCHING:
					Toast.makeText(getApplicationContext(),"Connected, scan for ID ...", Toast.LENGTH_SHORT).show();
					break;
				}
				break;

				// display popup
				//			case MESSAGE_TOAST: // ?
				//				toast = Toast.makeText(getApplicationContext(), msg.getData()
				//						.getString(TOAST), Toast.LENGTH_SHORT);
				//				setStatusLastCommand(msg.getData().getInt(result));
				//				toast.show();
				//				break;
				//
				//			case MESSAGE_SET_LOG:
				//				mLog.setText(msg.getData().getString(TOAST));
				//				break;

				//			case MESSAGE_SET_VERSION:
				//				mVersion.setText(msg.getData().getString(TOAST));
				//				break;

				//			case MESSAGE_CMD_UNKNOWN:
				//				mLog.setText(R.string.unknown);
				//				toast = Toast.makeText(getApplicationContext(),
				//						R.string.unknown, Toast.LENGTH_SHORT);
				//				toast.show();
				//				break;

				//			case MESSAGE_CMD_FAILED:
				//				mLog.setText(R.string.noresponse);
				//				toast = Toast.makeText(getApplicationContext(),
				//						R.string.noresponse, Toast.LENGTH_SHORT);
				//				toast.show();
				//				break;
			}
		}
	};
	//////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// Bluetooth startup & shutdown /////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////// externe activitys steuern ///////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		Log.d(TAG, "onActivityResult " + resultCode + " " + requestCode);
		String filePath;
		Intent intent;
		switch (requestCode) {
		case REQUEST_CONNECT_DEVICE:
			// When DeviceListActivity returns with a device to connect
			if (resultCode == Activity.RESULT_OK) {
				// Get the device MAC address
				String address = data.getExtras().getString(
						DeviceListActivity.EXTRA_DEVICE_ADDRESS);
				// Get the BLuetoothDevice object
				BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(address);
				// Attempt to connect to the device
				Log.e(TAG, "Device selected, connecting ...");
				try {
					mSerialService.connect(device,false);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
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
			if (resultCode == Activity.RESULT_OK) {
				// Log.d(LOG_TAG, "BT not enabled");
				// finishDialogNoBluetooth();
				setupBT();
			} else {
				// User did not enable Bluetooth or an error occurred
				Log.d(TAG, "BT not enabled");
				Toast.makeText(this, "byby",1).show();
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
		case RESULT_CANCELED:
			Log.i(TAG, "File open abgebrochen");
			break;
		default:
			Log.i(TAG, "nicht gut, keine ActivityResultHandle gefunden");
			break;
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////// externe activitys steuern ///////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////

}
