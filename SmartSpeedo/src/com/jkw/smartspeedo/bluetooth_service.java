/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.jkw.smartspeedo;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;
import java.util.concurrent.Semaphore;

import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;
import android.widget.Toast;

/**
 * This class does all the work for setting up and managing Bluetooth
 * connections with other devices. It has a thread that listens for
 * incoming connections, a thread for connecting with a device, and a
 * thread for performing data transmissions when connected.
 */
public class bluetooth_service {
	////////////////////////////////////////BROADCAST COMMUNICATIONS ////////////////////////////////////////
	// Interprocess communications
	public static final int short_name = 19;
	public static final int update = 1;
	

	// Main Identifier 
	public static final String BT_ACTION = "BT_ACTION";   	// send as identifier to sensor if the message is a sensor update or bluetooth toggle or or or
	public static final String BT_COMMAND = "BT_COMMAND"; 	// send as identifier to send to BT command byte to sensor class
	public static final String BT_VALUE = "BT_VALUE";		// send as identifier to send to BT value to sensor class

	// argument for main identifier
	public static final String BT_RESTART = "restart";					// argument for BT action
	public static final String ENABLE_BT = "ENABLE_BT";					// argument for BT action
	public static final String BT_CONNECT = "BT_CONNECT";				// argument for BT action
	public static final String BT_STOP = "STOP";
	public static final String BT_STATE_CHANGE = "BT_STATE_CHANGE";		// argument for BT action
	public static final String BT_SENSOR_UPDATE = "BT_SENSOR_UPDATE";	// argument for BT action

	// sub-arguments
	public static final String TARGET_ADDRESS = "target_address";		// subargument for BT_CONNECT
	public static final String BT_SENSOR_VALUE = "BT_SENSOR_VALUE";		// subargument for BT_SENSOR_UPDATE


	//////////////////////////////////////// BROADCAST COMMUNICATIONS ////////////////////////////////////////

	private static final String TAG = "JKW";
	private static final String TAG_SEM = "JKW_SEM";
	private static final String TAG_RECV = "JKW_RECV";
	private static final String TAG_TIME = "JKW_TIME";
	private static final String TAG_LOGIN = "LOGIN";
	private final Semaphore semaphore = new Semaphore(1, true);
	private char status=1;
	private int seqNum = 0;
	private long lastSend = System.currentTimeMillis();
	private int 	rx_tx_state	= ST_IDLE; // state der machine to process incoming data
	private int		msgLength	= 0;
	private char	checksum	= 0;
	private char	msgBuffer[] = new char[300];
	private int		ii			= 0;
	private Handler mTimerHandle = new Handler();
	private boolean silent=false;

	// states of the Bluetooth statemachine
	public static final int ST_IDLE				= -1;
	public static final int ST_START 			= 0;
	public static final int ST_GET_SEQ_NUM		= 1;
	public static final int ST_MSG_SIZE			= 2;
	public static final int ST_MSG_SIZE_2		= 3;
	public static final int ST_GET_TOKEN		= 4;
	public static final int ST_GET_DATA			= 5;
	public static final int ST_GET_CHECK		= 6;
	public static final int ST_PROCESS			= 7;
	public static final int ST_EMERGENCY_RELEASE= 999;


	// Message types sent by the BluetoothReadService Handler
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
	public static final int MESSAGE_SENSOR_VALUE = 13;

	private static final UUID SerialPortServiceClass_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

	public BluetoothDevice last_connected_device=null;
	private BluetoothAdapter mAdapter = null;

	private ConnectThread mConnectThread;
	private ConnectedThread mConnectedThread;
	private int mState;

	private int transfere_speed_flat;

	// Constants that indicate the current connection state
	public static final int STATE_NONE = 0;       // we're doing nothing
	public static final int STATE_CONNECTING = 2; // now initiating an outgoing connection
	public static final int STATE_CONNECTED = 3;  // now connected to a remote device
	public static final int STATE_CONNECTED_AND_SEARCHING = 4;  // now connected to a remote device

	public static final byte MESSAGE_START 			=  0x1B;
	public static final byte TOKEN					=  0x0E;

	public static final byte CMD_SIGN_ON			=  0x01;
	public static final byte CMD_LEAVE_FM			=  0x04;
	public static final byte CMD_LOAD_ADDRESS		=  0x06;
	public static final byte CMD_LEAVE_PROGMODE_ISP	=  0x11;
	public static final byte CMD_CHIP_ERASE_ISP		=  0x12;
	public static final byte CMD_PROGRAM_FLASH_ISP	=  0x13;
	public static final byte CMD_READ_FLASH_ISP     =  0x14;
	public static final byte CMD_SPI_MULTI			=  0x1D;
	public static final byte CMD_GO_LEFT			=  0x25;
	public static final byte CMD_GO_RIGHT			=  0x26;
	public static final byte CMD_GO_UP				=  0x27;
	public static final byte CMD_GO_DOWN			=  0x28;
	public static final byte CMD_DIR				=  0x31;
	public static final byte CMD_GET_FILE			=  0x32;
	public static final byte CMD_PUT_FILE			=  0x33;
	public static final byte CMD_DEL_FILE			=  0x34;
	public static final byte CMD_SHOW_GFX			=  0x35;
	public static final byte CMD_FILE_RECEIVE		=  0x39;
	public static final byte CMD_RESET_SMALL_AVR	=  0x40;
	public static final byte CMD_SIGN_ON_FIRMWARE	=  0x41;
	public static final byte CMD_SET_STARTUP		=  0x42;

	public static final byte CMD_GET_WATER_TEMP_ANALOG	=  0x53;
	public static final byte CMD_GET_WATER_TEMP_DIGITAL	=  0x54;
	public static final byte CMD_GET_OIL_TEMP_ANALOG	=  0x55;
	public static final byte CMD_GET_OIL_TEMP_DIGITAL	=  0x56;
	public static final byte CMD_GET_FLASHER_LEFT		=  0x47;
	public static final byte CMD_GET_FLASHER_RIGHT		=  0x58;
	public static final byte CMD_GET_SPEED				=  0x4A;
	
	public static final byte CMD_GET_FREQ_RPM			=  0x48; // set !
	public static final byte CMD_GET_FREQ_SPEED			=  0x49; // set !
	public static final byte CMD_GET_AIR_TEMP_ANALOG	=  0x46; // set !

	public static final char STATUS_CMD_OK      	=  0x00;
	public static final char STATUS_CMD_FAILED  	=  0xC0;
	public static final char STATUS_CKSUM_ERROR 	=  0xC1;
	public static final char STATUS_CMD_UNKNOWN 	=  0xC9;
	public static final char STATUS_EOF		   		=  0x10;

	public static final String PREFS_NAME = "SmartSpeedoSettings";
	private Handler mHandle;
	private Context mContext;

	/**
	 * Ablauf des Verbindungsaufbau:
	 * 1. Diese Klasse wird in einer externen Activity Instaziert (die externe Activity gibt einen callBackHandle mit)
	 * 2. Die Methode connect() wird aufgerufen mit einer Bluetooth Device ID als Argument
	 * 3. Connect() startet einen neuen Hintergrund Thread: ConnectThread damit das UI nicht einfriert beim verbinden
	 * 4. Der ConnectThread ruft in seinem constructor schon die Android connect API funktion auf
	 * 5. Die Funktion run vom ConnectThread baut dann einen Socket auf. Wenn das klappt wird ein weiterer neuer Thread vom Typ ConnectedThread gestartet connectThread ist damit zuende und läuft aus
	 * 7. Der ConnectedThread öffnet den Socket und läuft auf ewig in einer Endlosschleife und wartet auf neue Daten
	 * 
	 * Ablauf des Empfangens:
	 * 1. Der ConnectedThread liest Daten ein und schickt diese byte für byte an process_incoming()
	 * 2. wenn die checksumme stimmt und die daten vollständig sind werden sie über den handle zurück an die Activity geschickt
	 * 
	 * Ablauf des Sendens:
	 * 1. send_save bekommt ein command, ein timeout und die anzahl an retries mit geliefert
	 * 2. send_save ruft mit command und timeout send() auf
	 * 2.1 send holt sich als erstes einen semaphore
	 * 2.2 send_save muss warten weil es versucht sich den zweiten semaphore zu holen (es gibt nur einen und den hat send)
	 * 3. send erstellt aus der payload ein packet und schreibt es auf den ConnectedThread, gleichzeitig wird ein timer gestartet
	 * 4.1.1 wenn keine daten zurück kommen läuft der timer ab und gibt einen semaphore frei (den send geholt hatte) dadurch wird send_save wieder aktiv
	 * 4.1.2 send_save gibt den semaphore zurück und versucht den vorgang erneut (wenn die anzahl der retries es zulässt)
	 * 4.2.1 wenn eine antwort kommt wird sie von process_incoming() empfangen und der semaphore wird zurück gegeben (den send geholt hatte)
	 */

	public bluetooth_service(Handler mBluetoothHandle,Context applicationContext) {
		mHandle = mBluetoothHandle;
		mContext = applicationContext;
	

		// prepare bluetooth
		mAdapter = BluetoothAdapter.getDefaultAdapter();
		// If the adapter is null, then Bluetooth is not supported
		if (mAdapter == null) {
			Toast.makeText(mContext, "Bluetooth is not available",Toast.LENGTH_LONG).show(); 
		} else {
			if (!mAdapter.isEnabled()) {
				Message msg = mHandle.obtainMessage(short_name);
		        Bundle bundle = new Bundle();
		        bundle.putString(BT_ACTION, ENABLE_BT); // TODO
		        msg.setData(bundle);
		        mHandle.sendMessage(msg);
			}
		};

		mState=STATE_NONE;
		//		mSensors = new Sensors(this);
	}

	public void restart(){
		stop();
		start();
	}

	public void connect(String bt_adr){
		if(bt_adr!=""){
			BluetoothDevice device = mAdapter.getRemoteDevice(bt_adr);
			try {
				connect(device,false);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}


	/**
	 * Set the current state of the connection
	 * @param state  An integer defining the current connection state
	 */
	public synchronized void setState(int state) {
		Log.i(TAG,"Setting state to "+String.valueOf(state));
		mState = state;
		// Give the new state to the Handler so the UI Activity can update		
        Message msg = mHandle.obtainMessage(short_name);
        Bundle bundle = new Bundle();
        bundle.putString(BT_ACTION, BT_STATE_CHANGE);
        bundle.putInt(BT_STATE_CHANGE, (int)state);
        msg.setData(bundle);
        mHandle.sendMessage(msg);


		if(state == STATE_CONNECTED){
			rx_tx_state	= ST_IDLE;
		};

	}

	/**
	 * Return the current connection state. */
	public synchronized int getState() {
		return mState;
	}

	/**
	 * Start the service. Specifically start AcceptThread to begin a
	 * session in listening (server) mode. Called by the Activity onResume() or at failed */
	public synchronized void start() {
		// Cancel any thread attempting to make a connection
		if (mConnectThread != null) {
			mConnectThread.cancel(); 
			mConnectThread = null;
		}

		// Cancel any thread currently running a connection
		if (mConnectedThread != null) {
			mConnectedThread.cancel(); 
			mConnectedThread = null;
		}

		setState(STATE_NONE);
	}

	/**
	 * Start the ConnectThread to initiate a connection to a remote device.
	 * @param device  The BluetoothDevice to connect
	 * @throws InterruptedException 
	 */
	public synchronized void connect(BluetoothDevice device,boolean goto_bootloader) throws InterruptedException {

		// Cancel any thread currently running a connection
		if (mConnectedThread != null) {mConnectedThread.cancel(); mConnectedThread = null;}

		// Start the thread to connect with the given device
		Log.e("connect","start thread to connect...");
		last_connected_device=device; // remember for reconnect

		mConnectThread = new ConnectThread(device);
		Log.i("connect","starte jetzt mConnectedThread");
		mConnectThread.setGotoBootloader(goto_bootloader);
		mConnectThread.start();
		Log.i("connect","mConnectedThread ist durch");
		setState(STATE_CONNECTING);
	}


	/**
	 * Stop all threads
	 */
	public synchronized void stop() {
		Log.d("connect", "stop");

		if (mConnectThread != null) {
			mConnectThread.cancel(); 
			mConnectThread = null;
		}

		if (mConnectedThread != null) {
			mConnectedThread.cancel(); 
			mConnectedThread = null;
		}

		setState(STATE_NONE);
	}

	/**
	 * Write to the ConnectedThread in an unsynchronized manner
	 * @param out The bytes to write
	 * @see ConnectedThread#write(byte[])
	 */
	public void write(byte out[],int count) {
		// Create temporary object
		ConnectedThread r;
		// Synchronize a copy of the ConnectedThread
		synchronized (this) {
			// wait max 100ms until mConnectedThread has to be !=null
			int retries=100;
			while(mConnectedThread==null && retries>0){
				retries--;
				try {
					Thread.sleep(1);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
			if(mConnectedThread==null){
				// connection failed
				//Log.i("connect","Thread not ready in time");
				return;
			}
			r = mConnectedThread;
		}
		// Perform the write unsynchronized
		r.write(out,count);
	}

	/**
	 * Indicate that the connection attempt failed and notify the UI Activity.
	 */
	private void connectionFailed() {
		setState(STATE_NONE);

		// TODO
		// Send a failure message back to the Activity
		//		Message msg = mHandler.obtainMessage(MESSAGE_TOAST);
		//		Bundle bundle = new Bundle();
		//		bundle.putString(TOAST, "Unable to connect device");
		//		msg.setData(bundle);
		//		mHandler.sendMessage(msg);
		//
		//		BluetoothSerialService.this.start();
	}


	/**
	 * This thread runs while attempting to make an outgoing connection
	 * with a device. It runs straight through; the connection either
	 * succeeds or fails.
	 */
	private class ConnectThread extends Thread {
		private final BluetoothSocket mmSocket;
		private boolean goto_bootloader=false;

		public ConnectThread(BluetoothDevice device) {
			BluetoothSocket tmp = null;

			// Get a BluetoothSocket for a connection with the
			// given BluetoothDevice
			try {
				tmp = device.createRfcommSocketToServiceRecord(SerialPortServiceClass_UUID);
				Log.i("connect","RfcommSocked created()");
			} catch (IOException e) {
				Log.e("connect", "create() failed", e);
			}
			mmSocket = tmp;
		}

		public void setGotoBootloader(boolean b) {
			goto_bootloader=b;			
		}

		public void run() {
			Log.i("connect", "BEGIN mConnectThread");
			setName("ConnectThread");

			// Always cancel discovery because it will slow down a connection
			mAdapter.cancelDiscovery();

			// Make a connection to the BluetoothSocket
			try {
				// This is a blocking call and will only return on a
				// successful connection or an exception
				mmSocket.connect();
				Log.i("connect","Connect() finished...");
			} catch (IOException e) {
				Log.e("connect", "mConnectThread FAILED");
				connectionFailed();
				// Close the socket
				try {
					mmSocket.close();
				} catch (IOException e2) {
					Log.e("connect", "unable to close() socket during connection failure", e2);
				}
				// Start the service over to restart listening mode
				//BluetoothSerialService.this.start();
				return;
			}

			// Reset the ConnectThread because we're done
			synchronized (this) {
				mConnectThread = null;
			}

			// Cancel the thread that completed the connection
			if (mConnectThread != null) {
				mConnectThread.cancel(); 
				mConnectThread = null;
			}

			// Cancel any thread currently running a connection
			if (mConnectedThread != null) {
				mConnectedThread.cancel(); 
				mConnectedThread = null;
			}

			// Start the thread to manage the connection and perform transmissions
			mConnectedThread = new ConnectedThread(mmSocket);
			Log.i(TAG,"starte jetzt mConnectedThread");
			mConnectedThread.start();
			Log.i(TAG,"mConnectedThread start fertig");

			// Send the name of the connected device back to the UI Activity
			//			Message msg = mHandler.obtainMessage(MESSAGE_DEVICE_NAME);
			//			Bundle bundle = new Bundle();
			//			bundle.putString(DEVICE_NAME, mmDevice.getName());
			//			msg.setData(bundle);
			//			mHandler.sendMessage(msg);

			setState(STATE_CONNECTED_AND_SEARCHING);

			// if we don't need the bootloader, lets connect right here to the 
			// firmware and set the State to connected 

			if(!goto_bootloader){
				//wait 2 sec to prevent goint to bootloader
				try {
					Thread.sleep(2000);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}

				// check if connected
				silent=true; // kein popup, da wir durchaus mal 1...2... timeouts abwarten m�ssen f�r den bootloader
				byte send[] = new byte[1];
				send[0] = CMD_SIGN_ON_FIRMWARE;
				if(send_save(send,1,400,100)==0){ // 4 sec versuchen
					setState(STATE_CONNECTED);
				} else {
					setState(STATE_NONE);
				}
				silent=false;
			}
		}

		public void cancel() {
			try {
				mmSocket.close();
			} catch (IOException e) {
				Log.e("connect", "close() of connect socket failed", e);
			}
		}
	}

	/**
	 * This thread runs during a connection with a remote device.
	 * It handles all incoming and outgoing transmissions.
	 */
	private class ConnectedThread extends Thread {
		private final BluetoothSocket mmSocket;
		private final InputStream mmInStream;
		private final OutputStream mmOutStream;


		public ConnectedThread(BluetoothSocket socket) {
			Log.i("connect", "create ConnectedThread");
			mmSocket = socket;
			InputStream tmpIn = null;
			OutputStream tmpOut = null;

			// Get the BluetoothSocket input and output streams
			try {
				tmpIn = socket.getInputStream();
				tmpOut = socket.getOutputStream();
			} catch (IOException e) {
				Log.e("connect", "temp sockets not created", e);
			}
			Log.i("connect","Thread ready");
			mmInStream = tmpIn;
			mmOutStream = tmpOut;
		}

		public void run() {
			Log.i("connect", "BEGIN mConnectedThread");
			byte[] buffer = new byte[1024];
			int bytes;

			// Keep listening to the InputStream while connected
			while (true) {
				try {
					// Read from the InputStream
					bytes = mmInStream.read(buffer);
					for(int i=0;i<bytes;i++){
						process_incoming((char)buffer[i]);
					}
				} catch (IOException e) {
					Log.e("connect", "disconnected", e);
					//connectionLost();
					break;
				}
			}
		}

		/**
		 * Write to the connected OutStream.
		 * @param out  The bytes to write
		 */
		public void write(byte out[],int count) {
			try {
				mmOutStream.write(out, 0, count);
			} catch (IOException e) {
				Log.e(TAG, "Exception during write", e);
			}
		}

		public void cancel() {
			try {
				mmSocket.close();
			} catch (IOException e) {
				Log.e(TAG, "close() of connect socket failed", e);
			}
		}
	}

	public void reset_seq() {
		seqNum=1;
	}

	// beim save send gucken wir uns an ob wir auf die anfrage hin auch noch eine antwort erhalten
	// haben, falls nicht machen wir sogar retries
	public int send_save(byte data[],int msgLength, int timeout, int retries ){
		int failCounter=0;
		int return_value = 0;
		while(failCounter<retries){
			// send starten, send holt sich den verf?gbaren semaphore
			try {
				return_value=send(data,msgLength,timeout);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			if(return_value>0){
				Log.i(TAG_SEM,"Kolja 11: Break");
				break;
			}

			// jetzt holen wir uns einen, das blockiert uns bis 
			// a) Receive uns den semaphore von send zur?ck gegeben hat
			// oder 
			// b) der timer abgelaufen ist
			try {
				semaphore.acquire();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			semaphore.release();

			if(status==ST_EMERGENCY_RELEASE){
				Log.i(TAG_LOGIN,"Kolja 11:send_save notfall release");
				// hier sowas wie: 
				failCounter++;
				if(failCounter>=retries){
					return_value=-1;
					break;
				}
			} else { 	
				Log.i(TAG_LOGIN,"Kolja 11:answere received");
				return_value=0;
				break;
			}
		}
		return return_value;
	}

	public int send(byte data[],int msgLength ) throws InterruptedException {
		return send(data,msgLength,2000);
	}

	public int send(byte data[],int msgLength, int time) throws InterruptedException{
		byte[]	checksum = {0};
		byte 	c[] = {0};
		byte 	p[] = {0};
		// nur senden, wenn wir nicht gerade was empfangen
		if(getState()!=STATE_CONNECTED && data[0]!=CMD_SIGN_ON && data[0]!=CMD_SIGN_ON_FIRMWARE && data[0]!=CMD_RESET_SMALL_AVR){
			//			Message msg = mHandler.obtainMessage(MESSAGE_TOAST);
			//			Bundle bundle = new Bundle();
			//			bundle.putString(TOAST, "You are not connected to a SmartSpeedo");
			//			bundle.putInt(result, -1);
			//			msg.setData(bundle);
			//			mHandler.sendMessage(msg);

			return 1;
		}

		Log.i(TAG_SEM,"Kolja 11: BT Telegramm will starten, warte auf den semaphore");
		Log.i(TAG_SEM,String.valueOf(semaphore.availablePermits())+" frei");
		semaphore.acquire(1);
		Log.i(TAG_SEM,"Kolja 11: send hat den semaphore");

		// da der Tacho, nach 2sek den fast response mode verlaesst, muessen wir die seq neu zaehlen
		if(System.currentTimeMillis()-lastSend>time || data[0]==CMD_SIGN_ON || data[0]==CMD_SIGN_ON_FIRMWARE || data[0]==CMD_RESET_SMALL_AVR){
			reset_seq();
			Log.i(TAG_SEM,"Kolja 11: reset seq nr");
		}

		seqNum=(seqNum+1)%256; // wir starten mit 0 und setzten im notfall auch zu 0 zurueck, daher immer VOR dem senden inkrementieren


		if(rx_tx_state==ST_IDLE){
			if(msgLength<=0) return 2;
			c[0]=(byte)MESSAGE_START;
			Log.i(TAG_TIME,"MSG_START send");
			write(c,1);		// Message Start
			Log.d(TAG_RECV,"BTsend:"+String.valueOf((int)c[0])+" /MSG_START");
			checksum[0]	=	c[0];
			c[0]=(byte)(seqNum&0x00FF);
			write(c,1);		// Seq Nr
			Log.d(TAG_RECV,"BTsend:"+String.valueOf(((int)c[0])&0x00ff)+" /Seq Nr");
			checksum[0]	^=	c[0];
			c[0]=(byte) ((msgLength&0xFF00)>>8);
			write(c,1);		// length max 255
			Log.d(TAG_RECV,"BTsend:"+String.valueOf((int)c[0])+" /length of msg highbyte");
			checksum[0] ^= c[0];
			c[0]=(byte) (msgLength&0x00FF);
			write(c,1);		// length max 255
			Log.d(TAG_RECV,"BTsend:"+String.valueOf((int)c[0])+" /length of msg lowbyte");
			checksum[0] ^= c[0];
			c[0]=(byte)TOKEN;
			write(c,1);		// Token
			Log.d(TAG_RECV,"BTsend:"+String.valueOf((int)c[0])+" /TOKEN");
			checksum[0] ^= TOKEN;


			for(int i=0; i<msgLength; i++){
				p[0]	=	data[i];
				checksum[0] ^= p[0];
				//				Log.d(TAG_RECV,"BTsend:"+String.valueOf(((int)p[0])&0x00ff)+"/"+String.valueOf((char)p[0])+"   /DATA "+String.valueOf(i+1)+"/"+String.valueOf(msgLength));
			}
			write(data,msgLength);
			write(checksum,1);	//	CHECKSUM
			//Log.i(TAG_TIME,"checksum send "+String.valueOf(msgLength));
			//Log.d(TAG_RECV,"BTsend:"+String.valueOf((int)checksum[0])+" /Checksum");
			rx_tx_state=ST_START; // start listening

			// install guard, 2sec until check of receive
			mTimerHandle.removeCallbacks(mCheckResponseTimeTask);
			mTimerHandle.postDelayed(mCheckResponseTimeTask, time);
			return 0;

		} else {
			Log.i(TAG,"Kolja 11:State nicht IDLE");
			return 2;
		}
	};
	
	private void return_helper(char command, int value){
		Message msg = mHandle.obtainMessage(update);
        Bundle bundle = new Bundle();
        bundle.putInt(BT_COMMAND, (int)command);
        bundle.putInt(BT_VALUE, value);
        msg.setData(bundle);
        mHandle.sendMessage(msg);
	}

	private Runnable mCheckResponseTimeTask = new Runnable() {
		public void run() {
			if(rx_tx_state!=ST_IDLE){
				rx_tx_state=ST_IDLE;
				reset_seq();
				Log.i(TAG,"Kolja 11:timer notfall, gebe semaphore zurueck");
				Log.i(TAG_RECV,"timer notfall, gebe semaphore zurueck");
				status=ST_EMERGENCY_RELEASE;
				Log.i(TAG_SEM,String.valueOf(semaphore.availablePermits())+" frei durch notfall");
				Log.i(TAG_SEM,"Kolja 11:Notfall timer hat den semaphore zurueck gegeben");
				Log.i(TAG,"schreibe status:"+String.valueOf(status));

				if(!silent){
					Message msg = mHandle.obtainMessage(MESSAGE_TOAST);
					Bundle bundle = new Bundle();
//					bundle.putString(TOAST, "No response from target");
					msg.setData(bundle);
					mHandle.sendMessage(msg);
				}
				semaphore.release(); // als letztes !!
			};
		}
	};

	private void process_incoming(char data) {
		//		Log.i(TAG,"process_incoming gestartet mit:"+String.valueOf((int)(data&0x00ff))+"/"+((char)(data&0x00ff))+" rx_state:"+String.valueOf((int)rx_tx_state));			
		switch(rx_tx_state){
		case ST_START:
		case ST_IDLE:
			if ( data == MESSAGE_START){
				Log.i(TAG,"Message start erhalten");
				Log.i(TAG_TIME,"MSG_START recv");
				rx_tx_state	=	ST_GET_SEQ_NUM;
				checksum	=	data;
			}
			break;

		case ST_GET_SEQ_NUM: // smart speedo exclusion: ignore seqNum, the avr will post messages without request, therefore its useless
			if ( (int)data == 1 || (data&0xff) == (seqNum&0xff) ){
				Log.i(TAG,"Seq nr erhalten");
				seqNum		=	data  & 0xff;
				rx_tx_state	=	ST_MSG_SIZE;
				checksum	^=	data;
			} else {
				Log.i(TAG,"Seq unpassend:"+String.valueOf((int)data)+" erwartet "+String.valueOf((int)seqNum));
				//				rx_tx_state	=	ST_START;
				seqNum		=	data  & 0xff;
				rx_tx_state	=	ST_MSG_SIZE;
				checksum	^=	data;
			}
			break;

		case ST_MSG_SIZE:
			Log.i(TAG,"MSG size erhalten");
			msgLength		=	data;
			msgLength		&=	0x000000FF;
			msgLength		= msgLength<<8;
			rx_tx_state		=	ST_MSG_SIZE_2;
			checksum		^=	data;
			Log.i(TAG,"msgLength preview="+String.valueOf((int)msgLength));
			break;

		case ST_MSG_SIZE_2:
			Log.i(TAG,"MSG size2 erhalten");
			msgLength		|=	(data&0x000000FF);		
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
				for(int ii=0;ii<msgBuffer.length;ii++){
					msgBuffer[ii]=0x00;
				};
			} else {
				Log.e(TAG,"Token NICHT erhalten!");
				rx_tx_state	=	ST_START;
			}
			break;

		case ST_GET_DATA:
			msgBuffer[ii++]	=	data;
			checksum		^=	data;
			//			Log.i(TAG,"Erhalte Daten i="+String.valueOf((int)ii)+" von "+String.valueOf((int)msgLength));
			if (ii == msgLength ){
				rx_tx_state	=	ST_GET_CHECK;
				msgBuffer[ii]='\0';
			}
			break;

		case ST_GET_CHECK:
			rx_tx_state	=	ST_IDLE;
			lastSend = System.currentTimeMillis();

			if ( data == checksum ){

				Log.i(TAG,"Checksum korrekt");
				Log.i(TAG_TIME,"CHECKSUM recv "+String.valueOf(msgLength+5));
				if(msgBuffer[1]==STATUS_CMD_OK){
					//					mHandler.obtainMessage(MESSAGE_CMD_OK, 0, -1).sendToTarget();
				} else if(msgBuffer[1]==STATUS_CMD_FAILED) {
					//					mHandler.obtainMessage(MESSAGE_CMD_FAILED, 0, -1).sendToTarget();
				} else if(msgBuffer[1]==STATUS_CMD_UNKNOWN) {
					//					mHandler.obtainMessage(MESSAGE_CMD_UNKNOWN, 0, -1).sendToTarget();
				}

				boolean value_b = false;

				if(msgBuffer[0]==CMD_SIGN_ON || msgBuffer[0]==CMD_SIGN_ON_FIRMWARE){
					// hier jetzt in unsere oberflche die id eintragen
					if((msgBuffer[1] & 0xff)==STATUS_CMD_OK){
						//						msg = mHandler.obtainMessage(MESSAGE_SET_VERSION);
						//						bundle = new Bundle();
						//						bundle.putString(TOAST, str.substring(2,msgLength));
						//						msg.setData(bundle);
						//						mHandler.sendMessage(msg); 
						Log.i(TAG,"statemachine ok, gebe semaphore zurueck");

					} else {
						// irgendwie das command nochmal senden
					}
					// da alle richtungen zwar betaetigt werden, danach die schleife auf dem AVR aber unterbrochen wird -> seqNr resetten

				} else if(msgBuffer[0]==CMD_GET_WATER_TEMP_ANALOG){		
					return_helper(msgBuffer[0], (msgBuffer[1] << 8) + (msgBuffer[2] & 0xff));
				} else if(msgBuffer[0]==CMD_GET_FLASHER_LEFT){
					return_helper(msgBuffer[0], (msgBuffer[1]));
				} else if(msgBuffer[0]==CMD_GET_FLASHER_RIGHT){
					return_helper(msgBuffer[0], (msgBuffer[1]));
				} else if(msgBuffer[0]==CMD_GET_FREQ_RPM){
					return_helper(msgBuffer[0], (msgBuffer[1] << 8) + (msgBuffer[2] & 0xff));				
				} else if(msgBuffer[0]==CMD_GET_SPEED){
					return_helper(msgBuffer[0], (msgBuffer[1] << 8) + (msgBuffer[2] & 0xff));
				} else if(msgBuffer[0]==CMD_GET_FREQ_SPEED){
					return_helper(msgBuffer[0], (msgBuffer[1] << 8) + (msgBuffer[2] & 0xff));
				} else if(msgBuffer[0]==CMD_GET_AIR_TEMP_ANALOG){
					return_helper(msgBuffer[0], (msgBuffer[1] << 8) + (msgBuffer[2] & 0xff));

					// list of valid commands, the active task will process the data
				} else if(msgBuffer[0]==CMD_SPI_MULTI || 
						msgBuffer[0]==CMD_LEAVE_FM ||
						msgBuffer[0]==CMD_LOAD_ADDRESS ||
						msgBuffer[0]==CMD_LEAVE_PROGMODE_ISP ||
						msgBuffer[0]==CMD_CHIP_ERASE_ISP ||
						msgBuffer[0]==CMD_PROGRAM_FLASH_ISP ||
						msgBuffer[0]==CMD_READ_FLASH_ISP ||
						msgBuffer[0]==CMD_RESET_SMALL_AVR
						){
				} else {
					Log.i(TAG,"unknown command received:"+String.valueOf(msgBuffer[0]));
					// irgendwie das commando nochmal senden
					//					msg = mHandler.obtainMessage(MESSAGE_TOAST);
					//					bundle = new Bundle();
					//					bundle.putString(TOAST, "unknown command from speedo received");
					//					msg.setData(bundle);
					//					mHandler.sendMessage(msg);
				}
				semaphore.release();
				Log.i(TAG_SEM,String.valueOf(semaphore.availablePermits())+" frei");
				Log.i(TAG_SEM,"Receive hat den semaphore zurueck gegeben");
				Log.i(TAG,"semaphore zurueck gegeben");
				status=msgBuffer[2];
				Log.i(TAG,"schreibe status:"+String.valueOf(status));

			} else {
				Log.i(TAG,"Checksum FALSCH");
				//				Message msg = mHandler.obtainMessage(MESSAGE_SET_LOG);
				//				Bundle bundle = new Bundle();
				//				bundle.putString(TOAST, "Checksum failed");
				//				msg.setData(bundle);
				//				mHandler.sendMessage(msg);
				// ankommenden nachricht war nicht korrekt uebertragen
			}
			break;
		}	
		//Log.i(TAG,"Process incoming toll");
	}
	///////////////////////////// FIRMWARE UPDATE /////////////////////////////
	/* die uploadFirmware prozedure besteht aus den teilen:
	 * 1. Die Datei des ?bergebenen Dateinamen ?ffnen und einlesen
	 * 2. Die Verbindung zum Bluetooth device zu beenden, so sie existiert
	 * 3. Die Verbindung neu aufbauen (der reconnect startet den Bootloader) mit der gleichen Ger?te id ODER den Dialog zur Auswahl des Ger?ts anzuzeigen
	 * 4. Den Bootloader fangen 
	 * 5. Die ID des Bootloaders abzufragen
	 * 6. Die Firmware aus dem HexFile in den Controller schreiben
	 */

	public int uploadFirmware(String filename,Handler mHandlerUpdate,String flashingDevice) throws IOException, InterruptedException {
		/*
		Log.i(TAG, "uploadFirmware soll laden:"+filename);
		int error=0;
		int flash2560=1;



		////// hier beginnt teil 1, die datei ?ffnen und in ein array parsen //////
		byte send[] = new byte[256*1024]; // so gro? wie es maximal werten kann, 256k
		File file = new File(filename);
		FileInputStream in = null;

		try { 								in = new FileInputStream(file);						}	 
		catch (FileNotFoundException e) { 	e.printStackTrace();	error=1; 	return error;	}

		// jetzt datei parsen
		byte hex_sentence[]=new byte[100]; // nur eine hex zeile
		int byte_read=1; // wieviel byte hab ich gelesen, voragabe muss >0 zum starten der whileschleife sein
		int file_pos=0; // pointer of position in file
		long highest_pos=0;
		int overflow=0;

		while(byte_read>0){ ///////////!!!!!!!!! DEBUG //////////
			byte_read=0;
			// eine Zeile einlesen
			byte[] one_char=new byte[1];
			boolean new_line_found=false;
			int count_byte_read=0;
			while(!new_line_found && count_byte_read<100){ // maximal 100 byte lesen, aber haupts?chlich bis umbruch
				count_byte_read+=in.read(one_char, file_pos, 1);
				if(count_byte_read>0 && count_byte_read<100){
					hex_sentence[count_byte_read-1]=one_char[0];
					if(count_byte_read>=2){
						if(hex_sentence[count_byte_read-1]==0x0a && hex_sentence[count_byte_read-2]==0x0d){
							new_line_found=true;
							byte_read=count_byte_read;
						}
					} else if(count_byte_read==0){
						count_byte_read=100;
					}
				} else if(count_byte_read<0) { // end of file reached, einlese schleife beenden
					new_line_found=false;
					byte_read=0;
					break;
				}
			};
			// jetzt haben wir ein satz gelesen, parsen wir ihn also
			if(new_line_found){
				if(hex_sentence[0]==0x3a && hex_sentence[7]==0x30 && hex_sentence[8]==0x30){
					// generate length and offset of the acutal read line
					// ascii hex to int
					// e.g. ": 10 FFF0 00 	CF D9 00 E0 10 E0 55 C6 C7 01 62 E0 76 E1 0F 94 	6A"
					for(int i=1;i<7; i++){
						if(hex_sentence[i]>='0' && hex_sentence[i]<='9'){
							hex_sentence[i]-='0';
						} else if(hex_sentence[i]>='A' && hex_sentence[i]<='F'){
							hex_sentence[i]-=('A'-10);
						}
					}
					// e.g. length = 0x10, offset = 0xFFF0
					int length=(hex_sentence[1])<<4 | (hex_sentence[2]);
					int offset=(hex_sentence[3])<<12 | (hex_sentence[4])<<8 | (hex_sentence[5])<<4 | (hex_sentence[6]);

					// run the whole length and copy the data from the sencence to the send array
					// e.g. pos=[0..16]
					for(int pos=0;pos<length;pos++){
						// to have the postion in the hex string
						// since pos = [0..length_from_header]
						// but the data are in [9..length_from_header+9] so we have to add 9
						// in addition the result byte are dobbled in the hex sentence
						// e.g. pos_in_hex_sentence=[9..2*16+9]=[9..41], pos_in_send_array=[0..16]
						int pos_in_hex_sentence=2*pos+9;
						int pos_in_send_array=pos;

						// wir sind auf jeden fall um den offset weiter 
						// und eventuell sogar noch um bis zu dreimal den overflow
						// da es nur 4 Byte f?r die adresse gibt m?ssen wir weiterz?hlen im kopf
						// e.g. pos_in_send_array=[0xFFF0 .. 0xFFF0+0x10]=[0xFFF0 .. 0x10000]=[65520 .. 65536]
						pos_in_send_array+=offset;
						pos_in_send_array+=overflow*65536;
						if(pos_in_send_array==65535 || pos_in_send_array==131071 || pos_in_send_array==196607){
							overflow++;
						}
						if(pos_in_send_array>highest_pos){ highest_pos=pos_in_send_array; };

						// safety
						if(pos_in_send_array>=256*1024 || pos_in_hex_sentence>=100 || pos_in_send_array<0|| pos_in_hex_sentence<0){
							error=2;
							return error;
						}

						// convert from ascii hex to byte hex
						// since a byte (0x00-0xff) is based on two hex values (0x0-0xf) and the hex_sentence has a byte (0x30-0x39,0x41-0x46) per ascii hex(0x0-0x9,0xA-0xF)
						// after conversion from ascii hex to hex we have to build a byte out of the the hex digits, by adding the upper hex, shifted by 4 bit to the lower hex
						// e.g. pos_in_hex_sentence=[9..41], pos_in_send_array=[65520 .. 65536]
						// e.g. pos=0 pos_in_send_array[65520]=pos_in_hex_sentence[9]<4 | pos_in_hex_sentence[10]
						// e.g. pos=1 pos_in_send_array[65521]=pos_in_hex_sentence[11]<4 | pos_in_hex_sentence[12]
						for(int i=pos_in_hex_sentence;i<=pos_in_hex_sentence+1; i++){
							if(hex_sentence[i]>='0' && hex_sentence[i]<='9'){
								hex_sentence[i]-='0';
							} else if(hex_sentence[i]>='A' && hex_sentence[i]<='F'){
								hex_sentence[i]-=('A'-10);
							}
						}		
						send[pos_in_send_array]=(byte) ((hex_sentence[pos_in_hex_sentence]&0x0F)<<4 | (hex_sentence[pos_in_hex_sentence+1]&0x0F));
					} // pos

					// send update
					if(highest_pos%15==1){ 
						// fortschritt schreiben
						Message msg2 = mHandlerUpdate.obtainMessage(SmartSpeedoMain.MESSAGE_SET_VERSION);
						Bundle bundle2 = new Bundle();
						bundle2.putInt("size", (int) highest_pos);
						bundle2.putInt("state", 1);
						msg2.setData(bundle2);
						mHandlerUpdate.sendMessage(msg2);
					}
					// fortschritt schreiben
				} // richtige hex_sequence
			} // hab eine new line
		}; // while byte_read>0
		if(highest_pos==0){ // wir haben exakt nichts gelesen, das ist eher schlecht
			Message msg = mHandlerUpdate.obtainMessage(SmartSpeedoMain.MESSAGE_SET_VERSION);
			Bundle bundle = new Bundle();
			bundle.putString("msg", "Empty Hex File found, leaving");
			bundle.putInt("state", 101);
			msg.setData(bundle);
			mHandlerUpdate.sendMessage(msg);
			Thread.sleep(3000);
			error=3;
			return error;
		}
		////// hier beginnt teil 1, die datei ?ffnen und in ein array parsen //////


		int remaining_tries=0;
		boolean connection_established=false;
		while(remaining_tries<3 && !connection_established){
			remaining_tries++;

			////// hier beginnt teil 2, die verbindung abbauen //////
			if(last_connected_device!=null && getState()!=STATE_NONE){
				stop();
				// fortschritt schreiben
				Message msg3 = mHandlerUpdate.obtainMessage(SmartSpeedoMain.MESSAGE_SET_VERSION);
				Bundle bundle = new Bundle();
				bundle.putString("msg", "Disconnecting...");
				bundle.putInt("state", 2);
				msg3.setData(bundle);
				mHandlerUpdate.sendMessage(msg3);

				Thread.sleep(2000);
			};
			////// hier beginnt teil 2, die verbindung abbauen //////


			////// hier beginnt teil 3, die verbindung aufbauen //////
			// fortschritt schreiben
			Message msg3 = mHandlerUpdate.obtainMessage(SmartSpeedoMain.MESSAGE_SET_VERSION);
			Bundle bundle = new Bundle();
			bundle.putString("msg", "Connecting to Bootload, try "+String.valueOf(remaining_tries)+"/3");
			bundle.putInt("state", 3);
			msg3.setData(bundle);
			mHandlerUpdate.sendMessage(msg3);
			// fortschritt schreiben

			BluetoothAdapter mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
			BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(flashingDevice);

			// connect bluetooth!
			if(highest_pos<=32768){ // will den kleinen flashen, 
				connect(device, false); // false => dont go to bootloader
			} else {
				connect(device, true); // true => goto bootloader
			}

			Thread.sleep(50); // to make sure, the connection has at least 50 msec do realize it is not yet connected
			int wait_time=0; // maximal 5 sekunden darauf warten das sich der state von connecting auf connected dreht
			// wait as long as the state is STATE_NONE or STATE_CONNECTING, max 5 sec
			while( getState()!=STATE_CONNECTED_AND_SEARCHING && wait_time<5000){
				Thread.sleep(1);
				wait_time++;
			}
			// now the state is STATE_CONNECTED_AND_SEARCHING or STATE_CONNECTED or STATE_NONE
			// it MUST be STATE_CONNECTED_AND_SEARCHING or STATE_CONNECTED to continue normal
			if(getState()!=STATE_CONNECTED_AND_SEARCHING && getState()!=STATE_CONNECTED){
				error=4;

				Message msg4 = mHandlerUpdate.obtainMessage(SmartSpeedoMain.MESSAGE_SET_VERSION);
				Bundle bundle4 = new Bundle();
				bundle4.putString("msg", "Connection not established");
				bundle4.putInt("state", 102);
				msg4.setData(bundle4);
				mHandlerUpdate.sendMessage(msg4);

				//return error;
			} else {
				connection_established=true;

				Message msg4 = mHandlerUpdate.obtainMessage(SmartSpeedoMain.MESSAGE_SET_VERSION);
				Bundle bundle4 = new Bundle();
				bundle4.putString("msg", "Connection established");
				bundle4.putInt("state", 3);
				msg4.setData(bundle4);
				mHandlerUpdate.sendMessage(msg4);

//				* jetzt muessen wir uns ueberlegen:
//				 * ist das File maximal 32k gross ? 
//				 * -> wenn ja: ATm2560
//				 * --> Dann muessen wir in den Bootloader
//				 * --> Connection abbauen und neu aufbauen
//				 * --> dann STK500v2 
//				 * -> wenn nein: ATm328
//				 * --> Dann brauchen wir nur ein bestimmtes
//				 * --> Command schicken, der ATm2560 resettet
//				 * --> den ATm328 und tunnelt den Transfer
//				 *
				if(highest_pos<=32768){
					// wir flashen einen ATm328
					flash2560=0;
					// fortschritt schreiben
					Message msg5 = mHandlerUpdate.obtainMessage(SmartSpeedoMain.MESSAGE_SET_VERSION);
					Bundle bundle5 = new Bundle();

					bundle5.putString("msg", "Sending reset to AT328");
					bundle5.putInt("state", 4);
					msg5.setData(bundle5);
					mHandlerUpdate.sendMessage(msg5);

					// wait till connection is there
					wait_time=0; // maximal 5 sekunden darauf warten das sich der state von searching auf connected dreht
					while( getState()!=STATE_CONNECTED && wait_time<5000){
						Thread.sleep(1);
						wait_time++;
					} 
					if(getState()!=STATE_CONNECTED){
						error=4;
						msg4 = mHandlerUpdate.obtainMessage(SmartSpeedoMain.MESSAGE_SET_VERSION);
						bundle4 = new Bundle();
						bundle4.putString("msg", "Connection not established");
						bundle4.putInt("state", 102);
						msg4.setData(bundle4);
						mHandlerUpdate.sendMessage(msg4);
					}

					reset_seq(); // speedoino starts new, so we have to reset our seq as well
					byte send2[] = new byte[300];
					send2[0] = CMD_RESET_SMALL_AVR;
					Log.i("connect","sign_on sendet");
					Log.i("SEND","upload_fileware0()");
					if(send_save(send2, 1, 750, 15)==0){ // 750ms timeout, 15 retries, das hier wartet nochmal bis zu ~10sec
						// testing!!
						if(msgBuffer[1]!=STATUS_CMD_OK){ //!! BUG !! der Tacho geht zuerst in den Reset und w�rde danach erst anworten, daher wird hier nix zur�ck kommen :(
							error=401;

							// fortschritt schreiben
							Bundle bundle9 = new Bundle();
							Message msg9 = mHandlerUpdate.obtainMessage(SmartSpeedoMain.MESSAGE_SET_VERSION);
							bundle9.putString("msg", "Invalid response, quitting");
							bundle9.putInt("state",
									55);
							msg9.setData(bundle9);
							mHandlerUpdate.sendMessage(msg9);
							Thread.sleep(3000);
							// fortschritt schreiben

							return error;
						};
						setState(STATE_CONNECTED);
						// testing!!
						Thread.sleep(10); // wait 10 ms
						Log.i(TAG_LOGIN,"reset done");
					} else {
						error=41;

						// fortschritt schreiben
						Bundle bundle9 = new Bundle();
						Message msg9 = mHandlerUpdate.obtainMessage(SmartSpeedoMain.MESSAGE_SET_VERSION);
						bundle9.putString("msg", "No response receive, quitting");
						bundle9.putInt("state", 55);
						msg9.setData(bundle9);
						mHandlerUpdate.sendMessage(msg9);
						Thread.sleep(3000);
						// fortschritt schreiben

						return error;
					}
				};
				////// hier beginnt teil 3, die verbindung aufbauen //////
			} 	
		}

		if(!connection_established){
			// fortschritt schreiben
			Message msg5 = mHandlerUpdate.obtainMessage(SmartSpeedoMain.MESSAGE_SET_VERSION);
			Bundle bundle5 = new Bundle();
			bundle5.putString("msg", "Could no establish connection");
			bundle5.putInt("state", 41);
			msg5.setData(bundle5);
			mHandlerUpdate.sendMessage(msg5);
			Thread.sleep(10000);
			return -1;
		}

		////// hier beginnt teil 4, Den Bootloader fangen  //////
		// fortschritt schreiben
		Message msg3 = mHandlerUpdate.obtainMessage(SmartSpeedoMain.MESSAGE_SET_VERSION);
		Bundle bundle3 = new Bundle();
		bundle3.putString("msg", "Sending sign on");
		bundle3.putInt("state", 5);
		msg3.setData(bundle3);
		mHandlerUpdate.sendMessage(msg3);
		// fortschritt schreiben
		// jetzt stk500v2 kompatibel die versionsnummer abfragen
		byte send2[] = new byte[300];
		send2[0] = CMD_SIGN_ON;
		Log.i("connect","sign_on sendet");
		Log.i("SEND","upload_fileware1()");
		if(send_save(send2,1, 750, 15)==0){ // 750ms timeout, 15 retries
			if(msgBuffer[1]!=STATUS_CMD_OK){
				error=50;

				// fortschritt schreiben
				Bundle bundle9 = new Bundle();
				Message msg9 = mHandlerUpdate.obtainMessage(SmartSpeedoMain.MESSAGE_SET_VERSION);
				bundle9.putString("msg", "Unknown response, quitting");
				bundle9.putInt("state", 55);
				msg9.setData(bundle9);
				mHandlerUpdate.sendMessage(msg9);
				Thread.sleep(3000);
				// fortschritt schreiben

				return error;
			};
			setState(STATE_CONNECTED); // jep, we went to bootloader, so we need to ask on our own if there is a connection, if so, switch state here
			Thread.sleep(10); // wait 10 ms
			Log.i(TAG_LOGIN,"sign_on done");
			// fortschritt schreiben
			Bundle bundle = new Bundle();
			Message msg = mHandlerUpdate.obtainMessage(SmartSpeedoMain.MESSAGE_SET_VERSION);
			bundle.putString("msg", "Sign on done");
			bundle.putInt("state", 5);
			msg.setData(bundle);
			mHandlerUpdate.sendMessage(msg);
			// fortschritt schreiben
		} else {
			error=5;

			// fortschritt schreiben
			Bundle bundle9 = new Bundle();
			Message msg9 = mHandlerUpdate.obtainMessage(SmartSpeedoMain.MESSAGE_SET_VERSION);
			bundle9.putString("msg", "No response, quitting");
			bundle9.putInt("state", 55);
			msg9.setData(bundle9);
			mHandlerUpdate.sendMessage(msg9);
			Thread.sleep(3000);
			// fortschritt schreiben

			return error;
		}
		////// hier beginnt teil 4, Den Bootloader fangen  //////


		////// hier beginnt teil 5, Die ID des Bootloaders abzufragen  //////	
		boolean correct_id_found=false;
		int retries=0;
		while(retries<3 && !correct_id_found){
			retries++;

			// fortschritt schreiben
			Bundle bundle9 = new Bundle();
			Message msg9 = mHandlerUpdate.obtainMessage(SmartSpeedoMain.MESSAGE_SET_VERSION);
			bundle9.putString("msg", "Requesting ID, try "+String.valueOf(retries)+"/3");
			bundle9.putInt("state", 44);
			msg9.setData(bundle9);
			mHandlerUpdate.sendMessage(msg9);
			// fortschritt schreiben

			int prozessor_id=0;
			send2[0]=CMD_SPI_MULTI;
			send2[4]=0x30;
			send2[6]=0x00;
			Log.i("SEND","upload_fileware2()");
			if(send_save(send2,7,1000,3)==0){
				if(msgBuffer[1]!=STATUS_CMD_OK){
					error=601;

					// fortschritt schreiben
					Bundle bundle39 = new Bundle();
					Message msg39 = mHandlerUpdate.obtainMessage(SmartSpeedoMain.MESSAGE_SET_VERSION);
					bundle39.putString("msg", "Unknown response, quitting");
					bundle39.putInt("state", 55);
					msg39.setData(bundle39);
					mHandlerUpdate.sendMessage(msg39);
					Thread.sleep(3000);
					// fortschritt schreiben

					return error;
				};
				Log.i(TAG_LOGIN,"get_sig 0/2 done");
				prozessor_id|=((msgBuffer[5]&0xff)<<16);
				Log.i(TAG_LOGIN,"Prozessor: "+String.valueOf(prozessor_id));
				send2[6]=0x01;
				Log.i("SEND","upload_fileware3()");
				if(send_save(send2,7,1000,3)==0){
					if(msgBuffer[1]!=STATUS_CMD_OK){
						error=602;

						// fortschritt schreiben
						Bundle bundle19 = new Bundle();
						Message msg19 = mHandlerUpdate.obtainMessage(SmartSpeedoMain.MESSAGE_SET_VERSION);
						bundle19.putString("msg", "Unknown response, quitting");
						bundle19.putInt("state", 55);
						msg19.setData(bundle9);
						mHandlerUpdate.sendMessage(msg19);
						Thread.sleep(3000);
						// fortschritt schreiben

						return error;
					};
					Log.i(TAG_LOGIN,"get_sig 1/2 done");
					prozessor_id|=((msgBuffer[5]&0xff)<<8);
					Log.i(TAG_LOGIN,"Prozessor: "+String.valueOf(prozessor_id));
					send2[6]=0x02;
					Log.i("SEND","upload_fileware4()");
					if(send_save(send2,7,1000,3)==0){
						if(msgBuffer[1]!=STATUS_CMD_OK){
							error=603;

							// fortschritt schreiben
							Bundle bundle29 = new Bundle();
							Message msg29 = mHandlerUpdate.obtainMessage(SmartSpeedoMain.MESSAGE_SET_VERSION);
							bundle29.putString("msg", "Unknown response, quitting");
							bundle29.putInt("state", 55);
							msg29.setData(bundle9);
							mHandlerUpdate.sendMessage(msg29);
							Thread.sleep(3000);
							// fortschritt schreiben

							return error;
						};
						Log.i(TAG_LOGIN,"get_sig 2/2 done");
						prozessor_id|=((msgBuffer[5]&0xff));
						Log.i(TAG_LOGIN,"Prozessor: "+String.valueOf(prozessor_id));
					}
				}
			}
			if((prozessor_id==0x1E9801 && flash2560==1)||(prozessor_id==0x1E950F && flash2560==0)){ // ATm2560 Signature 2004993, oder ATm328 Signature 2004244

				correct_id_found=true;

				// fortschritt schreiben
				Bundle bundle8 = new Bundle();
				Message msg8 = mHandlerUpdate.obtainMessage(SmartSpeedoMain.MESSAGE_SET_VERSION);
				if(prozessor_id==0x1E9801){
					bundle8.putString("msg", "ATm2560 ID found");
				} else {
					bundle8.putString("msg", "ATm328 ID found");
				}
				bundle8.putInt("state", 44);
				msg8.setData(bundle8);
				mHandlerUpdate.sendMessage(msg8);
				// fortschritt schreiben
			} else {
				error=6;
			};
		};
		////// hier beginnt teil 5, Die ID des Bootloaders abzufragen  //////


		////// hier beginnt teil 6, Die Firmware aus dem HexFile in den Controller schreiben //////
		send2[0]=CMD_CHIP_ERASE_ISP;
		send2[1]=0x09;
		send2[2]=0x00;
		send2[3]=(byte) 0xAC;
		send2[4]=(byte) 0x80;
		send2[5]=0x00;
		send2[6]=0x00;
		if(send_save(send2,7,3000,1)!=0){
			return 61;
		}
		if(msgBuffer[1]!=STATUS_CMD_OK){
			error=611;
			return error;
		};

		send2[0]=CMD_LOAD_ADDRESS;
		send2[1]=(byte) 0x80; //warum denn 0x80 ? ist wahrscheinlich egal oder?
		send2[2]=0x00;
		send2[3]=0x00;
		send2[4]=0x00;
		send2[5]=(byte) 0x8f;
		if(send_save(send2,6,3000,1)!=0){
			return 62;
		}
		if(msgBuffer[1]!=STATUS_CMD_OK){
			error=621;
			return error;
		};

		send2[0]=CMD_PROGRAM_FLASH_ISP;
		int page_size=256;
		if(flash2560==0) page_size=128; //ATm328 has a smaller page size!!

		//get current date
		int time = (int) (System.currentTimeMillis());
		float speed_flat=1600;
		for(int send_position=0;send_position<=highest_pos;send_position+=page_size){
			int max_size_to_send=(int) (highest_pos-send_position+1); // bei 100byte im hex file -> highes pos=99 -> wenn send_position=99, dann m?ssen wir sehr wohl das 99te Byte noch senden
			if(max_size_to_send>page_size) max_size_to_send=page_size; // send page_size byte bursts, due to the limited size of input buffer in the atmega (285=256+overhead)
			// store length in telegramm
			send2[1]=(byte) ((max_size_to_send&0x0000ff00)>>8);// high byte
			send2[2]=(byte)((max_size_to_send)&0x000000ff);// low byte

			// prepare telegramm 
			for(int msgPosition=0; msgPosition<max_size_to_send; msgPosition++){
				send2[10+msgPosition]=send[send_position+msgPosition];
			}

			// send telegramm and check response
			Log.i("SEND","upload_fileware5()");
			if(send_save(send2,max_size_to_send+10,3000,1)==0){
				if(msgBuffer[0]!=CMD_PROGRAM_FLASH_ISP || msgBuffer[1]!=STATUS_CMD_OK){
					error=7;

					// fortschritt schreiben
					Bundle bundle9 = new Bundle();
					Message msg9 = mHandlerUpdate.obtainMessage(SmartSpeedoMain.MESSAGE_SET_VERSION);
					bundle9.putString("msg", "Unknown response, quitting");
					bundle9.putInt("state", 55);
					msg9.setData(bundle9);
					mHandlerUpdate.sendMessage(msg9);
					Thread.sleep(3000);
					// fortschritt schreiben

					return error;
				};
				msgBuffer[0]=0xff; //clear
				msgBuffer[1]=0xff; //clear
			} else {
				error=8;

				// fortschritt schreiben
				Bundle bundle49 = new Bundle();
				Message msg49 = mHandlerUpdate.obtainMessage(SmartSpeedoMain.MESSAGE_SET_VERSION);
				bundle49.putString("msg", "No response, quitting");
				bundle49.putInt("state", 55);
				msg49.setData(bundle49);
				mHandlerUpdate.sendMessage(msg49);
				Thread.sleep(3000);
				// fortschritt schreiben

				return error;
			};

			// fortschritt schreiben
			String full_message=get_speed_text(time, highest_pos, send_position, 0);

			Bundle bundle = new Bundle();
			Message msg = mHandlerUpdate.obtainMessage(SmartSpeedoMain.MESSAGE_SET_VERSION);
			bundle.putString("msg", full_message);
			bundle.putInt("state", 7);
			msg.setData(bundle);
			mHandlerUpdate.sendMessage(msg);
			// fortschritt schreiben

		}

		SharedPreferences settings = mContext.getSharedPreferences(PREFS_NAME, 0);
		Boolean verify_active =settings.getBoolean("verify_active",true);
		if(verify_active){
			// Verify
			send2[0]=CMD_LOAD_ADDRESS;
			send2[1]=(byte) 0x80; //warum denn 0x80 ? ist wahrscheinlich egal oder?
			send2[2]=0x00;
			send2[3]=0x00;
			send2[4]=0x00;
			send2[5]=(byte) 0x8f;
			if(send_save(send2,6,3000,1)!=0){
				return 62;
			}
			if(msgBuffer[1]!=STATUS_CMD_OK){
				error=721;
				return error;
			};

			int read_time = (int) (System.currentTimeMillis());
			float read_speed_flat=1600;
			for(int page=0; page<=highest_pos/256; page++){ // todo <=
				int size=256;
				if(page==highest_pos/256){
					size=(int)(highest_pos%256);
					if(size%2==1) size--;// word oriented
				};
				// send read command
				send2[0]=CMD_READ_FLASH_ISP;
				send2[1]=(byte)((byte)(size>>8)&0xff);  // 0x20
				send2[2]=(byte)(size&0xff);				// 0x00
				if(send_save(send2,3,3000,1)!=0){
					return 73;
				}
				if(msgBuffer[1]!=STATUS_CMD_OK){
					error=731;
					return error;
				};

				for(int rel_flash_pos=0; rel_flash_pos<size; rel_flash_pos+=2){ // in [0] steht das command und in [1] der status, danach die Daten
					int flash_word=page*256+rel_flash_pos;
					byte controllerbyte_low=(byte)msgBuffer[rel_flash_pos+2];
					byte controllerbyte_high=(byte)msgBuffer[rel_flash_pos+1+2];
					byte hexbyte_low=send[flash_word];
					byte hexbyte_high=send[flash_word+1];
					if(controllerbyte_high!=hexbyte_high || controllerbyte_low!=hexbyte_low){
						error=741;
						return error;
					}
				}
				// fortschritt schreiben
				String full_message=get_speed_text(time, highest_pos, (page*256), 3);

				Bundle bundle = new Bundle();
				Message msg = mHandlerUpdate.obtainMessage(SmartSpeedoMain.MESSAGE_SET_VERSION);
				bundle.putString("msg", full_message);
				bundle.putInt("state", 8);
				msg.setData(bundle);
				mHandlerUpdate.sendMessage(msg);
				// fortschritt schreiben
			}
			// Verify
		}

		// send leave_programmer command
		send2[0]=CMD_LEAVE_PROGMODE_ISP;
		Log.i("SEND","upload_fileware6()");
		send_save(send2,1,1000,1);	
		////// hier beginnt teil 6, Die Firmware aus dem HexFile in den Controller schreiben ////// 
		// fortschritt schreiben
		Bundle bundle37 = new Bundle();
		Message msg37 = mHandlerUpdate.obtainMessage(SmartSpeedoMain.MESSAGE_SET_VERSION);
		bundle37.putString("msg", "Thanks for using");
		bundle37.putInt("state", 97);
		msg37.setData(bundle37);
		stop();
		mHandlerUpdate.sendMessage(msg37);
		Thread.sleep(10000);

		 */
		return 0;
	}

	///////////////////////////// FIRMWARE UPDATE /////////////////////////////

	///////////////////////////// get_speed_text /////////////////////////////
	public String get_speed_text(int start_time, long filesize,long transfered_bytes, int transfere_type){
		// fortschritt schreiben
		// calculate speed
		int time_diff= ((int)System.currentTimeMillis()-start_time)/1000;
		if(time_diff==0) time_diff=1;
		int read_speed=(int) (transfered_bytes/time_diff);
		transfere_speed_flat=(19*transfere_speed_flat+read_speed)/20;
		String speed_filled=String.valueOf(Math.floor(transfere_speed_flat/10)/100);
		while(speed_filled.substring(speed_filled.lastIndexOf('.')+1).length()<2){
			speed_filled+="0";
		}
		// calculate remaining time
		if(transfere_speed_flat==0) transfere_speed_flat=1600;
		int time_left=(int) ((filesize-transfered_bytes)/transfere_speed_flat);
		String std_left=String.valueOf((int)Math.floor(time_left/3600));
		if(std_left.length()<2) std_left="0"+std_left;
		String min_left=String.valueOf((int)Math.floor(time_left/60));
		if(min_left.length()<2) min_left="0"+min_left;
		String sec_left=String.valueOf(time_left%60);
		if(sec_left.length()<2) sec_left="0"+sec_left;
		// calculate amount
		String amount=String.valueOf(Math.floor(transfered_bytes/100)/10)+"/"+String.valueOf(Math.floor(filesize/100)/10);
		String prozent=String.valueOf((int)(transfered_bytes*100)/filesize);

		// build message
		String full_message="";
		if(transfere_type==0){
			full_message="Speedoino at "+prozent+"%\n";
			full_message+="    Total "+amount+" KB\n";
			full_message+="    Upload @ "+speed_filled+" KB/sec\n";
			full_message+="    Remaining "+std_left+":"+min_left+":"+sec_left;
		} else if(transfere_type==3){
			full_message="Speedoino "+prozent+"% verified\n";
			full_message+="    Total "+amount+" KB\n";
			full_message+="    Download @ "+speed_filled+" KB/sec\n";
			full_message+="    Remaining "+std_left+":"+min_left+":"+sec_left;
		} else {
			if(transfere_type==1){
				full_message="File "+prozent+"% downloaded\n";
			} else if(transfere_type==2){
				full_message="File "+prozent+"% uploaded\n";
			}
			full_message+="Total "+amount+" KB\n";
			full_message+="Transfere @ "+speed_filled+" KB/sec\n";
			full_message+="Remaining "+std_left+":"+min_left+":"+sec_left;
		}
		return full_message;
		// fortschritt schreiben
	}

}