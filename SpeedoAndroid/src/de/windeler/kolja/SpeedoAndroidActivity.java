
package de.windeler.kolja;


import android.app.Activity;
import android.app.TabActivity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.content.res.Resources;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.TabHost;
import android.widget.TextView;
import android.widget.Toast;

public class SpeedoAndroidActivity extends TabActivity  {
	// Name of the connected device
	private String mConnectedDeviceName = null;
	private static final String TAG = "JKW";

	private MenuItem mMenuItemConnect;
	private BluetoothAdapter mBluetoothAdapter = null;
	public static final String DEVICE_NAME = "device_name";
	public static final String TOAST = "toast";
	public static BluetoothSerialService mSerialService = null;
	/**
	 * Our main view. Displays the emulated terminal screen.
	 */
	public TextView mLog;

	// Message types sent from the BluetoothReadService Handler
	public static final int MESSAGE_STATE_CHANGE = 1;
	public static final int MESSAGE_READ = 2;
	public static final int MESSAGE_WRITE = 3;
	public static final int MESSAGE_DEVICE_NAME = 4;
	public static final int MESSAGE_TOAST = 5;
	private static final int REQUEST_CONNECT_DEVICE = 1;
	private static final int REQUEST_ENABLE_BT = 2;

	// String buffer for outgoing messages
	private StringBuffer mOutStringBuffer;
	public TextView mTest;


	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);

		Resources res = getResources(); // Resource object to get Drawables
		TabHost tabHost = getTabHost();  // The activity TabHost
		TabHost.TabSpec spec;  // Resusable TabSpec for each tab
//		Intent intent;  // Reusable Intent for each tab
	
		// add orders tab
        spec = tabHost.newTabSpec("orders").setIndicator("Connect",
                          res.getDrawable(R.drawable.ic_tab_connect))
                          .setContent(R.id.connectLayout);
        tabHost.addTab(spec);

        // add positions tab
        spec = tabHost.newTabSpec("positions").setIndicator("Upload",
                          res.getDrawable(R.drawable.ic_tab_connect))
                          .setContent(R.id.uploadLayout);
        tabHost.addTab(spec);

        // add strategies tab
        spec = tabHost.newTabSpec("strategies").setIndicator("Download",
                          res.getDrawable(R.drawable.ic_tab_connect))
                          .setContent(R.id.downloadLayout);
        tabHost.addTab(spec);	
		// layout ende
		
        mLog = (TextView) findViewById(R.id.status_value);

		mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
		// If the adapter is null, then Bluetooth is not supported
		if (mBluetoothAdapter == null) {
			Toast.makeText(this, "Bluetooth is not available", Toast.LENGTH_LONG).show();
			finish();
			return;
		}

	}

	 public void setLog(CharSequence status){
	    	mLog.setText(status);
	    }
	
	 public void appendLog(CharSequence status){
	    	mLog.append(status);
	    }
	 
	@Override
	public void onStart() {
		super.onStart();
		Log.e(TAG, "++ ON START ++");

		// If BT is not on, request that it be enabled.
		// setupChat() will then be called during onActivityResult
		if (!mBluetoothAdapter.isEnabled()) {
			Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
			startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
			// Otherwise, setup the chat session
		} else {
			if (mSerialService == null) setupBT();
		}
	}


	private void setupBT() {
		Log.d(TAG, "setupBT()");

		// Initialize the BluetoothChatService to perform bluetooth connections
		mSerialService = new BluetoothSerialService(this, mHandlerBT);
		mOutStringBuffer = new StringBuffer("");
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		// Stop the Bluetooth chat services
		if (mSerialService != null) mSerialService.stop();
		Log.e(TAG, "--- ON DESTROY ---");
	}


	/**
	 * Sends a message.
	 * @param message  A string of text to send.
	 */
	private void sendMessage(String message) {
		// Check that we're actually connected before trying anything
		if (mSerialService.getState() != BluetoothSerialService.STATE_CONNECTED) {
			Toast.makeText(this, R.string.not_connected, Toast.LENGTH_SHORT).show();
			return;
		}

		// Check that there's actually something to send
		if (message.length() > 0) {
			// Get the message bytes and tell the BluetoothChatService to write
			byte[] send = message.getBytes();
			mSerialService.write(send);

			// Reset out string buffer to zero and clear the edit text field
			mOutStringBuffer.setLength(0);
		}
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.menu, menu);
		mMenuItemConnect = menu.getItem(0);
		//menu.add(0,EDIT_CONTACT, 0, "Edit"). setIcon(R.drawable.edit_icon); 
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
		case R.id.menu_connect:
			Log.e(TAG, "Menu -> connect");
			if (mSerialService.getState() == BluetoothSerialService.STATE_NONE) {
				// Launch the DeviceListActivity to see devices and do scan
				Intent serverIntent = new Intent(this, DeviceListActivity.class);
				startActivityForResult(serverIntent, REQUEST_CONNECT_DEVICE);
			}
			else {
				//if (mSerialService.getState() == BluetoothSerialService.STATE_CONNECTED) {
					mSerialService.stop();
					mSerialService.start();
				}
			return true;
		case R.id.menu_settings:
			//doPreferences();
			return true;
		case R.id.menu_about:
			//doDocumentKeys();
			return true;
		}
		return false;
	}

	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		Log.d(TAG, "onActivityResult " + resultCode);
		switch (requestCode) {
		case REQUEST_CONNECT_DEVICE:
			// When DeviceListActivity returns with a device to connect
			if (resultCode == Activity.RESULT_OK) {
				// Get the device MAC address
				String address = data.getExtras().getString(DeviceListActivity.EXTRA_DEVICE_ADDRESS);
				// Get the BLuetoothDevice object
				BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(address);
				// Attempt to connect to the device
				Log.e(TAG, "Device selected, connecting ...");
				mSerialService.connect(device);
			}
			break;

		case REQUEST_ENABLE_BT:
			// When the request to enable Bluetooth returns
			if (resultCode == Activity.RESULT_OK) {
				//Log.d(LOG_TAG, "BT not enabled");
				//finishDialogNoBluetooth();      
				setupBT();
			} else {
				// User did not enable Bluetooth or an error occurred
				Log.d(TAG, "BT not enabled");
				Toast.makeText(this, R.string.bt_not_enabled_leaving, Toast.LENGTH_SHORT).show();
				finish();
			}
		}
	}
	
	// The Handler that gets information back from the BluetoothService
		private final Handler mHandlerBT = new Handler() {
			@Override
			public void handleMessage(Message msg) {        	
				switch (msg.what) {

				// state switch
				case MESSAGE_STATE_CHANGE:
					if(mLog!=null){
//						mConnect.checkStatus();
					};
					Log.i(TAG, "MESSAGE_STATE_CHANGE: " + msg.arg1);
													
					switch (msg.arg1) {
					case BluetoothSerialService.STATE_CONNECTED:
						if (mMenuItemConnect != null) {
							//mMenuItemConnect.setIcon(android.R.drawable.ic_menu_close_clear_cancel);
							mMenuItemConnect.setTitle(R.string.disconnect);
						}
						if(mLog!=null){	setLog("Connected");	};
						break;

					case BluetoothSerialService.STATE_CONNECTING:
						if (mMenuItemConnect != null) {
							//mMenuItemConnect.setIcon(android.R.drawable.ic_menu_close_clear_cancel);
							mMenuItemConnect.setTitle(R.string.disconnect);
						}
						if(mLog!=null){	setLog("Connected");	};
						Toast.makeText(getApplicationContext(), "Connecting ...", Toast.LENGTH_SHORT).show();
						break;

					case BluetoothSerialService.STATE_NONE:
						if(mLog!=null){	setLog("Connected");	};
						break;
					}
					break;

					// display popup
				case MESSAGE_TOAST: //?
					Toast.makeText(getApplicationContext(), msg.getData().getString(TOAST),
							Toast.LENGTH_SHORT).show();
					break;

					// show device popup
				case MESSAGE_DEVICE_NAME:
					// save the connected device's name
					//mConnectedDeviceName = msg.getData().getString(mConnect.DEVICE_NAME);
					//Toast.makeText(getApplicationContext(), "Connected to " + mConnectedDeviceName, Toast.LENGTH_SHORT).show();
					Toast.makeText(getApplicationContext(), "Connected to whatever", Toast.LENGTH_SHORT).show();
					break;

				case MESSAGE_READ:
					byte[] readBuf = (byte[]) msg.obj;
					// construct a string from the valid bytes in the buffer
					String readMessage = new String(readBuf, 0, msg.arg1);
					//Toast.makeText(getApplicationContext(), readMessage, Toast.LENGTH_SHORT).show();
					if(mLog!=null){	appendLog(readMessage);	};
					break;

					//	here state machine
					//			case MESSAGE_WRITE:
					//                byte[] writeBuf = (byte[]) msg.obj;
					//                // construct a string from the buffer
					//                String writeMessage = new String(writeBuf);
					//                mConversationArrayAdapter.add("Me:  " + writeMessage);
					//                break;
				}
			}
		};
}
