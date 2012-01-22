
package de.windeler.kolja;


import java.util.ArrayList;
import java.util.HashMap;
import java.util.TreeMap;

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
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.Button;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.TabHost;
import android.widget.TextView;
import android.widget.Toast;

public class SpeedoAndroidActivity extends TabActivity implements OnClickListener {
	// Name of the connected device
	private static final String TAG = "JKW";

	private MenuItem mMenuItemConnect;
	private BluetoothAdapter mBluetoothAdapter = null;
	public static final String DEVICE_NAME = "device_name";
	public static final String TOAST = "toast";
	public static BluetoothSerialService mSerialService = null;
	private Toast toast;
	/**
	 * Our main view. Displays the emulated terminal screen.
	 */
	private TextView mLog;
	private TextView mStatus;
	private TextView mVersion;
	private TextView mDownload;
	private TextView mUpload;
	private Button mCheckVersion;
	private Button mLeftButton;
	private Button mRightButton;
	private Button mUpButton;
	private Button mDownButton;
	private Button browseToUploadMap;
	private Button browseToUploadConfig;
	private Button browseToUploadSpeedo;
	private Button browseToUploadGfx;
	private Button mloadRoot;
	private Button DlselButton;
	private ListView mDLListView;



	// Message types sent from the BluetoothReadService Handler
	public static final int MESSAGE_STATE_CHANGE = 1;
	public static final int MESSAGE_READ = 2;
	public static final int MESSAGE_WRITE = 3;
	public static final int MESSAGE_DEVICE_NAME = 4;
	public static final int MESSAGE_TOAST = 5;

	private static final int REQUEST_CONNECT_DEVICE = 1;
	private static final int REQUEST_ENABLE_BT 		= 2;
	private static final int REQUEST_OPEN_MAP		= 3;	// file open dialog
	private static final int REQUEST_OPEN_CONFIG	= 4;	// file open dialog
	private static final int REQUEST_OPEN_SPEEDO	= 5;	// file open dialog
	private static final int REQUEST_OPEN_GFX		= 6;	// file open dialog

	// incoming data statemachine
	public static final int ST_IDLE			= -1;
	public static final int ST_START 		= 0;
	public static final int ST_GET_SEQ_NUM	= 1;
	public static final int ST_MSG_SIZE		= 2;
	//public static final int ST_MSG_SIZE_2	= 3;
	public static final int ST_GET_TOKEN	= 4;
	public static final int ST_GET_DATA		= 5;
	public static final int ST_GET_CHECK	= 6;
	public static final int ST_PROCESS		= 7;


	// transfer messages
	public static final byte MESSAGE_START 		=  0x1C;
	public static final byte TOKEN				=  0x0E;

	public static final byte CMD_SIGN_ON		=  0x01;
	public static final byte CMD_LEAVE_FM		=  0x04;
	public static final byte CMD_GO_LEFT		=  0x05;
	public static final byte CMD_GO_RIGHT		=  0x06;
	public static final byte CMD_GO_UP			=  0x07;
	public static final byte CMD_GO_DOWN		=  0x08;
	public static final byte CMD_FILE_RECEIVE	=  0x09;
	public static final byte CMD_DIR			=  0x11;

	public static final char STATUS_CMD_OK      =  0x09;
	public static final char STATUS_CMD_FAILED  =  0xC0;
	public static final char STATUS_CKSUM_ERROR =  0xC1;
	public static final char STATUS_CMD_UNKNOWN =  0xC9;
	public static final char STATUS_EOF 		=  0x10;
	
	private long lastSend = System.currentTimeMillis();  


	// rx/tx vars
	private int 	seqNum		=	1;
	private int 	rx_tx_state	= ST_IDLE;
	private int		msgLength	= 0;
	private char	checksum	= 0;
	private char	msgBuffer[] = new char[300];
	private int		ii			= 0;

	// dir me
	private int 	dir_item_nr	= 0;
	private String 	dir_path = "";
	private TreeMap<String, String> dirsMap = new TreeMap<String, String>();
	private TreeMap<String, String> filesMap = new TreeMap<String, String>();
	private TreeMap<String, Integer> typeMap = new TreeMap<String, Integer>();
	private ArrayList<HashMap<String, Object>> mList = new ArrayList<HashMap<String, Object>>();
	private static final String ITEM_KEY = "key";
	private static final String ITEM_IMAGE = "image";
	// String buffer for outgoing messages
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

		// buttons
		mLog = (TextView) findViewById(R.id.log_value);
		mStatus = (TextView) findViewById(R.id.status_value);
		mVersion = (TextView) findViewById(R.id.version_value);
		mDownload = (TextView) findViewById(R.id.Download_textView);
		mUpload = (TextView) findViewById(R.id.Upload_textView);
		mCheckVersion = (Button) findViewById(R.id.button_checkVersion);
		mCheckVersion.setOnClickListener(this);
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
		browseToUploadConfig = (Button) findViewById(R.id.browseToUploadConfig);
		browseToUploadConfig.setOnClickListener(this);
		browseToUploadSpeedo = (Button) findViewById(R.id.browseToUploadSpeedo);
		browseToUploadSpeedo.setOnClickListener(this);
		mloadRoot = (Button) findViewById(R.id.loadRoot);
		mloadRoot.setOnClickListener(this);
		DlselButton = (Button) findViewById(R.id.DownloadButtonSelect);
		DlselButton.setEnabled(false);
		DlselButton.setOnClickListener(this);
		mDLListView = (ListView) findViewById(R.id.dlList);
		
		






		mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
		// If the adapter is null, then Bluetooth is not supported
		if (mBluetoothAdapter == null) {
			Toast.makeText(this, "Bluetooth is not available", Toast.LENGTH_LONG).show();
			finish();
			return;
		}

	}

	public void setVersion(String data){
		if(mVersion!=null)
			mVersion.setText(data);
	}

	public void setLog(CharSequence status){
		mLog.setText(status);
	}

	public void setStatus(CharSequence status){
		mStatus.setText(status);
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
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		// Stop the Bluetooth chat services
		if (mSerialService != null) mSerialService.stop();
		Log.e(TAG, "--- ON DESTROY ---");

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
				rx_tx_state	= ST_IDLE;
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
		String filePath;
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
			break;
		case REQUEST_OPEN_MAP:
			filePath = data.getStringExtra(FileDialog.RESULT_PATH);
			Log.i(TAG,"File open gab diesen Dateinamen aus:"+filePath);
			mUpload.setText(filePath);
			//process_uploadFile(REQUEST_OPEN_MAP,filePath)
			break;
		case REQUEST_OPEN_CONFIG:
			filePath = data.getStringExtra(FileDialog.RESULT_PATH);
			Log.i(TAG,"File open gab diesen Dateinamen aus:"+filePath);
			mUpload.setText(filePath);
			//process_uploadFile(REQUEST_OPEN_CONFIG,filePath)
			break;
		case REQUEST_OPEN_GFX:
			filePath = data.getStringExtra(FileDialog.RESULT_PATH);
			Log.i(TAG,"File open gab diesen Dateinamen aus:"+filePath);
			mUpload.setText(filePath);
			//process_uploadFile(REQUEST_OPEN_GFX,filePath)
			break;
		case REQUEST_OPEN_SPEEDO:
			filePath = data.getStringExtra(FileDialog.RESULT_PATH);
			Log.i(TAG,"File open gab diesen Dateinamen aus:"+filePath);
			mUpload.setText(filePath);
			//process_uploadFile(REQUEST_OPEN_SPEEDO,filePath)
			break;
		case RESULT_CANCELED:
			Log.i(TAG,"File open abgebrochen");
			break;
		default:
			Log.i(TAG,"nicht gut, keine ActivityResultHandle gefunden");

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
					if(mStatus!=null){	setStatus("Connected");	};
					break;

				case BluetoothSerialService.STATE_CONNECTING:
					if (mMenuItemConnect != null) {
						//mMenuItemConnect.setIcon(android.R.drawable.ic_menu_close_clear_cancel);
						mMenuItemConnect.setTitle(R.string.disconnect);
					}
					if(mStatus!=null){	setStatus("Connecting...");	};
					Toast.makeText(getApplicationContext(), "Connecting ...", Toast.LENGTH_SHORT).show();
					break;

				case BluetoothSerialService.STATE_NONE:
					if(mStatus!=null){	mStatus.setText(R.string.not_connected);	};
					if (mMenuItemConnect != null) {
						//mMenuItemConnect.setIcon(android.R.drawable.ic_menu_close_clear_cancel);
						mMenuItemConnect.setTitle(R.string.connect);
					}
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
				Toast.makeText(getApplicationContext(), "Connected", Toast.LENGTH_SHORT).show();
				break;

			case MESSAGE_READ:
				byte[] readBuf = (byte[]) msg.obj;
				for(int i=0; i<msg.arg1; i++){
					//Log.d(TAG, "geht das:"+readBuf[i]);
					process_incoming((char)readBuf[i]);
				};
				// construct a string from the valid bytes in the buffer
				//String readMessage = new String(readBuf, 0, msg.arg1);
				//Log.i(TAG,readMessage);
				//Toast.makeText(getApplicationContext(), readMessage, Toast.LENGTH_SHORT).show();
				//if(mDownload!=null){	mDownload.append(readMessage);	};
				break;
			}
		}
	};

	private void process_outgoing(byte data[],int msgLength){
		byte	checksum		= 0;
		byte 	c				= 0;
		byte 	p				= 0;
		// nur senden, wenn wir nicht gerade was empfangen
		if(mSerialService.getState()!=mSerialService.STATE_CONNECTED){
			toast.cancel();
			toast = Toast.makeText(this, "You are not connected", Toast.LENGTH_SHORT);
			toast.show();
			mDownload.setText("test");
			return;
		}
		
		if(System.currentTimeMillis()-lastSend>1000){ // 5 sec nix gehört
			rx_tx_state=ST_IDLE;
			seqNum=1;
		} else {
			seqNum++;
		}
		
		if(rx_tx_state==ST_IDLE){
			if(msgLength<=0) return;
			Log.i(TAG,"Sende nachricht");

			c=(byte)MESSAGE_START;
			mSerialService.write(c);		// Message Start
			Log.d(TAG,"BTsend:"+String.valueOf((int)c));
			checksum	=	c;
			c=(byte)seqNum;
			mSerialService.write(c);		// Seq Nr
			Log.d(TAG,"BTsend:"+String.valueOf((int)c));
			checksum	^=	c;
			c=(byte) (msgLength&0x00FF);
			mSerialService.write(c);		// length max 255
			Log.d(TAG,"BTsend:"+String.valueOf((int)c));
			checksum ^= c;
			c=(byte)TOKEN;
			mSerialService.write(c);		// Token
			Log.d(TAG,"BTsend:"+String.valueOf((int)c));
			checksum ^= TOKEN;


			for(int i=0; i<msgLength; i++){
				p	=	data[i];
				mSerialService.write(p);	// send some data
				checksum ^= p;
				Log.d(TAG,"BTsend:"+String.valueOf((int)p));
			}
			mSerialService.write(checksum);	//	CHECKSUM
			Log.d(TAG,"BTsend:"+String.valueOf((int)checksum));
			rx_tx_state=ST_START; // start listening
			lastSend = System.currentTimeMillis();
		} else {
			Log.i(TAG,"State nicht IDLE");
		};
	};


	private void process_incoming(char data) {
		Log.i(TAG,"process_incoming gestartet mit:"+String.valueOf((int)(data&0x00ff))+" rx_state:"+String.valueOf((int)rx_tx_state));			
		switch(rx_tx_state){
		case ST_START:
			if ( data == MESSAGE_START){
				Log.i(TAG,"Message start erhalten");
				rx_tx_state	=	ST_GET_SEQ_NUM;
				checksum	=	data;
			}
			break;

		case ST_GET_SEQ_NUM:
			if ( (int)data == 1 || (int)data == seqNum ){
				Log.i(TAG,"Seq nr erhalten");
				seqNum		=	data  & 0xff;
				rx_tx_state	=	ST_MSG_SIZE;
				checksum	^=	data;
			} else {
				Log.i(TAG,"Seq unpassend:"+String.valueOf((int)data)+" erwartet "+String.valueOf((int)seqNum));
				rx_tx_state	=	ST_START;
			}
			break;

		case ST_MSG_SIZE:
			Log.i(TAG,"MSG size 1 erhalten");
			msgLength		=	data;
			rx_tx_state		=	ST_GET_TOKEN;
			checksum		^=	data;
			Log.i(TAG,"msgLength="+String.valueOf((int)msgLength));
			break;

		case ST_GET_TOKEN:
			if ( data == TOKEN ){
				Log.i(TAG,"Token erhalten");
				rx_tx_state		=	ST_GET_DATA;
				checksum		^=	data;
				ii				=	0;
			} else {
				mDownload.append("ST_START");
				Log.e(TAG,"Token NICHT erhalten!");
				rx_tx_state	=	ST_START;
			}
			break;

		case ST_GET_DATA:
			msgBuffer[ii++]	=	data;
			checksum		^=	data;
			Log.i(TAG,"Erhalte Daten i="+String.valueOf((int)ii)+" von "+String.valueOf((int)msgLength));
			if (ii == msgLength ){
				rx_tx_state	=	ST_GET_CHECK;
			}
			break;

		case ST_GET_CHECK:
			if ( data == checksum ){
				rx_tx_state	=	ST_IDLE;

				Log.i(TAG,"Checksum korrekt");
				if(msgBuffer[1]==STATUS_CMD_OK){
					setLog("Command OK");
				} else if(msgBuffer[1]==STATUS_CMD_FAILED) {
					setLog("Command failed");
				} else if(msgBuffer[1]==STATUS_CMD_UNKNOWN) {
					setLog("Command unknown");
				}

				switch((msgBuffer[0])){
				case CMD_SIGN_ON:
					// hier jetzt in unsere oberflche die id eintragen
					if((msgBuffer[1] & 0xff)==STATUS_CMD_OK){
						String str = new String(msgBuffer);
						str=str.substring(2,msgLength);
						setVersion(str);
					} else {
						// irgendwie das command nochmal senden
					}
					break;
				case CMD_FILE_RECEIVE:
					break;
				// da alle richtungen zwar betätigt werden, danach die schleife auf dem AVR aber unterbrochen wird -> seqNr resetten
				case CMD_GO_LEFT:
				case CMD_GO_RIGHT:
				case CMD_GO_UP:
				case CMD_GO_DOWN:
					seqNum=1;
					break;
				case CMD_DIR:
					Log.i(TAG,"CMD Dir erhalten");

					String str = new String(msgBuffer);
					str=str.substring(3,msgLength);
					Log.i(TAG,"test="+str);

					receive_dir(str,(int)msgBuffer[2]);

					break;
				default:
					// irgendwie das commando nochmal senden
					break;
				}

			} else {
				Log.i(TAG,"Checksum FALSCH");
				setLog("Checksum failed");
				// ankommenden nachricht war nicht korrekt uebertragen
				rx_tx_state	=	ST_IDLE;
			}
			break;

		}			
	}

	@Override
	public void onClick(View arg0) {
		Intent intent; // reusable
		switch (arg0.getId()){
		case R.id.button_checkVersion:
			byte send[] = new byte[1];
			send[0]=CMD_SIGN_ON;
			process_outgoing(send,1);
			break;
		case R.id.button_left:
			byte send2[] = new byte[1];
			send2[0]=CMD_GO_LEFT;
			process_outgoing(send2,1);
			break;
		case R.id.button_up:
			byte send3[] = new byte[1];
			send3[0]=CMD_GO_UP;
			process_outgoing(send3,1);
			break;
		case R.id.button_right:
			byte send4[] = new byte[1];
			send4[0]=CMD_GO_RIGHT;
			process_outgoing(send4,1);
			break;
		case R.id.button_down:
			byte send5[] = new byte[1];
			send5[0]=CMD_GO_DOWN;
			process_outgoing(send5,1);
			break;
		case R.id.browseToUploadMap:
			intent = new Intent(getBaseContext(),FileDialog.class);
			intent.putExtra(FileDialog.START_PATH, "/sdcard");
			intent.putExtra(FileDialog.SELECTION_MODE, SelectionMode.MODE_OPEN);
			startActivityForResult(intent, REQUEST_OPEN_MAP);
			break;
		case R.id.browseToUploadConfig:
			intent = new Intent(getBaseContext(),FileDialog.class);
			intent.putExtra(FileDialog.START_PATH, "/sdcard");
			intent.putExtra(FileDialog.SELECTION_MODE, SelectionMode.MODE_OPEN);
			startActivityForResult(intent, REQUEST_OPEN_CONFIG);
			break;
		case R.id.browseToUploadGfx:
			intent = new Intent(getBaseContext(),FileDialog.class);
			intent.putExtra(FileDialog.START_PATH, "/sdcard");
			intent.putExtra(FileDialog.SELECTION_MODE, SelectionMode.MODE_OPEN);
			startActivityForResult(intent, REQUEST_OPEN_GFX);
			break;
		case R.id.browseToUploadSpeedo:
			intent = new Intent(getBaseContext(),FileDialog.class);
			intent.putExtra(FileDialog.START_PATH, "/sdcard");
			intent.putExtra(FileDialog.SELECTION_MODE, SelectionMode.MODE_OPEN);
			startActivityForResult(intent, REQUEST_OPEN_SPEEDO);
			break;
		case R.id.loadRoot:
			toast = Toast.makeText(getBaseContext(), "loading content of folder /.\nPlease wait...", 9999);
			toast.show();
			send_dir("/",0); // change to "/"
			break;
		case R.id.DownloadButtonSelect:
			//		     if (selectedFile != null) {
			//                             getIntent().putExtra(RESULT_PATH, selectedFile.getPath());
			//                             setResult(RESULT_OK, getIntent());
			//                             finish();

			break;

		default:
			Log.i(TAG,"Hier wurde was geklickt das ich nicht kenne!!");
			break;
		}
	}

	public void send_dir(String dir,int item){
		Log.i(TAG,"Betrete send_dir mit:"+dir+" und item="+String.valueOf(item));
		dir_item_nr=item;
		dir_path=dir;
		byte send[] = new byte[dir.length()+2+1]; // 2 für length of item + name + command

		send[0]=CMD_DIR;
		send[1]=(byte) ((item & 0xff00)>>8); //danger wegen signed ? interessant ab über 127 Files
		send[2]=(byte) (item & 0x00ff);

		for(int i=0;i<dir.length();i++){
			send[i+3]=(byte)dir.charAt(i);
		}

		process_outgoing(send, send.length);
	};

	public void receive_dir(String name, int type){
		Log.i(TAG,"receive_dir gestartet mit:"+name);
		if(dir_item_nr==0){
			mDownload.setText("");
			filesMap.clear();
			typeMap.clear();
			dirsMap.clear();
			mList.clear();
		};
		if(type==1){ // file
			mDownload.append("File: "+name+"\n");
			filesMap.put(name,name);
			typeMap.put(name,1);
		} else if(type==2){ // dir
			mDownload.append("Folder: "+name+"\n");
			dirsMap.put(name, name);
			typeMap.put(name,2);
		};
		if(type!=STATUS_EOF){
			dir_item_nr++;
			send_dir(dir_path,dir_item_nr);
		} else {
			Log.d(TAG,"beginne liste aufzubauen");
			

			// send to display
			SimpleAdapter fileList = new SimpleAdapter(this, mList, R.layout.file_dialog_row,
					new String[] { ITEM_KEY, ITEM_IMAGE }, new int[] { R.id.fdrowtext, R.id.fdrowimage });
			
			if(dir_path!="/"){
				addItem("/", R.drawable.folder);
				typeMap.put("/", 2);
			};
			
			for (String dir : dirsMap.tailMap("").values()) {
				if(dir.toString().length()>23){
					addItem(dir.toString().substring(0, 20)+"...", R.drawable.folder);
				} else {
					addItem(dir, R.drawable.folder);
				}
			}

			for (String file : filesMap.tailMap("").values()) {
				if(file.toString().length()>23){
					addItem(file.toString().substring(0, 20)+"...", R.drawable.file);
				} else {
					addItem(file, R.drawable.file);
				}
			}
			mDLListView.setAdapter(fileList);
			if(toast!=null){
				toast.cancel();
			}
			mDLListView.setOnItemClickListener(new OnItemClickListener()
	        {
	        	public void onItemClick(AdapterView<?> arg0, View arg1,int arg2, long arg3)
	        	{
	        		String name=null;
	        		Integer type=0;
	        		HashMap<String, Object> item = new HashMap<String, Object>();
	        			        		
	        		item = mList.get(arg2);
	        		name=(String) item.get(ITEM_KEY);
	        		type=typeMap.get(name);
	        		
	        		if(type==1){
	        			String path="/";
	        			if(dir_path!="/")
	        				path=path+dir_path+"/";
	        			path=path+name;
	        			
	        			Toast.makeText(getBaseContext(), "You clicked on file "+path, Toast.LENGTH_LONG).show();
	        			DlselButton.setEnabled(true);
	        		} else if (type==2) {
	        			toast = Toast.makeText(getBaseContext(), "loading content of folder "+name+".\nPlease wait...", 9999);
	        			toast.show();
	        			send_dir(name,0); 
	        		}
	    		}
	    	});
		}
	}


	private void addItem(String fileName, int imageId) {
		HashMap<String, Object> item = new HashMap<String, Object>();
		item.put(ITEM_KEY, fileName);
		item.put(ITEM_IMAGE, imageId);
		mList.add(item);
	}
	
}
