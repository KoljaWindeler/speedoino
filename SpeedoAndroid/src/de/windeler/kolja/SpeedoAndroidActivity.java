package de.windeler.kolja;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.TreeMap;
import android.app.Activity;
import android.app.ProgressDialog;
import android.app.TabActivity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.Button;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.TabHost;
import android.widget.TextView;
import android.widget.Toast;

public class SpeedoAndroidActivity extends TabActivity implements
OnClickListener {
	// Name of the connected device
	private static final String TAG = "JKW";
	private MenuItem mMenuItemConnect;
	public BluetoothAdapter mBluetoothAdapter = null;
	private String firmware_flash_filename=null;
	private String firmware_flash_bluetooth_device=null;
	public static final String DEVICE_NAME = "device_name";
	public static final String TOAST = "toast";
	public static final String result = "result";
	public static final String BYTE_TRANSFERED = "byte_trans";
	public static BluetoothSerialService mSerialService = null;
	private Toast toast;
	private Handler mTimerHandle = new Handler();
	private getDirDialog _getDirDialog;
	private getFileDialog _getFileDialog;
	private putFileDialog _putFileDialog;
	private delFileDialog _delFileDialog;
	private firmwareBurnDialog _firmwareBurnDialog;
	private String dl_basedir = "/";
	private String t2a_dest = "";
	private long t2a_size = 0;
	private int back_pushed = 0;
	private int statusLastCommand = 0;

	/**
	 * Our main view. Displays the emulated terminal screen.
	 */
	private TextView mLog;
	private TextView mStatus;
	private TextView mVersion;
	private TextView mDownload;
	private TextView mUpload;
	private Button mLeftButton;
	private Button mRightButton;
	private Button mUpButton;
	private Button mDownButton;
	private Button browseToUploadMap;
	private Button browseToUploadConfig;
	private Button browseToUploadGfx;
	private Button browseToUploadFirmware;
	private Button browseToRouteMap;
	private Button mloadRoot;
	private Button DlselButton;
	private Button DeleteButton;
	private ListView mDLListView;

	// Message types sent from the BluetoothReadService Handler
	public static final int MESSAGE_STATE_CHANGE = 1;
	public static final int MESSAGE_READ = 2;
	public static final int MESSAGE_WRITE = 3;
	public static final int MESSAGE_DEVICE_NAME = 4;
	public static final int MESSAGE_TOAST = 5;
	public static final int MESSAGE_CMD_OK = 6;
	public static final int MESSAGE_CMD_FAILED = 7;
	public static final int MESSAGE_CMD_UNKNOWN = 8;
	public static final int MESSAGE_SET_VERSION = 9;
	public static final int MESSAGE_SET_LOG = 10;
	public static final int MESSAGE_DIR_APPEND = 11;
	public static final int MESSAGE_CREATE_CONNECTION = 12;

	private static final int REQUEST_CONNECT_DEVICE = 1;
	private static final int REQUEST_ENABLE_BT = 2;
	private static final int REQUEST_OPEN_MAP = 3; // file open dialog
	private static final int REQUEST_OPEN_CONFIG = 4; // file open dialog
	private static final int REQUEST_OPEN_GFX = 6; // file open dialog
	private static final int REQUEST_CONVERT_GFX = 7; // convert image
	private static final int REQUEST_CONVERT_MAP = 8; // convert maps
	private static final int REQUEST_SHOW_MAP = 9; // open maps
	private static final int REQUEST_SHOW_MAP_DONE = 10; // open maps
	private static final int REQUEST_UPLOAD_FIRMWARE = 11; // firmware
	private static final int REQUEST_SELECTED_DEVICE = 12; // get selected bluetooth device


	// transfer messages
	public static final byte CMD_SIGN_ON				=  0x01;
	public static final byte CMD_LEAVE_FM				=  0x04;
	public static final byte CMD_LOAD_ADDRESS			=  0x06;
	public static final byte CMD_LEAVE_PROGMODE_ISP	=  0x11;
	public static final byte CMD_CHIP_ERASE_ISP		=  0x12;
	public static final byte CMD_PROGRAM_FLASH_ISP	=  0x13;
	public static final byte CMD_SPI_MULTI			=  0x1D;
	public static final byte CMD_GO_LEFT				=  0x25;
	public static final byte CMD_GO_RIGHT				=  0x26;
	public static final byte CMD_GO_UP				=  0x27;
	public static final byte CMD_GO_DOWN				=  0x28;
	public static final byte CMD_DIR					=  0x31;
	public static final byte CMD_GET_FILE				=  0x32;
	public static final byte CMD_PUT_FILE				=  0x33;
	public static final byte CMD_DEL_FILE				=  0x34;
	public static final byte CMD_SHOW_GFX				=  0x35;
	public static final byte CMD_FILE_RECEIVE			=  0x39;
	public static final byte CMD_RESET_SMALL_AVR		=  0x40;


	public static final char STATUS_CMD_OK      =  0x00;
	public static final char STATUS_CMD_FAILED  =  0xC0;
	public static final char STATUS_CKSUM_ERROR =  0xC1;
	public static final char STATUS_CMD_UNKNOWN =  0xC9;
	public static final char STATUS_EOF		   =  0x10;

	// dir me
	private String dir_path = "";
	private boolean dir_completed = true;
	private TreeMap<String, String> dirsMap = new TreeMap<String, String>();
	private TreeMap<String, String> filesMap = new TreeMap<String, String>();
	private TreeMap<String, Integer> typeMap = new TreeMap<String, Integer>();
	private TreeMap<String, Long> sizeMap = new TreeMap<String, Long>();
	private ArrayList<HashMap<String, Object>> mList = new ArrayList<HashMap<String, Object>>();
	private static final String ITEM_KEY = "key";
	private static final String ITEM_KEY_LOW = "key_low";
	private static final String ITEM_IMAGE = "image";
	// String buffer for outgoing messages
	public TextView mTest;

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);

		Resources res = getResources(); // Resource object to get Drawables
		TabHost tabHost = getTabHost(); // The activity TabHost
		TabHost.TabSpec spec; // Resusable TabSpec for each tab
		// Intent intent; // Reusable Intent for each tab

		// add orders tab
		spec = tabHost
				.newTabSpec("orders")
				.setIndicator("Connect",
						res.getDrawable(R.drawable.ic_tab_connect))
						.setContent(R.id.connectLayout);
		tabHost.addTab(spec);

		// add positions tab
		spec = tabHost
				.newTabSpec("positions")
				.setIndicator("Upload",
						res.getDrawable(R.drawable.ic_tab_upload))
						.setContent(R.id.uploadLayout);
		tabHost.addTab(spec);

		// add strategies tab
		spec = tabHost
				.newTabSpec("strategies")
				.setIndicator("Download",
						res.getDrawable(R.drawable.ic_tab_download))
						.setContent(R.id.downloadLayout);
		tabHost.addTab(spec);
		// layout ende

		// buttons
		mLog = (TextView) findViewById(R.id.log_value);
		mStatus = (TextView) findViewById(R.id.status_value);
		mVersion = (TextView) findViewById(R.id.version_value);
		mVersion.setOnClickListener(this);
		mDownload = (TextView) findViewById(R.id.Download_textView);
		mLeftButton = (Button) findViewById(R.id.button_left);
		mLeftButton.setOnClickListener(this);
		mRightButton = (Button) findViewById(R.id.button_right);
		mRightButton.setOnClickListener(this);
		mUpButton = (Button) findViewById(R.id.button_up);
		mUpButton.setOnClickListener(this);
		mDownButton = (Button) findViewById(R.id.button_down);
		mDownButton.setOnClickListener(this);
		browseToUploadMap = (Button) findViewById(R.id.browseToUploadMap);
		browseToUploadMap.setOnClickListener(this);
		browseToUploadGfx = (Button) findViewById(R.id.browseToUploadGfx);
		browseToUploadGfx.setOnClickListener(this);
		browseToRouteMap = (Button) findViewById(R.id.browseToRouteMap);
		browseToRouteMap.setOnClickListener(this);
		browseToUploadConfig = (Button) findViewById(R.id.browseToUploadConfig);
		browseToUploadConfig.setOnClickListener(this);
		browseToUploadGfx = (Button) findViewById(R.id.browseToUploadFirmware);
		browseToUploadGfx.setOnClickListener(this);
		mloadRoot = (Button) findViewById(R.id.loadRoot);
		mloadRoot.setOnClickListener(this);
		DlselButton = (Button) findViewById(R.id.DownloadButtonSelect);
		DlselButton.setOnClickListener(this);
		DeleteButton = (Button) findViewById(R.id.DeleteButton);
		DeleteButton.setOnClickListener(this);
		mDLListView = (ListView) findViewById(R.id.dlList);
		
		update_visible_elements(false);

		if (mLog != null)
			mLog.setText(R.string.bindestrich);
		if (mVersion != null)
			mVersion.setText(R.string.bindestrich);

		// create dir if not existent
		File sdCard = Environment.getExternalStorageDirectory(); //
		File dir = new File(sdCard.getAbsolutePath() + "/Speedoino"); // /mnt/sdcard/Download/
		if (!dir.exists()) {
			if (dir.mkdir()) {
				dl_basedir = dir.getAbsolutePath() + "/";
			} else {
				dl_basedir = sdCard.getAbsolutePath() + "/";
				toast = Toast.makeText(this,
						"Can't create directory on SD card", Toast.LENGTH_LONG);
				toast.show();
			}
		} else {
			dl_basedir = dir.getAbsolutePath() + "/";
		}

		mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
		// If the adapter is null, then Bluetooth is not supported
		if (mBluetoothAdapter == null) {
		toast = Toast.makeText(this, "Bluetooth is not available",
		Toast.LENGTH_LONG);
		toast.show();
		// finish();
		return;
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
			Intent enableIntent = new Intent(
			BluetoothAdapter.ACTION_REQUEST_ENABLE);
			startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
			// Otherwise, setup the chat session
			} else {
			if (mSerialService == null)
			setupBT();
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
		// Stop the Bluetooth chat services
		if (mSerialService != null)
			mSerialService.stop();
		Log.e(TAG, "--- ON DESTROY ---");

	}
	//////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////// Bluetooth startup & shutdown /////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////

	
	//////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////// men� & buttons steuerung ///////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.menu, menu);
		mMenuItemConnect = menu.getItem(0);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
		case R.id.menu_connect:
			if (mSerialService.getState() == BluetoothSerialService.STATE_NONE) {
				// Launch the DeviceListActivity to see devices and do scan
				Intent serverIntent = new Intent(this, DeviceListActivity.class);
				startActivityForResult(serverIntent, REQUEST_CONNECT_DEVICE);
			} else {
				mSerialService.stop();
				mSerialService.start();
			}
			return true;
		case R.id.menu_settings:
			// doPreferences();
			return true;
		case R.id.menu_about:
			// doDocumentKeys();
			return true;
		}
		return false;
	}
	
	@Override
	public void onClick(View arg0) {
		Intent intent; // reusable
		switch (arg0.getId()) {
		case R.id.version_value:
			mCheckVer.run();
			break;
		case R.id.button_left:
			byte send2[] = new byte[1];
			send2[0] = CMD_GO_LEFT;
			try {
				Log.i("SEND","goleft()");
				mSerialService.send(send2, 1);
			} catch (InterruptedException e1) {
				e1.printStackTrace();
			}
			break;
		case R.id.button_up:
			byte send3[] = new byte[1];
			send3[0] = CMD_GO_UP;
			try {
				Log.i("SEND","goup()");
				mSerialService.send(send3, 1);
			} catch (InterruptedException e1) {
				e1.printStackTrace();
			}
			break;
		case R.id.button_right:
			byte send4[] = new byte[1];
			send4[0] = CMD_GO_RIGHT;
			try {
				Log.i("SEND","goright()");
				mSerialService.send(send4, 1);
			} catch (InterruptedException e1) {
				e1.printStackTrace();
			}
			break;
		case R.id.button_down:
			byte send5[] = new byte[1];
			send5[0] = CMD_GO_DOWN;
			try {
				Log.i("SEND","godown()");
				mSerialService.send(send5, 1);
			} catch (InterruptedException e1) {
				e1.printStackTrace();
			}
			break;
		case R.id.browseToUploadMap:
			intent = new Intent(getBaseContext(), FileDialog.class);
			intent.putExtra(FileDialog.START_PATH, dl_basedir);
			intent.putExtra(FileDialog.SELECTION_MODE, SelectionMode.MODE_OPEN);
			startActivityForResult(intent, REQUEST_OPEN_MAP);
			break;
		case R.id.browseToUploadConfig:
			intent = new Intent(getBaseContext(), FileDialog.class);
			intent.putExtra(FileDialog.START_PATH, dl_basedir);
			intent.putExtra(FileDialog.SELECTION_MODE, SelectionMode.MODE_OPEN);
			startActivityForResult(intent, REQUEST_OPEN_CONFIG);
			break;
		case R.id.browseToUploadGfx:
			intent = new Intent(getBaseContext(), FileDialog.class);
			intent.putExtra(FileDialog.START_PATH, dl_basedir);
			intent.putExtra(FileDialog.SELECTION_MODE, SelectionMode.MODE_OPEN);
			startActivityForResult(intent, REQUEST_OPEN_GFX);
			break;
		case R.id.loadRoot:
			_getDirDialog = new getDirDialog(this);
			_getDirDialog.execute("/");
			break;
		case R.id.DownloadButtonSelect:
			Log.i(TAG, "download gedrueckt");
			_getFileDialog = new getFileDialog(this);
			_getFileDialog.execute(t2a_dest, dl_basedir, String.valueOf(t2a_size));
			break;
		case R.id.DeleteButton:
			Log.i(TAG, " delete gedrueckt!");
			_delFileDialog = new delFileDialog(this);
			_delFileDialog.execute(t2a_dest);
			break;
		case R.id.browseToRouteMap:
			intent = new Intent(getBaseContext(), FileDialog.class);
			intent.putExtra(FileDialog.START_PATH, dl_basedir);
			intent.putExtra(FileDialog.SELECTION_MODE, SelectionMode.MODE_OPEN);
			startActivityForResult(intent, REQUEST_SHOW_MAP);
			break;
		case R.id.browseToUploadFirmware:
			firmware_update(1,null,null);
			break;
		default:
			Log.i(TAG, "Hier wurde was geklickt das ich nicht kenne!!");
			break;
		}
	}
	
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if ((keyCode == KeyEvent.KEYCODE_BACK)) {
			if (back_pushed == 1) {
				mSerialService.stop();
				mSerialService.start();
				update_visible_elements(false);
				finish();
			} else {
				toast = Toast.makeText(this,
						this.getString(R.string.push_back_twice),
						Toast.LENGTH_LONG);
				toast.show();
				back_pushed = 1;
				// install guard, 2sec until check of receive
				mTimerHandle.removeCallbacks(mCheckDoublePushBack);
				mTimerHandle.postDelayed(mCheckDoublePushBack, 2000);
			}
		} else if (keyCode == KeyEvent.KEYCODE_HOME) {
			mSerialService.stop();
			mSerialService.start();
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
	//////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////// men� & buttons steuerung ///////////////////////////////////////
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
			
		case REQUEST_SELECTED_DEVICE:
			if (resultCode == Activity.RESULT_OK) {
				// Get the device MAC address
				String address = data.getExtras().getString(
						DeviceListActivity.EXTRA_DEVICE_ADDRESS);
				// tell firmware update which device has been selected
				firmware_update(0,null,address);
			}
			break;

		case REQUEST_ENABLE_BT:
			// When the request to enable Bluetooth returns
			if (resultCode == Activity.RESULT_OK) {
				// Log.d(LOG_TAG, "BT not enabled");
				// finishDialogNoBluetooth();
				setupBT();
			} else {
				// User did not enable Bluetooth or an error occurred
				Log.d(TAG, "BT not enabled");
				toast = Toast.makeText(this, R.string.bt_not_enabled_leaving,
						Toast.LENGTH_SHORT);
				toast.show();
				finish();
			}
			break;
		case REQUEST_OPEN_MAP:
			filePath = data.getStringExtra(FileDialog.RESULT_PATH);
			Log.i(TAG, "File open gab diesen Dateinamen aus:" + filePath);
			if (resultCode == RESULT_OK) {
				intent = new Intent(getBaseContext(), MapEditor.class);
				intent.putExtra(MapEditor.INPUT_FILE_NAME, filePath);
				intent.putExtra(MapEditor.INPUT_DIR_PATH, dl_basedir);
				startActivityForResult(intent, REQUEST_CONVERT_MAP);
			}
			;
			// process_uploadFile(REQUEST_OPEN_MAP,filePath)
			break;
		case REQUEST_CONVERT_MAP:
			Log.i(TAG, "Map converter hat was zurueckgegeben ");
			if (resultCode == RESULT_OK) {
				filePath = data.getStringExtra(ImageEditor.OUTPUT_FILE_PATH);
				Log.i(TAG, "Der Resultcode war OK, der Pfad:" + filePath);
				_putFileDialog = new putFileDialog(this);
				_putFileDialog.execute(filePath,
						"NAVI" + filePath.substring(filePath.lastIndexOf('/'))); // /mnt/sdcard/Download/bild.sng,
				// GFX/bild.sng
			} else {
				toast = Toast.makeText(this, "Dialog cancled",
						Toast.LENGTH_SHORT);
				toast.show();
			}
			break;
		case REQUEST_OPEN_CONFIG:
			if (resultCode == RESULT_OK) {
				filePath = data.getStringExtra(FileDialog.RESULT_PATH);
				Log.i(TAG, "File open gab diesen Dateinamen aus:" + filePath);
				_putFileDialog = new putFileDialog(this);
				_putFileDialog.execute(filePath,
						"CONFIG" + filePath.substring(filePath.lastIndexOf('/'))); // /mnt/sdcard/Download/bild.sng,
				// GFX/bild.sng
				Log.i(TAG, "Datei wurde hochgeladen");
				// delete the file
				// File file = new File(filePath);
				// file.delete();
			}
			;
			break;
		case REQUEST_OPEN_GFX:
			// gfx datei ausgewaehlt jetzt damit den converter starten
			filePath = data.getStringExtra(FileDialog.RESULT_PATH);
			Log.i(TAG, "File open gab diesen GFX Dateinamen aus:" + filePath);
			if (resultCode == RESULT_OK) {
				intent = new Intent(getBaseContext(), ImageEditor.class);
				intent.putExtra(ImageEditor.INPUT_FILE_NAME, filePath);
				intent.putExtra(ImageEditor.INPUT_DIR_PATH, dl_basedir);
				startActivityForResult(intent, REQUEST_CONVERT_GFX);
			}
			;
			break;
		case REQUEST_CONVERT_GFX:
			Log.i(TAG, "Image converter hat was zurueckgegeben ");
			if (resultCode == RESULT_OK) {
				filePath = data.getStringExtra(ImageEditor.OUTPUT_FILE_PATH);
				Log.i(TAG, "Der Resultcode war OK, der Pfad:" + filePath);
				_putFileDialog = new putFileDialog(this);
				_putFileDialog.execute(filePath,
						"GFX" + filePath.substring(filePath.lastIndexOf('/'))); // /mnt/sdcard/Download/bild.sng,
				// GFX/bild.sng

				Log.i(TAG, "Datei wurde hochgeladen");
				// delete the file
				File file2 = new File(filePath);
				// file2.delete();

			} else {
				toast = Toast.makeText(this, "Dialog cancled",
						Toast.LENGTH_SHORT);
				toast.show();
			}
			break;
		case REQUEST_SHOW_MAP:
			Log.i(TAG, "Image converter hat was zurueckgegeben ");
			if (resultCode == RESULT_OK) {
				filePath = data.getStringExtra(FileDialog.RESULT_PATH);
				Log.i(TAG, "Der Resultcode war OK, der Pfad:" + filePath);
				intent = new Intent(getBaseContext(), RouteMap.class);
				intent.putExtra(RouteMap.INPUT_FILE_NAME, filePath);
				startActivityForResult(intent, REQUEST_SHOW_MAP_DONE);
			};
			break;
		case REQUEST_UPLOAD_FIRMWARE:
			// hier jetzt die firmware laden, und dann �bertragen... huijuijui
			if (resultCode == RESULT_OK) {
				filePath = data.getStringExtra(FileDialog.RESULT_PATH);
				Log.i(TAG, "Der Resultcode war OK, der Pfad:" + filePath);
				firmware_update(0,filePath, null); // mit dateinamen starten
			};
			break;
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
	

	//////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////// BLUETOOTH connection handling /////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
	private int getStatusLastCommand() {
		return statusLastCommand;
	}

	private void setStatusLastCommand(int status) {
		statusLastCommand = status;
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////// BLUETOOTH connection handling /////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
	
	
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////// BLUETOOTH classes for function in background ////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
	// klasse die das loading fenster startet und im hintergrund "dir" ausfuehrt
	protected class getDirDialog extends AsyncTask<String, Integer, String> {
		private Context context;
		ProgressDialog dialog;

		public getDirDialog(Context cxt) {
			context = cxt;
			dialog = new ProgressDialog(context);
		}

		@Override
		protected String doInBackground(String... params) {
			try {
				mSerialService.getDir(params[0], mHandlerUpdate);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			return "japp";
		}

		@Override
		protected void onPreExecute() {
			dialog.setMessage("Loading Directory...");
			dialog.show();
		};

		@Override
		protected void onPostExecute(String result) {
			dialog.dismiss();
		}

		private final Handler mHandlerUpdate = new Handler() {
			@Override
			public void handleMessage(Message msg) {
				Log.i(TAG, "handle:" + msg.getData().getString(BYTE_TRANSFERED));
				dialog.setMessage(msg.getData().getString(BYTE_TRANSFERED));
				Log.i(TAG, "update prozenzzahl");
			};
		};
	}

	// klasse die das loading fenster startet und im hintergrund "download"
	// ausfuehrt
	protected class getFileDialog extends AsyncTask<String, Integer, String> {
		private Context context;
		ProgressDialog dialog;

		public getFileDialog(Context cxt) {
			context = cxt;
			dialog = new ProgressDialog(context);
		}

		protected void onProgressUpdate(String... progress) {
			dialog.setProgress(Integer.parseInt(progress[0]));
		}

		@Override
		protected String doInBackground(String... params) {
			try {
				mSerialService.getFile(params[0], params[1], mHandlerUpdate, params[2]);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			;
			return "japp";
		}

		protected void onProgressUpdate(Integer... progress) {
			dialog.setProgress(8);
		}

		@Override
		protected void onPreExecute() {
			dialog.setMessage("Downloading file...");
			dialog.show();
		};

		@Override
		protected void onPostExecute(String result) {
			dialog.dismiss();
		}

		private final Handler mHandlerUpdate = new Handler() {
			@Override
			public void handleMessage(Message msg) {
				dialog.setMessage(msg.getData().getString(BYTE_TRANSFERED));
				Log.i(TAG, "update prozenzzahl");
			};
		};
	}

	// klasse die das loading fenster startet und im hintergrund "download"
	// ausfuehrt
	protected class putFileDialog extends AsyncTask<String, Integer, String> {
		private Context context;
		ProgressDialog dialog;

		public putFileDialog(Context cxt) {
			context = cxt;
			dialog = new ProgressDialog(context);
		}

		@Override
		protected String doInBackground(String... params) {
			Log.i("JKW","starte put file");
			try {
				mSerialService.delFile(params[1]);
				Log.i("JKW","delete file passed");
				mSerialService.putFile(params[0], params[1], mHandlerUpdate);
				Log.i("JKW","put file passed");
				if (params[1].substring(0, 3).contentEquals("GFX")) {
					mSerialService.showgfx(params[1].substring(params[1]
							.lastIndexOf('/') + 1));
				}
			} catch (InterruptedException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			}
			;
			return "japp";
		}

		@Override
		protected void onPreExecute() {
			dialog.setMessage("Uploading file...");
			dialog.show();
		};

		@Override
		protected void onPostExecute(String result) {
			dialog.dismiss();
		}

		private final Handler mHandlerUpdate = new Handler() {
			@Override
			public void handleMessage(Message msg) {
				dialog.setMessage(msg.getData().getString(BYTE_TRANSFERED));
				Log.i(TAG, "update prozenzzahl");
			};
		};

	}

	// klasse die das updaten der firmware machen soll ausfuehrt
	protected class firmwareBurnDialog extends AsyncTask<String, Integer, String> {
		private Context context;
		ProgressDialog dialog;

		public firmwareBurnDialog(Context cxt) {
			context = cxt;
			dialog = new ProgressDialog(context);
		}

		@Override
		protected String doInBackground(String... params) {
			Log.i("JKW","starte firmwareBurnDialog");
			try {
				// params[0] ist der filename
				// params[1] ist das Bluetooth device
				int return_value=mSerialService.uploadFirmware(params[0],mHandlerUpdate,params[1]);
				if(return_value>0){
					Log.i(TAG, "Warum kam denn uploadFirmware mit status "+String.valueOf(return_value)+" zurueck");
				}
			} catch (InterruptedException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			}
			;
			return "japp";
		}

		@Override
		protected void onPreExecute() {
			dialog.setMessage("Flashing Firmware ...");
			dialog.show();
		};

		@Override
		protected void onPostExecute(String result) {
			dialog.dismiss();
		}

		private final Handler mHandlerUpdate = new Handler() {
			@Override
			public void handleMessage(Message msg) {
				if(msg.what==MESSAGE_SET_VERSION){
					dialog.setMessage(msg.getData().getString(BYTE_TRANSFERED));
					//Log.i(TAG, "update prozenzzahl");
				}
			};
		};
	}

	protected class showGFX extends AsyncTask<String, Integer, String> {
		@Override
		protected String doInBackground(String... params) {
			try {
				mSerialService.showgfx(params[0]);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			;
			return "japp";
		}
	};

	// klasse die das loading fenster startet und im hintergrund "download"
	// ausfuehrt
	protected class delFileDialog extends AsyncTask<String, Integer, String> {
		private Context context;
		ProgressDialog dialog;

		public delFileDialog(Context cxt) {
			context = cxt;
			dialog = new ProgressDialog(context);
		}

		@Override
		protected String doInBackground(String... params) {
			try {
				mSerialService.delFile(params[0]);
				mSerialService.reset_seq();
				mSerialService.getDir(
						params[0].substring(0, params[0].lastIndexOf('/')),
						mHandlerUpdate);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			;
			return "japp";
		}

		@Override
		protected void onPreExecute() {
			dialog.setMessage("Deleting file...");
			dialog.show();
		};

		@Override
		protected void onPostExecute(String result) {
			dialog.dismiss();
		}

		private final Handler mHandlerUpdate = new Handler() {
			@Override
			public void handleMessage(Message msg) {
				dialog.setMessage(msg.getData().getString(BYTE_TRANSFERED));
				Log.i(TAG, "update prozenzzahl");
			};
		};
	}	
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////// BLUETOOTH classes for function in background ////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////

	// The Handler that gets information back from the BluetoothService
	private final Handler mHandlerBT = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {

			// state switch
			case MESSAGE_STATE_CHANGE:
				Log.i(TAG, "MESSAGE_STATE_CHANGE: " + msg.arg1);

				switch (msg.arg1) {
				case BluetoothSerialService.STATE_CONNECTED:
					if (mStatus != null) {
						mStatus.setText("Connected,Speedoino found");
					};

					update_visible_elements(true);

					toast = Toast.makeText(getApplicationContext(),"Connected, Speedoino found", Toast.LENGTH_SHORT);
					toast.show();

					//mTimerHandle.postDelayed(mCheckVer, 500);
					break;

				case BluetoothSerialService.STATE_CONNECTING:
					if (mMenuItemConnect != null) {
						// mMenuItemConnect.setIcon(android.R.drawable.ic_menu_close_clear_cancel);
						mMenuItemConnect.setTitle(R.string.disconnect);
					}
					if (mStatus != null) {
						mStatus.setText("Connecting...");
					};
					toast = Toast.makeText(getApplicationContext(),
							"Connecting ...", Toast.LENGTH_SHORT);
					toast.show();
					break;

				case BluetoothSerialService.STATE_NONE:
					if (mStatus != null) {
						mStatus.setText(R.string.not_connected);
					};
					if (mVersion != null) {
						mVersion.setText(R.string.bindestrich);
					};
					if (mLog != null) {
						mLog.setText(R.string.bindestrich);
					};

					update_visible_elements(false);

					TextView mselfile = (TextView) findViewById(R.id.dl_selected_file);
					mselfile.setText(R.string.no_selected_file);

					toast = Toast.makeText(getApplicationContext(),
							"Connection closed...", Toast.LENGTH_SHORT);
					toast.show();
					break;

				case BluetoothSerialService.STATE_CONNECTED_AND_SEARCHING:
					if (mStatus != null) {
						mStatus.setText("Connected, searching...");
					};
					break;
				}
				break;

				// display popup
			case MESSAGE_TOAST: // ?
				toast = Toast.makeText(getApplicationContext(), msg.getData()
						.getString(TOAST), Toast.LENGTH_SHORT);
				setStatusLastCommand(msg.getData().getInt(result));
				toast.show();
				break;

			case MESSAGE_SET_LOG:
				mLog.setText(msg.getData().getString(TOAST));
				break;

			case MESSAGE_SET_VERSION:
				mVersion.setText(msg.getData().getString(TOAST));
				break;

			case MESSAGE_DIR_APPEND:
				if (dir_completed == true) {
					dir_completed = false;
					mDownload.setText("");
					filesMap.clear();
					typeMap.clear();
					sizeMap.clear();
					dirsMap.clear();
					mList.clear();
				};
				String name = msg.getData().getString("name");
				int type = msg.getData().getInt("type");
				long size = msg.getData().getLong("size");

				Log.i(TAG,
						"CMD:" + name + " item nr:"
								+ String.valueOf(mSerialService.item));

				if (type == 1) { // file
					filesMap.put(name, name);
					typeMap.put(name, 1);
					sizeMap.put(name, size);
				} else if (type == 2) { // dir
					dirsMap.put(name, name);
					typeMap.put(name, 2);
					sizeMap.put(name, size);
				};
				if (type == STATUS_EOF) {
					dir_completed = true;
					Log.d(TAG, "beginne liste aufzubauen");

					// send to display												
					SimpleAdapter fileList = new SimpleAdapter(getApplicationContext(), mList,
							R.layout.file_dialog_row,
							new String[] { ITEM_KEY, ITEM_IMAGE, ITEM_KEY_LOW }, new int[] {
						R.id.fdrowtext, R.id.fdrowimage, R.id.fdrowtext_lower });

					if (dir_path != "/") {
						addItem("/", R.drawable.folder,-1);
						typeMap.put("/", 2);
					};

					for (String dir : dirsMap.tailMap("").values()) {
						if (dir.toString().length() > 23) {
							addItem(dir.toString().substring(0, 20) + "...",R.drawable.folder,-1);
						} else {
							addItem(dir, R.drawable.folder,-1);
						}
					}

					for (String file : filesMap.tailMap("").values()) {
						long file_size=0;
						file_size = sizeMap.get(file);
						if (file.toString().length() > 23) {
							addItem(file.toString().substring(0, 20) + "...",R.drawable.file,file_size);
						} else {
							addItem(file, R.drawable.file,file_size);
						}
					}
					mDLListView.setAdapter(fileList);

					mDLListView
					.setOnItemClickListener(new OnItemClickListener() {
						public void onItemClick(AdapterView<?> arg0,
								View arg1, int arg2, long arg3) {
							String name = null;
							Integer type = 0;
							Long size = (long) 0;
							HashMap<String, Object> item = new HashMap<String, Object>();

							item = mList.get(arg2);
							name = (String) item.get(ITEM_KEY);
							type = typeMap.get(name);
							size = sizeMap.get(name);

							if (type == 1) {
								t2a_dest = "";
								if (dir_path != "/") // z.B. CONFIG
									t2a_dest = dir_path + "/"; // CONFIG/
								t2a_dest = t2a_dest + name; // CONFIG/BASE.TXT
								t2a_size = size;

								TextView mselfile = (TextView) findViewById(R.id.dl_selected_file);
								mselfile.setText("Selected file: "
										+ t2a_dest);

								DlselButton.setEnabled(true);
								DeleteButton.setEnabled(true);
							} else if (type == 2) {
								dir_path = name;
								_getDirDialog = new getDirDialog(arg0
										.getContext());
								_getDirDialog.execute(name);
							}
						} // public void onItemClick(A
					}); // setOnItemClickListener(...
				} // if(type==STATUS_EOF){
				break;

			case MESSAGE_CMD_UNKNOWN:
				mLog.setText(R.string.unknown);
				toast = Toast.makeText(getApplicationContext(),
						R.string.unknown, Toast.LENGTH_SHORT);
				toast.show();
				break;

			case MESSAGE_CMD_FAILED:
				mLog.setText(R.string.noresponse);
				toast = Toast.makeText(getApplicationContext(),
						R.string.noresponse, Toast.LENGTH_SHORT);
				toast.show();
				break;

				// show device popup
			case MESSAGE_DEVICE_NAME:
				// save the connected device's name
				toast = Toast.makeText(getApplicationContext(),
						"Connected, searching Speedoino", Toast.LENGTH_SHORT);
				toast.show();
				break;

			}
		}
	};

	
	
	
	/* prozedure zum updaten der firmware
	* wir ben�tigen zum updaten zwei dinge: den dateinamen des hex files und 
	* den bluetooth string des devices, die funktion firmware_update wird initial
	* �ber den button gestartet, ruft ihrerseits dann mindestens den intent f�r die
	* datei auswahl auf (und, falls keine Verbindung besteht, den BT auswahl dialog)
	* und staretet dann den flashvorgang
	* die funktion endet mit dem aufruf der anderen intents, wird aber von ihnen wieder
	* augerufen, tricky
	*/
	private void firmware_update(int init,String input_file, String bluetooth_adr){
		// first call, set vars
		if(init==1){
			firmware_flash_filename=null;
			if(mSerialService.last_connected_device!=null){
				firmware_flash_bluetooth_device=mSerialService.last_connected_device.getAddress();
			} else {
				firmware_flash_bluetooth_device=null;
			}
		}
		
		//mal sehen ob wir aufruf parameter haben
		if(input_file!=null){ firmware_flash_filename=input_file; };
		if(bluetooth_adr!=null){ firmware_flash_bluetooth_device=bluetooth_adr; };
		
		// wenn wir hingegen noch nicht alle aufruf parameter haben, entsprechende activity starten
		
		// get filename
		if(firmware_flash_filename==null){
			Intent intent = new Intent(getBaseContext(), FileDialog.class);
			intent.putExtra(FileDialog.START_PATH, dl_basedir);
			intent.putExtra(FileDialog.SELECTION_MODE, SelectionMode.MODE_OPEN);
			startActivityForResult(intent, REQUEST_UPLOAD_FIRMWARE);
			return; // ende
		}
		
		// get device
		if(firmware_flash_bluetooth_device==null){
			Intent serverIntent = new Intent(this, DeviceListActivity.class);
			startActivityForResult(serverIntent, REQUEST_SELECTED_DEVICE);
			return; // ende
		}
		
		// sobald wir beides haben: start
		_firmwareBurnDialog = new firmwareBurnDialog(this);
		_firmwareBurnDialog.execute(firmware_flash_filename,firmware_flash_bluetooth_device); // /mnt/sdcard/Download/bild.sng,
	};


	private void addItem(String fileName, int imageId, long size) {
		HashMap<String, Object> item = new HashMap<String, Object>();
		item.put(ITEM_KEY, fileName);
		if(size>=100*1024*1024){ // ab 100mb
			item.put(ITEM_KEY_LOW, String.valueOf(size/1024/1024)+" MB");
		} else if(size>=10*1024*1024){ // ab 10mb
			item.put(ITEM_KEY_LOW, String.valueOf(size/1024/1024)+"."+String.valueOf((size/1024/102)%10)+" MB");
		} else if(size>=1024*1024){ // ab 1mb
			item.put(ITEM_KEY_LOW, String.valueOf(size/1024/1024)+"."+String.valueOf((size/1024/10)%100)+" MB");
		} else if(size>=100*1024){ // ab 100kb
			item.put(ITEM_KEY_LOW, String.valueOf(size/1024)+" KB");
		} else if(size>=10*1024){ // ab 10kb
			item.put(ITEM_KEY_LOW, String.valueOf(size/1024)+"."+String.valueOf((size/102)%10)+" KB");
		} else if(size>=1024){ // ab 1kb
			item.put(ITEM_KEY_LOW, String.valueOf(size/1024)+"."+String.valueOf((size/10)%100)+" KB");
		} else if(size>0){	// solange es nicht 0 ist
			item.put(ITEM_KEY_LOW, String.valueOf(size)+" Byte");
		} else {
			item.put(ITEM_KEY_LOW, " ");
		};
		item.put(ITEM_IMAGE, imageId);
		mList.add(item);
	}

	private Runnable mCheckVer = new Runnable() {
		public void run() {
			byte send[] = new byte[1];
			send[0] = CMD_SIGN_ON;
			try {
				Log.i("SEND","signon()");
				mSerialService.send(send, 1);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	};

	public void update_visible_elements(boolean visible) {
		if (mSerialService == null
				|| BluetoothSerialService.STATE_CONNECTED != mSerialService
				.getState()) {
			if (mMenuItemConnect != null) {
				// mMenuItemConnect.setIcon(android.R.drawable.ic_menu_close_clear_cancel);
				mMenuItemConnect.setTitle(R.string.connect);
			}
			mloadRoot.setEnabled(false);
			DeleteButton.setEnabled(false);
			DlselButton.setEnabled(false);
			mUpButton.setEnabled(false);
			mDownButton.setEnabled(false);
			mLeftButton.setEnabled(false);
			mRightButton.setEnabled(false);
		} else {
			if (mMenuItemConnect != null) {
				// mMenuItemConnect.setIcon(android.R.drawable.ic_menu_close_clear_cancel);
				mMenuItemConnect.setTitle(R.string.disconnect);
			}
			mloadRoot.setEnabled(true);
			mUpButton.setEnabled(true);
			mDownButton.setEnabled(true);
			mLeftButton.setEnabled(true);
			mRightButton.setEnabled(true);
		}
	}

	
}