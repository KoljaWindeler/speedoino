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

import de.windeler.kolja.SpeedoAndroidActivity.getFileDialog;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

/**
 * This class does all the work for setting up and managing Bluetooth
 * connections with other devices. It has a thread that listens for
 * incoming connections, a thread for connecting with a device, and a
 * thread for performing data transmissions when connected.
 */
public class BluetoothSerialService {
	// Debugging
	private static final String TAG = "JKW";
	private static final String TAG_RECV = "JKW_RECV";
	private static final String TAG_BT = "BT";
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
	public int item=0;

	public static final int ST_IDLE				= -1;
	public static final int ST_START 			= 0;
	public static final int ST_GET_SEQ_NUM		= 1;
	public static final int ST_MSG_SIZE			= 2;
	//public static final int ST_MSG_SIZE_2		= 3;
	public static final int ST_GET_TOKEN		= 4;
	public static final int ST_GET_DATA			= 5;
	public static final int ST_GET_CHECK		= 6;
	public static final int ST_PROCESS			= 7;
	public static final int ST_EMERGENCY_RELEASE= 8;


	private static final UUID SerialPortServiceClass_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

	// Member fields
	private final BluetoothAdapter mAdapter;
	private final Handler mHandler;
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
	public static final byte CMD_GET_FILE		=  0x12;
	public static final byte CMD_PUT_FILE		=  0x13;
	public static final byte CMD_DEL_FILE		=  0x14;

	public static final char STATUS_CMD_OK      =  0x09;
	public static final char STATUS_CMD_FAILED  =  0xC0;
	public static final char STATUS_CKSUM_ERROR =  0xC1;
	public static final char STATUS_CMD_UNKNOWN =  0xC9;
	public static final char STATUS_EOF 		=  0x10;

	/**
	 * Constructor. Prepares a new BluetoothChat session.
	 * @param context  The UI Activity Context
	 * @param handler  A Handler to send messages back to the UI Activity
	 */
	public BluetoothSerialService(Context context, Handler handler) {
		mAdapter = BluetoothAdapter.getDefaultAdapter();
		mState = STATE_NONE;
		mHandler = handler;
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
	 */
	public synchronized void connect(BluetoothDevice device) {
		// Cancel any thread attempting to make a connection
		if (mState == STATE_CONNECTING) {
			if (mConnectThread != null) {mConnectThread.cancel(); mConnectThread = null;}
		}

		// Cancel any thread currently running a connection
		if (mConnectedThread != null) {mConnectedThread.cancel(); mConnectedThread = null;}

		// Start the thread to connect with the given device
		Log.e(TAG,"start thread to connect...");
		mConnectThread = new ConnectThread(device);
		Log.i(TAG,"starte jetzt mConnectedThread 2");
		mConnectThread.start();
		Log.i(TAG,"mConnectedThread ist durch 2");
		setState(STATE_CONNECTING);
	}

	/**
	 * Start the ConnectedThread to begin managing a Bluetooth connection
	 * @param socket  The BluetoothSocket on which the connection was made
	 * @param device  The BluetoothDevice that has been connected
	 */
	public synchronized void connected(BluetoothSocket socket, BluetoothDevice device) {
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
	}

	/**
	 * Stop all threads
	 */
	public synchronized void stop() {
		if (D) Log.d(TAG, "stop");

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
	public void write(byte out) {
		// Create temporary object
		ConnectedThread r;
		// Synchronize a copy of the ConnectedThread
		synchronized (this) {
			if (mState != STATE_CONNECTED) return;
			r = mConnectedThread;
		}
		// Perform the write unsynchronized
		r.write(out);
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
	private void connectionLost() {
		setState(STATE_NONE);
		BluetoothSerialService.this.start();
	}

	/**
	 * This thread runs while attempting to make an outgoing connection
	 * with a device. It runs straight through; the connection either
	 * succeeds or fails.
	 */
	private class ConnectThread extends Thread {
		private final BluetoothSocket mmSocket;
		private final BluetoothDevice mmDevice;

		public ConnectThread(BluetoothDevice device) {
			mmDevice = device;
			BluetoothSocket tmp = null;

			// Get a BluetoothSocket for a connection with the
			// given BluetoothDevice
			try {
				tmp = device.createRfcommSocketToServiceRecord(SerialPortServiceClass_UUID);
				Log.i(TAG,"RfcommSocked created()");
			} catch (IOException e) {
				Log.e(TAG, "create() failed", e);
			}
			mmSocket = tmp;
		}

		public void run() {
			Log.i(TAG, "BEGIN mConnectThread");
			setName("ConnectThread");

			// Always cancel discovery because it will slow down a connection
			mAdapter.cancelDiscovery();

			// Make a connection to the BluetoothSocket
			try {
				// This is a blocking call and will only return on a
				// successful connection or an exception
				mmSocket.connect();
				Log.i(TAG,"Connect() finished...");
			} catch (IOException e) {
				Log.e(TAG, "mConnectThread FAILED");
				connectionFailed();
				// Close the socket
				try {
					mmSocket.close();
				} catch (IOException e2) {
					Log.e(TAG, "unable to close() socket during connection failure", e2);
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
			connected(mmSocket, mmDevice);
		}

		public void cancel() {
			try {
				mmSocket.close();
			} catch (IOException e) {
				Log.e(TAG, "close() of connect socket failed", e);
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
			Log.i(TAG, "create ConnectedThread");
			mmSocket = socket;
			InputStream tmpIn = null;
			OutputStream tmpOut = null;

			// Get the BluetoothSocket input and output streams
			try {
				tmpIn = socket.getInputStream();
				tmpOut = socket.getOutputStream();
			} catch (IOException e) {
				Log.e(TAG, "temp sockets not created", e);
			}

			mmInStream = tmpIn;
			mmOutStream = tmpOut;
		}

		public void run() {
			Log.i(TAG, "BEGIN mConnectedThread");
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
						if(!preamble_found){
							if(finished_str[finished_counter]==buffer[i]){
								finished_counter++;
								if(finished_counter==temp.length()){
									setState(STATE_CONNECTED);
									preamble_found=true;
								}
							} else {
								finished_counter=0;
							}
						}
					}
				} catch (IOException e) {
					Log.e(TAG, "disconnected", e);
					//connectionLost();
					break;
				}
			}
		}

		/**
		 * Write to the connected OutStream.
		 * @param out  The bytes to write
		 */
		public void write(byte out) {
			try {
				mmOutStream.write(out);

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

	public int send(byte data[],int msgLength) throws InterruptedException{
		byte	checksum		= 0;
		byte 	c				= 0;
		byte 	p				= 0;
		// nur senden, wenn wir nicht gerade was empfangen
		if(getState()!=STATE_CONNECTED){
			Message msg = mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_TOAST);
			Bundle bundle = new Bundle();
			bundle.putString(SpeedoAndroidActivity.TOAST, "You are not connected to a Speedoino");
			msg.setData(bundle);
			mHandler.sendMessage(msg);

			return 1;
		}

		// da der Tacho, nach 2sek den fast response mode verlässt, müssen wir die seq neu zählen
		if(System.currentTimeMillis()-lastSend>2000){
			reset_seq();
		}

		seqNum++; // wir starten mit 0 und setzten im notfall auch zu 0 zurück, daher immer VOR dem senden inkrementieren


		if(rx_tx_state==ST_IDLE){
			if(msgLength<=0) return 2;
			
			Log.i(TAG,"BT Telegramm will starten, warte auf den semaphore");
			semaphore.acquire();
			Log.i(TAG,"hab den semaphore");


			c=(byte)MESSAGE_START;
			write(c);		// Message Start
			Log.d(TAG_RECV,"BTsend:"+String.valueOf((int)c)+"/MSG_START");
			checksum	=	c;
			c=(byte)seqNum;
			write(c);		// Seq Nr
			Log.d(TAG_RECV,"BTsend:"+String.valueOf((int)c));
			checksum	^=	c;
			c=(byte) (msgLength&0x00FF);
			write(c);		// length max 255
			Log.d(TAG_RECV,"BTsend:"+String.valueOf((int)c));
			checksum ^= c;
			c=(byte)TOKEN;
			write(c);		// Token
			Log.d(TAG_RECV,"BTsend:"+String.valueOf((int)c)+"/TOKEN");
			checksum ^= TOKEN;


			for(int i=0; i<msgLength; i++){
				p	=	data[i];
				write(p);	// send some data
				checksum ^= p;
				Log.d(TAG_RECV,"BTsend:"+String.valueOf((int)p)+"/"+String.valueOf((char)p));
			}
			write(checksum);	//	CHECKSUM
			Log.d(TAG_RECV,"BTsend:"+String.valueOf((int)checksum));
			rx_tx_state=ST_START; // start listening

			// install guard, 2sec until check of receive
			mTimerHandle.removeCallbacks(mCheckResponseTimeTask);
			mTimerHandle.postDelayed(mCheckResponseTimeTask, 2000);
			return 0;

		} else {
			Log.i(TAG,"State nicht IDLE");
		};
		return 3;
	};

	private Runnable mCheckResponseTimeTask = new Runnable() {
		public void run() {
			if(rx_tx_state!=ST_IDLE){
				rx_tx_state=ST_IDLE;
				reset_seq();
				Log.i(TAG,"timer notfall, gebe semaphore zurück");
				Log.i(TAG_RECV,"timer notfall, gebe semaphore zurück");
				semaphore.release();
				status=ST_EMERGENCY_RELEASE;


				Message msg = mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_TOAST);
				Bundle bundle = new Bundle();
				bundle.putString(SpeedoAndroidActivity.TOAST, "@string/noresponse");
				msg.setData(bundle);
				mHandler.sendMessage(msg);
			};
		}
	};

	private void process_incoming(char data) {
		Log.i(TAG,"process_incoming gestartet mit:"+String.valueOf((int)(data&0x00ff))+"/"+((char)(data&0x00ff))+" rx_state:"+String.valueOf((int)rx_tx_state));			
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
			msgLength		&=	0x000000FF;
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
					// hier jetzt in unsere oberflche die id eintragen
					if((msgBuffer[1] & 0xff)==STATUS_CMD_OK){
						String str = new String(msgBuffer);

						msg = mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_VERSION);
						bundle = new Bundle();
						bundle.putString(SpeedoAndroidActivity.TOAST, str.substring(2,msgLength));
						msg.setData(bundle);
						mHandler.sendMessage(msg); 
						Log.i(TAG,"statemachine ok, gebe semaphore zurück");

					} else {
						// irgendwie das command nochmal senden
					}
					break;
					// da alle richtungen zwar betätigt werden, danach die schleife auf dem AVR aber unterbrochen wird -> seqNr resetten
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
				case CMD_DIR:
					String str = new String(msgBuffer);
					Log.i(TAG,"CMD Dir erhalten:"+str.substring(3,msgLength)+" type "+String.valueOf((int)msgBuffer[2]));


					msg = mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_DIR_APPEND);
					bundle = new Bundle();
					bundle.putString("name", str.substring(3,msgLength));
					bundle.putInt("type", (int)msgBuffer[2]);
					msg.setData(bundle);
					mHandler.sendMessage(msg);

					break;
				case CMD_GET_FILE:
					// nothing to do, just keep it in buffer, get_file() will care for it.
					break;
				default:
					Log.i(TAG,"unknown command received");
					// irgendwie das commando nochmal senden
					msg = mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_DIR_APPEND);
					bundle = new Bundle();
					bundle.putString(SpeedoAndroidActivity.TOAST, "unknown command from speedo received");
					msg.setData(bundle);
					mHandler.sendMessage(msg);
					break;
				}
				semaphore.release();
				Log.i(TAG,"semaphore zurück gegeben");
				status=msgBuffer[2];

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

	public int getDir(String dir) throws InterruptedException{

		item=0;
		byte send[] = new byte[dir.length()+2+1]; // 2 für length of item + name + command
		status=1;

		// prepare static part
		send[0]=CMD_DIR;
		for(int i=0;i<dir.length();i++){
			send[i+3]=(byte)dir.charAt(i);
		}

		while(status!=STATUS_EOF){
			// prepare dynamic part
			send[1]=(byte) ((item & 0xff00)>>8); //danger wegen signed ? interessant ab über 127 Files
			send[2]=(byte) (item & 0x00ff);
			item++;
			// send setzt jetzt den semaphore und erst 
			// receive gibt ihn wieder her
			Log.e(TAG_BT,"Vor dem Send "+String.valueOf(item));
			Log.i(TAG,"derzeit sind "+String.valueOf(semaphore.availablePermits())+" semaphore frei");
			int send_value=send(send, send.length);
			Log.e(TAG_BT,"Hinter dem Send");

			if(send_value>0){
				semaphore.release();
				return send_value;
			}

			// wait here until we can get the semaphore
			// im schlimmsten fall hier ein while auf ne globale variable
			semaphore.acquire();
			Log.i(TAG,"get dir hat den semaphore von receive zurück bekommen und geht in die nächste runde");
			semaphore.release();

		}
		return 0;
	};

	public int getFile(String filename, String dlBaseDir,getFileDialog topProcess) throws InterruptedException {
		Log.i(TAG,"getFile gestartet: filename "+filename+" dlBasedir "+dlBaseDir);
		
		
		//publishProgress("" + (int)((total*100)/lenghtOfFile));
		/* hinweg:
		 * msgBuffer[0]=CMD_GET_FILE
		 * msgBUffer[1]=length of filename
		 * msgBuffer[2..X]=filename  ... datei.txt oder folder/datei.txt
		 * msgBuffer[x+1]=high_nibble of cluster nr
		 * msgBuffer[x+2]=low_nibble of cluster nr 
		 * 
		 * rückweg:
		 * msgBuffer[0]=CMD_GET_FILE
		 * msgBuffer[1]=COMMAND_OK
		 * msgBuffer[2..]=DATA 
		 */
		
		//topProcess.publishProgress(3);
		//topProcess.onProgressUpdate(13); 
		
		int failCounter=0;
		item = 0;
		byte send[] = new byte[2+filename.length()+2]; // 2 für 250Byte Cluster + n für name + 1 command + 1 filename length
		// prepare static part
		send[0]=CMD_GET_FILE;
		send[1]=(byte) (filename.length() & 0x000000FF);
		for(int i=0;i<filename.length();i++){
			send[i+2]=(byte)filename.charAt(i);
		};

		// open File
		File sdCard = Environment.getExternalStorageDirectory();
		File dir = new File (sdCard.getAbsolutePath() + "/Download/");
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
			// prepare dynamic part
			send[2+filename.length()]=(byte) ((item & 0xff00)>>8); //danger wegen signed ? interessant ab über 127 Files
			send[3+filename.length()]=(byte) (item & 0x00ff);
			item++;
			// send setzt jetzt den semaphore und erst 
			// receive gibt ihn wieder her
			Log.e(TAG_RECV,"Vor dem Send item"+String.valueOf(item));
			int send_value=send(send, send.length);
			Log.e(TAG_BT,"Hinter dem Send");

			if(send_value>0){
				semaphore.release();
				return send_value;
			}

			// wait here until we can get the semaphore
			// im schlimmsten fall hier ein while auf ne globale variable
			semaphore.acquire();
			Log.i(TAG,"getFile hat sich einen semaphore gekrallt");
			// hier können wir nun am status sehen, wer uns wieder freigegeben hat: 1=Speedoino, ST_EMERGENCY_RELEASE=Timer
			if(status==ST_EMERGENCY_RELEASE){
				// hier sowas wie: 
				item--; 
				failCounter++;
				if(failCounter>3){
					status=STATUS_EOF;
					msgLength=0;
					Message msg = mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_LOG);
					Bundle bundle = new Bundle();
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

			// löse desSemaphore und damit sind wir bei 0 genommenen semaphoren und send kann in der nächsten 
			// runde, wieder einen semphore ohne einschränkung bekommen
			semaphore.release();
			if(msgBuffer[1]==STATUS_CMD_FAILED || msgBuffer[1]==STATUS_EOF){
				status=STATUS_EOF;
				break;
			}

		}
		// status EOF erreich, datei schließen und meldung zurück geben
		// file.close();
		try {						out.close();			} 
		catch (IOException e) {		e.printStackTrace();	}
		return 0;

	}

	public int putFile(String source, String dest)  throws IOException, InterruptedException {
		/* hinweg:
		 * msgBuffer[0]=CMD_PUT_FILE
		 * msgBuffer[1]=length of filename
		 * msgBuffer[2..X]=filename  ... datei.txt oder folder/datei.txt
		 * msgBuffer[x+1]=high_nibble of cluster nr
		 * msgBuffer[x+2]=low_nibble of cluster nr
		 * msgBuffer[X+3..250]=Content
		 * 
		 * rückweg:
		 * msgBuffer[0]=CMD_PUT_FILE
		 * msgBuffer[1]=COMMAND_OK
		 */
		int startOfPayload=0;
		int payloadLength=250;
		int bytesToSend=999;
		item = 0; // cluster nr
		byte send[] = new byte[250]; // 250 auf Vorbehalt, da die tatsächliche länge auch davon abhöngt wieviel noch da ist in der Datei
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

		try { 								in = new FileInputStream(file);		} 
		catch (FileNotFoundException e) { 	e.printStackTrace();				}

		int upload_status=1;

		while(upload_status!=STATUS_EOF){
			Log.e(TAG_RECV,"Vor dem Send item"+String.valueOf(item));
			// datei auslesen und in puffer packen

			bytesToSend=startOfPayload; // 										17
			//in.reset(); // rewind file
			//in.skip(item*payloadLength); // seek to pos
			bytesToSend+=in.read(send, startOfPayload, payloadLength); // +84


			Log.e(TAG_RECV,"Bytes to send: "+String.valueOf(bytesToSend));

			// prepare dynamic part
			send[startOfPayload-2]=(byte) ((item & 0xff00)>>8); //danger wegen signed ? interessant ab über 127 Files
			send[startOfPayload-1]=(byte) (item & 0x00ff);
			item++;

			if(bytesToSend<=startOfPayload){ // fehler von in.read() oder 0 byte mehr zu lesen
				bytesToSend=2; 	// der tacho muss auf beides achten, einfach nur checken was in byte[1] steht reicht nicht !!
				send[1]=STATUS_EOF;
				upload_status=STATUS_EOF;
			};

			// send setzt jetzt den semaphore und erst 
			// receive gibt ihn wieder her

			int send_value=send(send, bytesToSend);

			if(send_value>0){
				semaphore.release();
				return send_value;
			}

			// wait here until we can get the semaphore
			// im schlimmsten fall hier ein while auf ne globale variable
			semaphore.acquire();
			Log.i(TAG,"putfile hat sich einen semaphore gekrallt");
			// hier können wir nun am status sehen, wer uns wieder freigegeben hat: 1=Speedoino, ST_EMERGENCY_RELEASE=Timer
			if(status==ST_EMERGENCY_RELEASE){
				Log.i(TAG_RECV,"EMERGENCY TOKEN RETURN");
				// hier sowas wie: 

				status=STATUS_EOF;
				msgLength=0;
				Message msg = mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_LOG);
				Bundle bundle = new Bundle();
				bundle.putString(SpeedoAndroidActivity.TOAST, "Transmission failed");
				msg.setData(bundle);
				mHandler.sendMessage(msg);

			}


			// löse desSemaphore und damit sind wir bei 0 genommenen semaphoren und send kann in der nächsten 
			// runde, wieder einen semphore ohne einschränkung bekommen
			semaphore.release();
			if(msgBuffer[1]==STATUS_EOF || msgBuffer[1]==STATUS_CMD_FAILED){
				upload_status=STATUS_EOF;
				if(msgBuffer[1]==STATUS_CMD_FAILED){
					Message msg = mHandler.obtainMessage(SpeedoAndroidActivity.MESSAGE_SET_LOG);
					Bundle bundle = new Bundle();
					bundle.putString(SpeedoAndroidActivity.TOAST, "Transmission error");
					msg.setData(bundle);
					mHandler.sendMessage(msg);
				}
				break;
			}

		}
		// status EOF erreich, datei schließen und meldung zurück geben
		// file.close();
		try {						in.close();			} 
		catch (IOException e) {		e.printStackTrace();	}
		return 0;
	}

	public int delFile(String filename) throws InterruptedException {
		/* hinweg:
		 * msgBuffer[0]=CMD_DEL_FILE
		 * msgBuffer[1]=length of filename
		 * msgBuffer[2..X]=filename  ... datei.txt oder folder/datei.txt
		 * 
		 * rückweg:
		 * msgBuffer[0]=CMD_PUT_FILE
		 * msgBuffer[1]=COMMAND_OK
		 */
		Log.i(TAG, "löschen gestartet");
		byte send[] = new byte[2+filename.length()]; // 250 auf Vorbehalt, da die tatsächliche länge auch davon abhöngt wieviel noch da ist in der Datei
		// prepare static part
		send[0]=CMD_DEL_FILE;
		send[1]=(byte) (filename.length() & 0x000000FF);
		for(int i=0;i<filename.length();i++){
			send[i+2]=(byte)filename.charAt(i);
		};
		Log.i(TAG, "sendet");
		int send_value=send(send, 2+filename.length());

		if(send_value>0){
			semaphore.release();
			return send_value;
		}

		// wait here until we can get the semaphore
		// im schlimmsten fall hier ein while auf ne globale variable
		Log.i(TAG, "wartet");
		semaphore.acquire();
		Log.i(TAG,"delfile hat sich einen semaphore gekrallt");
		semaphore.release();
		Log.i(TAG, "fertig");
		// hier können wir nun am status sehen, wer uns wieder freigegeben hat: 1=Speedoino, ST_EMERGENCY_RELEASE=Timer
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

		}
		Log.i(TAG, "return 0");
		return 0;
	}
}
