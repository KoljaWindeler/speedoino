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

package de.windeler.kolja;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;
import java.util.concurrent.Semaphore;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.widget.Toast;

/**
 * This class does all the work for setting up and managing Bluetooth
 * connections with other devices. It has a thread that listens for
 * incoming connections, a thread for connecting with a device, and a
 * thread for performing data transmissions when connected.
 */
public class BluetoothSerialService {
	// Debugging
	private static final String TAG = "JKW";
	private static final String TAG_SEM = "JKW_SEM";
	private static final String TAG_RECV = "JKW_RECV";
	private static final String TAG_TIME = "JKW_TIME";
	private static final String TAG_BT = "BT";
	private static final String TAG_LOGIN = "LOGIN";
	private static final boolean D = true;

	private final Semaphore semaphore = new Semaphore(1, true);
	private char status=1;
	private int seqNum = 0;
	private long lastSend = System.currentTimeMillis();
	private int 	rx_tx_state	= ST_IDLE;
	private int		msgLength	= 0;
	private char	checksum	= 0;
	private char	msgBuffer[] = new char[300];
	private int		ii			= 0;
	private Handler mTimerHandle = new Handler();
	private boolean silent=false;
	public int item=0;
	private String last_dir_received="-";

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


	private static final UUID SerialPortServiceClass_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

	public BluetoothDevice last_connected_device=null;

	// Member fields
	private final BluetoothAdapter mAdapter;
	private final Handler mHandler;
	private Context mContext;
	private ConnectThread mConnectThread;
	private ConnectedThread mConnectedThread;
	private int mState;

	// preamble sucher
	private int finished_counter=0;
	private boolean preamble_found=false; 

	//private EmulatorView mEmulatorView;

	// Constants that indicate the current connection state
	public static final int STATE_NONE = 0;       // we're doing nothing
	public static final int STATE_CONNECTING = 2; // now initiating an outgoing connection
	public static final int STATE_CONNECTED = 3;  // now connected to a remote device
	public static final int STATE_CONNECTED_AND_SEARCHING = 4;  // now connected to a remote device

	public static final byte MESSAGE_START 			=  0x1B;
	public static final byte TOKEN					=  0x0E;

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
	public static final byte CMD_SIGN_ON_FIRMWARE		=  0x41;


	public static final char STATUS_CMD_OK      =  0x00;
	public static final char STATUS_CMD_FAILED  =  0xC0;
	public static final char STATUS_CKSUM_ERROR =  0xC1;
	public static final char STATUS_CMD_UNKNOWN =  0xC9;
	public static final char STATUS_EOF		   =  0x10;

	/**
	 * Constructor. Prepares a new BluetoothChat session.
	 * @param context  The UI Activity Context
	 * @param handler  A Handler to send messages back to the UI Activity
	 */
	public BluetoothSerialService(Context context, Handler handler) {
		mAdapter = BluetoothAdapter.getDefaultAdapter();
		mState = STATE_NONE;
		mHandler = handler;
		mContext = context;
	}

	/**
	 * Set the current state of the chat connection
	 * @param state  An integer defining the current connection state
	 */
	public synchronized void setState(int state) {
		Log.i(TAG,"Setting state to "+String.valueOf(state));
		mState = state;
		// Give the new state to the Handler so the UI Activity can update
		mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_STATE_CHANGE, state, -1).sendToTarget();
		if(state == STATE_NONE){
			preamble_found=false;
		} else if(state == STATE_CONNECTED){
			rx_tx_state	= ST_IDLE;
		};

	}

	/**
	 * Return the current connection state. */
	public synchronized int getState() {
		return mState;
	}

	/**
	 * Start the chat service. Specifically start AcceptThread to begin a
	 * session in listening (server) mode. Called by the Activity onResume() */
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
		// Cancel any thread attempting to make a connection
		if (mState == STATE_CONNECTING) {
			if (mConnectThread != null) {mConnectThread.cancel(); mConnectThread = null;}
		}

		// Cancel any thread currently running a connection
		if (mConnectedThread != null) {mConnectedThread.cancel(); mConnectedThread = null;}



		// Start the thread to connect with the given device
		Log.e("connect","start thread to connect...");
		last_connected_device=device;
		mConnectThread = new ConnectThread(device);
		Log.i("connect","starte jetzt mConnectedThread");
		mConnectThread.setGotoBootloader(goto_bootloader);
		mConnectThread.start();
		Log.i("connect","mConnectedThread ist durch");
		setState(STATE_CONNECTING);
	}

	/**
	 * Start the ConnectedThread to begin managing a Bluetooth connection
	 * @param socket  The BluetoothSocket on which the connection was made
	 * @param device  The BluetoothDevice that has been connected
	 * @throws InterruptedException 
	 */
	public synchronized void connected(BluetoothSocket socket, BluetoothDevice device, boolean goto_bootloader) throws InterruptedException {
		Log.i(TAG, "connected");

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
		mConnectedThread = new ConnectedThread(socket);
		Log.i(TAG,"starte jetzt mConnectedThread");
		mConnectedThread.start();
		Log.i(TAG,"mConnectedThread start fertig");

		// Send the name of the connected device back to the UI Activity
		Message msg = mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_DEVICE_NAME);
		Bundle bundle = new Bundle();
		bundle.putString(SpeedoAndroidActivity.DEVICE_NAME, device.getName());
		msg.setData(bundle);
		mHandler.sendMessage(msg);

		setState(STATE_CONNECTED_AND_SEARCHING);

		// if we don't need the bootloader, lets connect right here to the 
		// firmware and set the State to connected 
		if(!goto_bootloader){
			//wait 5 sec to prevent goint to bootloader
			try {
				Thread.sleep(2000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

			// check if connected
			silent=true; // kein popup, da wir durchaus mal 1...2... timeouts abwarten müssen für den bootloader
			byte send[] = new byte[1];
			send[0] = CMD_SIGN_ON_FIRMWARE;
			if(send_save(send,1,400,30)==0){
				setState(STATE_CONNECTED);
			} else {
				setState(STATE_NONE);
			}
			silent=false;
		}
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
			//if (mState != STATE_CONNECTED) return;
			int retries=100;
			while(mConnectedThread==null && retries>0){
				retries--;
				try {
					Thread.sleep(1);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
			if(mConnectedThread==null){
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

		// Send a failure message back to the Activity
		Message msg = mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_TOAST);
		Bundle bundle = new Bundle();
		bundle.putString(SpeedoAndroidActivity.TOAST, "Unable to connect device");
		msg.setData(bundle);
		mHandler.sendMessage(msg);

		BluetoothSerialService.this.start();
	}

	/**
	 * Indicate that the connection was lost and notify the UI Activity.
	 */
	//	private void connectionLost() {
	//		setState(STATE_NONE);
	//		BluetoothSerialService.this.start();
	//	}

	/**
	 * This thread runs while attempting to make an outgoing connection
	 * with a device. It runs straight through; the connection either
	 * succeeds or fails.
	 */
	private class ConnectThread extends Thread {
		private final BluetoothSocket mmSocket;
		private final BluetoothDevice mmDevice;
		private boolean goto_bootloader=false;

		public ConnectThread(BluetoothDevice device) {
			mmDevice = device;
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
			synchronized (BluetoothSerialService.this) {
				mConnectThread = null;
			}

			// Start the connected thread
			try {
				connected(mmSocket, mmDevice, goto_bootloader);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
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

			String temp= new String("finished");
			char finished_str[]= new char[temp.length()+1];
			temp.getChars(0, temp.length(), finished_str, 0);

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

				// Share the sent message back to the UI Activity
				//mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_WRITE, buffer.length, -1, buffer).sendToTarget();
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
		seqNum=0;
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
				// TODO Auto-generated catch block
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
				// TODO Auto-generated catch block
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
			Message msg = mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_TOAST);
			Bundle bundle = new Bundle();
			bundle.putString(SpeedoAndroidActivity.TOAST, "You are not connected to a Speedoino");
			bundle.putInt(SpeedoAndroidActivity.result, -1);
			msg.setData(bundle);
			mHandler.sendMessage(msg);

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
				//Log.d(TAG_RECV,"BTsend:"+String.valueOf(((int)p)&0x00ff)+"/"+String.valueOf((char)p)+"   /DATA "+String.valueOf(i+1)+"/"+String.valueOf(msgLength));
			}
			write(data,msgLength);
			write(checksum,1);	//	CHECKSUM
			Log.i(TAG_TIME,"checksum send "+String.valueOf(msgLength));
			Log.d(TAG_RECV,"BTsend:"+String.valueOf((int)checksum[0])+" /Checksum");
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
					Message msg = mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_TOAST);
					Bundle bundle = new Bundle();
					bundle.putString(SpeedoAndroidActivity.TOAST, "No response from target");
					msg.setData(bundle);
					mHandler.sendMessage(msg);
				}
				semaphore.release(); // als letztes !!
			};
		}
	};

	private void process_incoming(char data) {
		Log.i(TAG,"process_incoming gestartet mit:"+String.valueOf((int)(data&0x00ff))+"/"+((char)(data&0x00ff))+" rx_state:"+String.valueOf((int)rx_tx_state));			
		switch(rx_tx_state){
		case ST_START:
			if ( data == MESSAGE_START){
				Log.i(TAG,"Message start erhalten");
				Log.i(TAG_TIME,"MSG_START recv");
				rx_tx_state	=	ST_GET_SEQ_NUM;
				checksum	=	data;
			}
			break;

		case ST_GET_SEQ_NUM:
			if ( (int)data == 1 || (data&0xff) == (seqNum&0xff) ){
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
			} else {
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
					mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_CMD_OK, 0, -1).sendToTarget();
				} else if(msgBuffer[1]==STATUS_CMD_FAILED) {
					mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_CMD_FAILED, 0, -1).sendToTarget();
				} else if(msgBuffer[1]==STATUS_CMD_UNKNOWN) {
					mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_CMD_UNKNOWN, 0, -1).sendToTarget();
				}

				Message msg;
				Bundle bundle;

				switch((msgBuffer[0])){
				case CMD_SIGN_ON:
				case CMD_SIGN_ON_FIRMWARE:
					// hier jetzt in unsere oberflche die id eintragen
					if((msgBuffer[1] & 0xff)==STATUS_CMD_OK){
						String str = new String(msgBuffer);

						msg = mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
						bundle = new Bundle();
						bundle.putString(SpeedoAndroidActivity.TOAST, str.substring(2,msgLength));
						msg.setData(bundle);
						mHandler.sendMessage(msg); 
						Log.i(TAG,"statemachine ok, gebe semaphore zurueck");

					} else {
						// irgendwie das command nochmal senden
					}
					break;
					// da alle richtungen zwar betaetigt werden, danach die schleife auf dem AVR aber unterbrochen wird -> seqNr resetten
				case CMD_GO_LEFT:
					msg = mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_LOG);
					bundle = new Bundle();
					bundle.putString(SpeedoAndroidActivity.TOAST, "go_left OK");
					msg.setData(bundle);
					mHandler.sendMessage(msg); 

					reset_seq();
					break;
				case CMD_GO_RIGHT:
					msg = mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_LOG);
					bundle = new Bundle();
					bundle.putString(SpeedoAndroidActivity.TOAST, "go_right OK");
					msg.setData(bundle);
					mHandler.sendMessage(msg); 

					reset_seq();
					break;
				case CMD_GO_UP:
					msg = mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_LOG);
					bundle = new Bundle();
					bundle.putString(SpeedoAndroidActivity.TOAST, "go_up OK");
					msg.setData(bundle);
					mHandler.sendMessage(msg);

					reset_seq();
					break;
				case CMD_GO_DOWN:
					msg = mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_LOG);
					bundle = new Bundle();
					bundle.putString(SpeedoAndroidActivity.TOAST, "go_down OK");
					msg.setData(bundle);
					mHandler.sendMessage(msg);

					reset_seq();
					break;

					// list of valid commands, the active task will process the data
				case CMD_DIR: 		// durch einen doofen zufall kann das hier auch LEAVE_PRGM_MODE sein, dann ist die l?nge aber nur 2, bei dem dir ist sie >2 daher kann man das unterscheiden
				case CMD_GET_FILE: 	// nothing to do, just keep it in buffer, get_file() will care for it.
				case CMD_PUT_FILE: 	// this is the same like CMD_PROGRAM_FLASH_ISP 	// nothing to do, just keep it in buffer, put_file() or fireware() will care for it. // depending on who asked for this
				case CMD_SPI_MULTI:
				case CMD_LEAVE_FM:
				case CMD_LOAD_ADDRESS:
				case CMD_LEAVE_PROGMODE_ISP:
				case CMD_CHIP_ERASE_ISP:
				case CMD_PROGRAM_FLASH_ISP:
				case CMD_DEL_FILE:
				case CMD_SHOW_GFX:
				case CMD_FILE_RECEIVE:
				case CMD_RESET_SMALL_AVR:
					break;

				default:
					Log.i(TAG,"unknown command received:"+String.valueOf(msgBuffer[0]));
					// irgendwie das commando nochmal senden
					msg = mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_TOAST);
					bundle = new Bundle();
					bundle.putString(SpeedoAndroidActivity.TOAST, "unknown command from speedo received");
					msg.setData(bundle);
					mHandler.sendMessage(msg);
					break;
				}
				semaphore.release();
				Log.i(TAG_SEM,String.valueOf(semaphore.availablePermits())+" frei");
				Log.i(TAG_SEM,"Receive hat den semaphore zurueck gegeben");
				Log.i(TAG,"semaphore zurueck gegeben");
				status=msgBuffer[2];
				Log.i(TAG,"schreibe status:"+String.valueOf(status));

			} else {
				Log.i(TAG,"Checksum FALSCH");
				Message msg = mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_LOG);
				Bundle bundle = new Bundle();
				bundle.putString(SpeedoAndroidActivity.TOAST, "Checksum failed");
				msg.setData(bundle);
				mHandler.sendMessage(msg);
				// ankommenden nachricht war nicht korrekt uebertragen
			}
			break;
		}	
		Log.i(TAG,"Process incoming toll");
	}

	public int getDir(String dir, Handler mHandlerUpdate) throws InterruptedException{
		item=0;
		byte send[] = new byte[dir.length()+2+1]; // 2 fuer length of item + name + command
		status=1;

		// prepare static part
		send[0]=CMD_DIR;
		for(int i=0;i<dir.length();i++){
			send[i+3]=(byte)dir.charAt(i);
		}

		while(status!=STATUS_EOF){
			// prepare dynamic part
			send[1]=(byte) ((item & 0xff00)>>8); //danger wegen signed ? interessant ab ueber 127 Files
			send[2]=(byte) (item & 0x00ff);
			item++;
			// send setzt jetzt den semaphore und erst 
			// receive gibt ihn wieder her
			Log.e(TAG_BT,"Vor dem Send "+String.valueOf(item));
			Log.i(TAG,"derzeit sind "+String.valueOf(semaphore.availablePermits())+" semaphore frei");
			Log.i("SEND","getdir()");
			int send_value=send(send, send.length);
			Log.e(TAG_BT,"Hinter dem Send");

			if(send_value>0){
				mTimerHandle.removeCallbacks(mCheckResponseTimeTask);
				semaphore.release();
				Log.i(TAG_SEM,String.valueOf(semaphore.availablePermits())+" frei");
				Log.i(TAG_SEM,"Send failed hat den semaphore zurueck gegeben");
				return send_value;
			}

			// wait here until we can get the semaphore
			// im schlimmsten fall hier ein while auf ne globale variable
			Log.i(TAG_SEM,"get dir wartet");
			Log.i(TAG_SEM,String.valueOf(semaphore.availablePermits())+" frei");
			semaphore.acquire();
			Log.i(TAG_SEM,"get dir hat den semaphore von bekommen und geht in die naechste runde");
			semaphore.release();
			Log.i(TAG_SEM,String.valueOf(semaphore.availablePermits())+" frei");
			Log.i(TAG_SEM,"get dir hat den semaphore zurueck gegeben");

			// auswerten der Ergebnisse
			String str = new String(msgBuffer);
			if(msgLength>2){
				Log.i(TAG,"CMD Dir erhalten:"+str.substring(3,msgLength)+" type "+String.valueOf((int)msgBuffer[2]));
				Message msg = mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_DIR_APPEND);
				Bundle bundle = new Bundle();
				bundle.putInt("type", (int)msgBuffer[2]);
				if(msgLength>5){
					Log.i(TAG,"size: [3]="+String.valueOf((int)msgBuffer[3]&0xff)+" [4]="+String.valueOf((int)msgBuffer[4]&0xff)+" [5]="+String.valueOf((int)msgBuffer[5]&0xff)+" [6]="+String.valueOf((int)msgBuffer[6]&0xff));
					long size=0;
					for (int i = 3; i < 7; i++)	{
						size = (size << 8) + (msgBuffer[i] & 0xff);
					}
					Log.i(TAG,"size="+String.valueOf(size));
					bundle.putLong("size", size);
					bundle.putString("name", str.substring(7,msgLength));
					last_dir_received=str.substring(7,msgLength);
				} else {
					bundle.putLong("size", (long)0);
					bundle.putString("name", "-");
					last_dir_received="";
				}
				msg.setData(bundle);
				mHandler.sendMessage(msg);
			};


			// fortschritt schreiben
			Message msg = mHandlerUpdate.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION); // typ egal wir zeigen alles an
			Bundle bundle = new Bundle();
			String shown_message="Listing "+ last_dir_received;
			Log.i(TAG,shown_message);
			bundle.putString(SpeedoAndroidActivity.BYTE_TRANSFERED, shown_message);
			msg.setData(bundle);
			mHandlerUpdate.sendMessage(msg); 

		}
		return 0;
	};

	public int getFile(String filename, String dlBaseDir,Handler mHandlerUpdate, String file_size) throws InterruptedException {
		Log.i(TAG,"getFile gestartet: filename "+filename+" dlBasedir "+dlBaseDir);


		//publishProgress("" + (int)((total*100)/lenghtOfFile));
		/* hinweg:
		 * msgBuffer[0]=CMD_GET_FILE
		 * msgBUffer[1]=length of filename
		 * msgBuffer[2..X]=filename  ... datei.txt oder folder/datei.txt
		 * msgBuffer[x+1]=high_nibble of cluster nr
		 * msgBuffer[x+2]=low_nibble of cluster nr 
		 * 
		 * rueckweg:
		 * msgBuffer[0]=CMD_GET_FILE
		 * msgBuffer[1]=COMMAND_OK
		 * msgBuffer[2..]=DATA 
		 */

		//topProcess.publishProgress(3);
		//topProcess.onProgressUpdate(13); 

		int failCounter=0;
		item = 0;
		byte send[] = new byte[2+filename.length()+2]; // 2 fuer 250Byte Cluster + n fuer name + 1 command + 1 filename length
		// prepare static part
		send[0]=CMD_GET_FILE;
		send[1]=(byte) (filename.length() & 0x000000FF);
		for(int i=0;i<filename.length();i++){
			send[i+2]=(byte)filename.charAt(i);
		};

		// open File
		// lets see, what kind of file we have
		if(filename.substring(filename.indexOf('/')+1).toLowerCase().equals("gps")){
			dlBaseDir=dlBaseDir+"GPS/";
		} else if(filename.substring(filename.indexOf('/')+1).toLowerCase().equals("sgf")){
			dlBaseDir=dlBaseDir+"GFX/";
		} else if(filename.substring(filename.indexOf('/')+1).toLowerCase().equals("smf")){
			dlBaseDir=dlBaseDir+"NAVI/";
		}
		File dir = new File (dlBaseDir);
		File file = new File(dir, filename.substring(filename.indexOf('/')+1));

		FileOutputStream out = null;
		try { 								out = new FileOutputStream(file);	} 
		catch (FileNotFoundException e) { 	e.printStackTrace();				}

		// check folder dlBaseDir
		// check folder in filename, if isset
		// JFile file.open()

		status=1;
		while(status!=STATUS_EOF){
			Log.i(TAG,"Whileschleifeniteration");

			// fortschritt schreiben
			Message msg = mHandlerUpdate.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
			Bundle bundle = new Bundle();

			String shown_message=null;
			int prozent=(int) ((int)(item*250*100)/Long.parseLong(file_size));
			if(prozent>100 && Long.parseLong(file_size)<250) prozent=100; // wir ?bertragen 250 Byte auf einmal .. daher
			shown_message=String.valueOf(prozent)+ "% of "+file_size+" Bytes transfered";
			bundle.putString(SpeedoAndroidActivity.BYTE_TRANSFERED, shown_message);
			msg.setData(bundle);
			mHandlerUpdate.sendMessage(msg); 

			// prepare dynamic part
			send[2+filename.length()]=(byte) ((item & 0xff00)>>8); //danger wegen signed ? interessant ab ueber 127 Files
			send[3+filename.length()]=(byte) (item & 0x00ff);
			item++;
			// send setzt jetzt den semaphore und erst 
			// receive gibt ihn wieder her
			Log.e(TAG_RECV,"Vor dem Send item"+String.valueOf(item));
			Log.i("SEND","getFile()");
			int send_value=send(send, send.length);
			Log.e(TAG_BT,"Hinter dem Send");

			if(send_value>0){
				mTimerHandle.removeCallbacks(mCheckResponseTimeTask);
				semaphore.release();
				Log.i(TAG_SEM,"get_file, send failed hat den semaphore zurueck gegeben");
				return send_value;
			}

			// wait here until we can get the semaphore
			// im schlimmsten fall hier ein while auf ne globale variable
			Log.i(TAG_SEM,"get file wartet");
			Log.i(TAG_SEM,String.valueOf(semaphore.availablePermits())+" frei");
			semaphore.acquire();
			Log.i(TAG_SEM,"getFile hat sich einen semaphore gekrallt");
			// hier koennen wir nun am status sehen, wer uns wieder freigegeben hat: 1=Speedoino, ST_EMERGENCY_RELEASE=Timer
			if(status==ST_EMERGENCY_RELEASE){
				// hier sowas wie: 
				item--; 
				failCounter++;
				if(failCounter>3){
					status=STATUS_EOF;
					msgLength=0;
					msg = mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_LOG);
					bundle = new Bundle();
					bundle.putString(SpeedoAndroidActivity.TOAST, "Transmission failed");
					msg.setData(bundle);
					mHandler.sendMessage(msg);
				}
			} else { 
				failCounter=0;
			}

			// versuche den inhalt des Buffers in die Datei zu schreiben
			// inhalt casten
			byte tempBuffer[] = new byte[300];
			for(int i=0;i<msgLength-2;i++){
				tempBuffer[i]=(byte)(msgBuffer[i+2] & 0x00FF);
			}

			if(msgLength>2){
				try {						out.write(tempBuffer,0,msgLength-2);	} 
				catch (IOException e) {		e.printStackTrace();			}
			}

			// loese desSemaphore und damit sind wir bei 0 genommenen semaphoren und send kann in der naechsten 
			// runde, wieder einen semphore ohne einschraenkung bekommen

			if((msgBuffer[1]&0x00ff)==(STATUS_CMD_FAILED&0x00ff)){
				status=STATUS_EOF;
			} else if ((msgBuffer[1]&0x00ff)==(STATUS_EOF&0x00ff)) {
				status=STATUS_EOF;
			}
			semaphore.release();
			Log.i(TAG_SEM,"getFile hat den semaphore zurueck gegeben");

		}
		// status EOF erreich, datei schliessen und meldung zurueck geben
		// file.close();
		try {						out.close();			} 
		catch (IOException e) {		e.printStackTrace();	}
		return 0;

	}

	public int putFile(String source, String dest, Handler mHandlerUpdate)  throws IOException, InterruptedException {
		/* hinweg:
		 * msgBuffer[0]=CMD_PUT_FILE
		 * msgBuffer[1]=length of filename
		 * msgBuffer[2..X]=filename  ... datei.txt oder folder/datei.txt
		 * msgBuffer[x+1]=high_nibble of cluster nr
		 * msgBuffer[x+2]=low_nibble of cluster nr
		 * msgBuffer[X+3..250]=Content
		 *  
		 * rueckweg:
		 * msgBuffer[0]=CMD_PUT_FILE
		 * msgBuffer[1]=COMMAND_OK
		 */
		int return_value=0;
		Log.i(TAG_RECV,"Put file startet: "+source+" to "+dest);
		int startOfPayload=0;
		int payloadLength=250;
		int bytesToSend=999;
		item = 0; // cluster nr
		byte send[] = new byte[250]; // 250 auf Vorbehalt, da die tatsaechliche laenge auch davon abhoengt wieviel noch da ist in der Datei
		// prepare static part
		send[0]=CMD_PUT_FILE;
		send[1]=(byte) (dest.length() & 0x000000FF);
		for(int i=0;i<dest.length();i++){
			send[i+2]=(byte)dest.charAt(i);
		};
		startOfPayload=dest.length()+4;
		payloadLength=250-startOfPayload;

		// open File
		File file = new File(source);
		FileInputStream in = null;
		long filesize=file.length();

		try { 								in = new FileInputStream(file);		} 
		catch (FileNotFoundException e) { 	e.printStackTrace();				}

		int upload_status=1;
		int transfered_bytes=0;

		while(upload_status!=STATUS_EOF){
			Log.i(TAG_RECV,"Vor dem Send item"+String.valueOf(item));

			// fortschritt schreiben
			Message msg = mHandlerUpdate.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
			Bundle bundle = new Bundle();

			String shown_message=null;
			int per_transfered=(int) (transfered_bytes*100/filesize);
			if(per_transfered<120){
				if(per_transfered>100){
					shown_message="Transfer completed";
				} else {
					shown_message=String.valueOf(per_transfered)+ "% transfered";
				};
			} else {
				shown_message="Error in transfer";
			}
			bundle.putString(SpeedoAndroidActivity.BYTE_TRANSFERED, shown_message);
			msg.setData(bundle);
			mHandlerUpdate.sendMessage(msg); 

			// datei auslesen und in puffer packen
			bytesToSend=startOfPayload; // 										17
			//in.reset(); // rewind file
			//in.skip(item*payloadLength); // seek to pos
			bytesToSend+=in.read(send, startOfPayload, payloadLength); // +84
			Log.e(TAG_RECV,"Bytes to send: "+String.valueOf(bytesToSend));

			// prepare dynamic part
			send[startOfPayload-2]=(byte) ((item & 0xff00)>>8); //danger wegen signed ? interessant ab ueber 127 Files
			send[startOfPayload-1]=(byte) (item & 0x00ff);
			item++;

			if(bytesToSend<=startOfPayload){ // fehler von in.read() oder 0 byte mehr zu lesen
				bytesToSend=2; 	// der tacho muss auf beides achten, einfach nur checken was in byte[1] steht reicht nicht !!
				send[1]=STATUS_EOF;
				upload_status=STATUS_EOF;
			};

			// send setzt jetzt den semaphore und erst 
			// receive gibt ihn wieder her
			Log.i("SEND","put_file()");
			int send_value=send(send, bytesToSend);

			if(send_value>0){
				mTimerHandle.removeCallbacks(mCheckResponseTimeTask);
				semaphore.release();
				Log.i(TAG_SEM,"Put File, send failed hat den semaphore zurueck gegeben");
				return send_value;
			}

			// wait here until we can get the semaphore
			// im schlimmsten fall hier ein while auf ne globale variable
			Log.i(TAG_SEM,"puf file wartet");
			Log.i(TAG_SEM,String.valueOf(semaphore.availablePermits())+" frei");
			semaphore.acquire();
			Log.i(TAG_SEM,"put file hat semaphore erhalten");
			transfered_bytes+=payloadLength;
			// hier koennen wir nun am status sehen, wer uns wieder freigegeben hat: 1=Speedoino, ST_EMERGENCY_RELEASE=Timer
			if(status==ST_EMERGENCY_RELEASE){
				Log.i(TAG_RECV,"EMERGENCY TOKEN RETURN");
				// hier sowas wie: 

				status=STATUS_EOF;
				msgLength=0;
				msg = mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_LOG);
				bundle = new Bundle();
				bundle.putString(SpeedoAndroidActivity.TOAST, "Transmission failed");
				msg.setData(bundle);
				mHandler.sendMessage(msg);
				return_value=-2;
			}


			// loese desSemaphore und damit sind wir bei 0 genommenen semaphoren und send kann in der naechsten 
			// runde, wieder einen semphore ohne einschraenkung bekommen
			semaphore.release();
			Log.i(TAG_SEM,"Put file hat den semaphore zurueck gegeben");
			Log.i(TAG,"und auch wieder gehen lassen");
			if(msgBuffer[1]==STATUS_EOF || msgBuffer[1]==STATUS_CMD_FAILED){
				upload_status=STATUS_EOF;
				if(msgBuffer[1]==STATUS_CMD_FAILED){
					msg = mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_LOG);
					bundle = new Bundle();
					bundle.putString(SpeedoAndroidActivity.TOAST, "Transmission error");
					msg.setData(bundle);
					mHandler.sendMessage(msg);
					return_value=-1;
				}
				break;
			}

		}
		Log.i(TAG,"while schleife abgeschlossen");
		// status EOF erreich, datei schliessen und meldung zurueck geben
		// file.close();
		try {						in.close();			} 
		catch (IOException e) {		e.printStackTrace(); return_value=-3;	}
		if(return_value==0){
			//			if(!file.delete()){
			//				return_value=-4;
			//			}
		}
		return return_value;
	}

	public int delFile(String filename) throws InterruptedException {
		/* hinweg:
		 * msgBuffer[0]=CMD_DEL_FILE
		 * msgBuffer[1]=length of filename
		 * msgBuffer[2..X]=filename  ... datei.txt oder folder/datei.txt
		 * 
		 * rueckweg:
		 * msgBuffer[0]=CMD_PUT_FILE
		 * msgBuffer[1]=COMMAND_OK
		 */
		Log.i(TAG, "loeschen gestartet");
		byte send[] = new byte[2+filename.length()]; // 250 auf Vorbehalt, da die tatsaechliche laenge auch davon abhoengt wieviel noch da ist in der Datei
		// prepare static part
		send[0]=CMD_DEL_FILE;
		send[1]=(byte) (filename.length() & 0x000000FF);
		for(int i=0;i<filename.length();i++){
			send[i+2]=(byte)filename.charAt(i);
		};
		Log.i(TAG, "sendet");
		Log.i("SEND","deleteFile()");
		int send_value=send(send, 2+filename.length());

		if(send_value>0){
			mTimerHandle.removeCallbacks(mCheckResponseTimeTask);
			semaphore.release();
			Log.i(TAG_SEM,"delefe file, send failed hat den semaphore zurueck gegeben");
			return send_value;
		}

		// wait here until we can get the semaphore
		// im schlimmsten fall hier ein while auf ne globale variable
		Log.i(TAG_SEM, "delete file wartet");
		Log.i(TAG_SEM,String.valueOf(semaphore.availablePermits())+" frei");
		semaphore.acquire();
		Log.i(TAG_SEM,"delfile hat sich einen semaphore gekrallt");
		semaphore.release();
		Log.i(TAG_SEM,String.valueOf(semaphore.availablePermits())+" frei");
		Log.i(TAG_SEM,"delete file hat den semaphore zurueck gegeben");
		// hier koennen wir nun am status sehen, wer uns wieder freigegeben hat: 1=Speedoino, ST_EMERGENCY_RELEASE=Timer
		if(status==ST_EMERGENCY_RELEASE){
			Log.i(TAG_RECV,"EMERGENCY TOKEN RETURN");
			// hier sowas wie: 

			status=STATUS_EOF;
			msgLength=0;
			Message msg = mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_LOG);
			Bundle bundle = new Bundle();
			bundle.putString(SpeedoAndroidActivity.TOAST, "Delete failed");
			msg.setData(bundle);
			mHandler.sendMessage(msg);
			Log.i(TAG, "return -1");
			return -1;

		} else {
			Log.i(TAG_RECV,"normales exit");
		}
		Log.i(TAG, "delfile ok,return 0");
		return 0;
	}

	public int showgfx(String filename) throws InterruptedException {
		/* hinweg:
		 * msgBuffer[0]=CMD_SHOW_GFX
		 * msgBuffer[1]=length of filename
		 * msgBuffer[2..X]=filename  ... datei.txt oder folder/datei.txt
		 * 
		 * rueckweg:
		 * msgBuffer[0]=CMD_SHOW_GFX
		 * msgBuffer[1]=COMMAND_OK
		 */
		filename=filename.toUpperCase();
		Log.i(TAG, "Zeige bild");
		byte send[] = new byte[2+filename.length()]; // 250 auf Vorbehalt, da die tatsaechliche laenge auch davon abhoengt wieviel noch da ist in der Datei
		// prepare static part
		send[0]=CMD_SHOW_GFX;
		send[1]=(byte) (filename.length() & 0x000000FF);
		for(int i=0;i<filename.length();i++){
			send[i+2]=(byte)filename.charAt(i);
		};
		Log.i(TAG, "sendet");
		Log.i("SEND","showgfx()");
		int send_value=send(send, 2+filename.length(),10000); // 10 sec für die animation

		if(send_value>0){
			mTimerHandle.removeCallbacks(mCheckResponseTimeTask);
			semaphore.release();
			Log.i(TAG_SEM,"show gfx send failed hat den semaphore zurueck gegeben");
			return send_value;
		}

		// wait here until we can get the semaphore
		// im schlimmsten fall hier ein while auf ne globale variable
		Log.i(TAG_SEM, "show gfx wartet");
		Log.i(TAG_SEM,String.valueOf(semaphore.availablePermits())+" frei");
		semaphore.acquire();
		Log.i(TAG_SEM,"showgfx hat sich einen semaphore gekrallt");
		semaphore.release();
		Log.i(TAG_SEM,String.valueOf(semaphore.availablePermits())+" frei");
		Log.i(TAG_SEM,"show gfx hat den semaphore zurueck gegeben");
		// hier koennen wir nun am status sehen, wer uns wieder freigegeben hat: 1=Speedoino, ST_EMERGENCY_RELEASE=Timer
		if(status==ST_EMERGENCY_RELEASE){
			Log.i(TAG_RECV,"EMERGENCY TOKEN RETURN");
			// hier sowas wie: 

			status=STATUS_EOF;
			msgLength=0;
			Message msg = mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_LOG);
			Bundle bundle = new Bundle();
			bundle.putString(SpeedoAndroidActivity.TOAST, "Show image failed");
			msg.setData(bundle);
			mHandler.sendMessage(msg);
			Log.i(TAG, "return -1");
			return -1;

		}
		Log.i(TAG, "return 0");
		return 0;

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
	public int uploadFirmware(String filename,Handler mHandlerUpdate,String flashingDevice)throws IOException, InterruptedException {
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
						Message msg2 = mHandlerUpdate.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
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
			Message msg = mHandlerUpdate.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
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
				Message msg3 = mHandlerUpdate.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
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
			Message msg3 = mHandlerUpdate.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
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

				Message msg4 = mHandlerUpdate.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
				Bundle bundle4 = new Bundle();
				bundle4.putString("msg", "Connection not established");
				bundle4.putInt("state", 102);
				msg4.setData(bundle4);
				mHandlerUpdate.sendMessage(msg4);

				//return error;
			} else {
				connection_established=true;

				Message msg4 = mHandlerUpdate.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
				Bundle bundle4 = new Bundle();
				bundle4.putString("msg", "Connection established");
				bundle4.putInt("state", 103);
				msg4.setData(bundle4);
				mHandlerUpdate.sendMessage(msg4);

				/* jetzt muessen wir uns ueberlegen:
				 * ist das File maximal 32k gross ? 
				 * -> wenn ja: ATm2560
				 * --> Dann muessen wir in den Bootloader
				 * --> Connection abbauen und neu aufbauen
				 * --> dann STK500v2 
				 * -> wenn nein: ATm328
				 * --> Dann brauchen wir nur ein bestimmtes
				 * --> Command schicken, der ATm2560 resettet
				 * --> den ATm328 und tunnelt den Transfer
				 */
				if(highest_pos<=32768){
					// wir flashen einen ATm328
					flash2560=0;
					// fortschritt schreiben
					Message msg5 = mHandlerUpdate.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
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
						msg4 = mHandlerUpdate.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
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
//						if(msgBuffer[1]!=STATUS_CMD_OK && false){ //!! BUG !! der Tacho geht zuerst in den Reset und würde danach erst anworten, daher wird hier nix zurück kommen :(
//							error=401;
//
//							// fortschritt schreiben
//							Bundle bundle9 = new Bundle();
//							Message msg9 = mHandlerUpdate.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
//							bundle9.putString("msg", "Invalid response, quitting");
//							bundle9.putInt("state",
//									55);
//							msg9.setData(bundle9);
//							mHandlerUpdate.sendMessage(msg9);
//							Thread.sleep(3000);
//							// fortschritt schreiben
//
//							return error;
//						};
						//setState(STATE_CONNECTED);
						Thread.sleep(10); // wait 10 ms
						Log.i(TAG_LOGIN,"reset done");
					} else {
						error=41;

						// fortschritt schreiben
						Bundle bundle9 = new Bundle();
						Message msg9 = mHandlerUpdate.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
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
			Message msg5 = mHandlerUpdate.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
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
		Message msg3 = mHandlerUpdate.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
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
				Message msg9 = mHandlerUpdate.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
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
			Message msg = mHandlerUpdate.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
			bundle.putString("msg", "Sign on done");
			bundle.putInt("state", 88);
			msg.setData(bundle);
			mHandlerUpdate.sendMessage(msg);
			// fortschritt schreiben
		} else {
			error=5;

			// fortschritt schreiben
			Bundle bundle9 = new Bundle();
			Message msg9 = mHandlerUpdate.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
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
		while(retries<4 && !correct_id_found){
			retries++;

			// fortschritt schreiben
			Bundle bundle9 = new Bundle();
			Message msg9 = mHandlerUpdate.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
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
					Message msg39 = mHandlerUpdate.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
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
						Message msg19 = mHandlerUpdate.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
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
							Message msg29 = mHandlerUpdate.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
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
				Message msg8 = mHandlerUpdate.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
				if(prozessor_id==0x1E9801){
					bundle8.putString("msg", "ATm2560 ID found");
				} else {
					bundle8.putString("msg", "ATm328 ID found");
				}
				bundle8.putInt("state", 45);
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
				if(msgBuffer[1]!=STATUS_CMD_OK){
					error=7;

					// fortschritt schreiben
					Bundle bundle9 = new Bundle();
					Message msg9 = mHandlerUpdate.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
					bundle9.putString("msg", "Unknown response, quitting");
					bundle9.putInt("state", 55);
					msg9.setData(bundle9);
					mHandlerUpdate.sendMessage(msg9);
					Thread.sleep(3000);
					// fortschritt schreiben

					return error;
				};
			} else {
				error=8;

				// fortschritt schreiben
				Bundle bundle49 = new Bundle();
				Message msg49 = mHandlerUpdate.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
				bundle49.putString("msg", "No response, quitting");
				bundle49.putInt("state", 55);
				msg49.setData(bundle49);
				mHandlerUpdate.sendMessage(msg49);
				Thread.sleep(3000);
				// fortschritt schreiben

				return error;
			};

			// fortschritt schreiben
			Bundle bundle = new Bundle();
			Message msg = mHandlerUpdate.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
			bundle.putString("msg", "Speedoino at "+String.valueOf(Math.floor(send_position/100)/10)+"/"+String.valueOf(Math.floor(highest_pos/100)/10)+" KB");
			bundle.putInt("state", 7);
			msg.setData(bundle);
			mHandlerUpdate.sendMessage(msg);
			// fortschritt schreiben

		}
		// send leave_programmer command
		send2[0]=CMD_LEAVE_PROGMODE_ISP;
		Log.i("SEND","upload_fileware6()");
		send_save(send2,1,1000,1);	
		////// hier beginnt teil 6, Die Firmware aus dem HexFile in den Controller schreiben ////// 
		// fortschritt schreiben
		Bundle bundle37 = new Bundle();
		Message msg37 = mHandlerUpdate.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
		bundle37.putString("msg", "Thanks for using");
		bundle37.putInt("state", 97);
		msg37.setData(bundle37);
		mHandlerUpdate.sendMessage(msg37);
		Thread.sleep(10000);


		return 0;
	}
	///////////////////////////// FIRMWARE UPDATE /////////////////////////////
}
