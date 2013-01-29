package de.windeler.kolja;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.MappedByteBuffer;
import java.nio.channels.FileChannel;
import java.nio.channels.FileChannel.MapMode;

import android.R.string;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.graphics.BitmapFactory.Options;
import android.graphics.Color;
import android.graphics.Matrix;
import android.os.Bundle;
import android.os.Environment;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.View;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.Toast;

public class ImageEditor extends Activity implements OnClickListener{

	public static final String INPUT_FILE_NAME = "leeer";
	public static final String INPUT_DIR_PATH = "leer";
	public static final String OUTPUT_FILE_PATH = "RESULT_PATH";
	private static final String TAG = "imgconv";
	private boolean changing_text = false;

	private Button mSave;
	private Button mcancel;
	private EditText image_filename;
	private byte[] converted_image_buffer = new byte[64*64];
	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.image_editor_main);		
		String filename = getIntent().getStringExtra(INPUT_FILE_NAME);

		// idea: start show_preview,
		// if the file is an ordinary image, the preview will be shown
		// if its no openable the exeption will be catched
		// and if its a gif, the gif class will split it in temp images, show the first and
		// redirect the input filename to the temp_files
		try {		
			Log.i("JKW","kolja dein file ist:"+filename);
			show_preview(filename);
		} catch (Exception e) {
			Log.e("Error reading file", e.toString());
			AlertDialog alertDialog = new AlertDialog.Builder(this).create();
			alertDialog.setTitle("Warning");
			alertDialog.setMessage("Could not open file as image! Upload anyway?");
			alertDialog.setButton("Yes",new DialogInterface.OnClickListener() {
				@Override
				public void onClick(DialogInterface arg0, int arg1) {	 
					getIntent().putExtra(OUTPUT_FILE_PATH,getIntent().getStringExtra(INPUT_FILE_NAME));
					setResult(RESULT_OK, getIntent());
					finish();
				}});
			alertDialog.setButton2("No",new DialogInterface.OnClickListener() {
				@Override
				public void onClick(DialogInterface arg0, int arg1) {	finish();	}});
			alertDialog.show();
		}

		mSave = (Button) findViewById(R.id.SaveImageConverter);
		mSave.setOnClickListener(this);
		mcancel = (Button) findViewById(R.id.DiscardImageConverter);
		mcancel.setOnClickListener(this);

		image_filename = (EditText)findViewById(R.id.image_filename);
		String cleaned_filename = filename.substring(filename.lastIndexOf('/')+1).replaceAll("(?:[^a-z0-9A-Z]|(?<=['\"])s)","");

		int length_of_substring=cleaned_filename.length()-3; // "IAVjpg"
		if(length_of_substring>8){
			length_of_substring=8;
		}
		image_filename.setText(cleaned_filename.substring(0,length_of_substring));
		image_filename.addTextChangedListener(new TextWatcher() {

			@Override
			public void afterTextChanged(Editable arg0) {
				// TODO Auto-generated method stub

			}

			@Override
			public void beforeTextChanged(CharSequence arg0, int arg1,
					int arg2, int arg3) {
				// TODO Auto-generated method stub

			}

			@Override
			public void onTextChanged(CharSequence arg0, int start,
					int befor, int count) {
				Log.i(TAG,"los gehts");
				if(!changing_text){
					String dirty=arg0.toString();
					String clean=dirty.replaceAll("(?:[^a-z0-9A-Z]|(?<=['\"])s)","");
					if(clean.compareTo(dirty)!=0){
						changing_text=true;
						image_filename.setText("");
						image_filename.append(clean);
						changing_text=false;
						show_toast("Please avoid special chars as well as spaces in filename, extension will be added automaticly");
					}

					if(arg0.length()>8){
						changing_text=true;
						image_filename.setText("");
						image_filename.append(arg0.subSequence(0, 8));
						changing_text=false;
						show_toast("Max 8 Chars allowed");
					}
				} // changing text
			} // on text change
		}); // add text change listener

	}

	public void show_preview(String filename) throws IOException{
		//Create temp file
		File outputDir =  getBaseContext().getCacheDir(); // context being the Activity pointer
		File outputFile = File.createTempFile("Speedoino", "stf", outputDir); // speedoino temp file

		// first check if its a gif animation!
		// if so, let the Gif Decoder cut it and redirect fileinput to the temp files
		String ext=filename.substring(filename.lastIndexOf("."));
		if(ext.toLowerCase().equals(".gif")){
			String filename_without_ext=filename.substring(0, filename.lastIndexOf("."));					
			FileInputStream stream=new FileInputStream(filename);
					
			GifDecoderView status=new GifDecoderView(this, stream,filename_without_ext,outputDir.getAbsolutePath());
			// status checken .. sollte 0 sein
			getIntent().putExtra(INPUT_FILE_NAME,outputDir.getAbsolutePath()+filename_without_ext.substring(filename_without_ext.lastIndexOf("/")+1)+"_0.PNG");
		}


		// let convert_image to the work and create our image
		convert_image(filename,outputFile.getAbsolutePath(),0,false);

		// open it to read the result
		FileInputStream in=new FileInputStream(outputFile.getAbsolutePath());
		Bitmap bMap = Bitmap.createBitmap(128, 64, Bitmap.Config.RGB_565);
		ImageView image = (ImageView) findViewById(R.id.imageSpeedoPreview);
		// read datablock
		in.read(converted_image_buffer, 0, (int)(64*128*0.5)); // 64 lines, 128 cols, but just a half byte per px
		in.close();

		// copy values to image
		int x=0,y=0;
		for(int byte_read_counter=0;byte_read_counter<64*128*0.5;byte_read_counter++){
			int gs=(converted_image_buffer[byte_read_counter] & 0xf0);
			bMap.setPixel(x,y,Color.rgb(gs,gs,0)); // yellow = green+red, no blue
			gs=(converted_image_buffer[byte_read_counter] & 0xf) << 4;
			bMap.setPixel(x+1,y,Color.rgb(gs,gs,0)); // yellow = green+red, no blue
			x+=2;
			if(x>127){
				y++;
				x=0;
			}
		}

		// rotate image and shrink it
		DisplayMetrics metrics = new DisplayMetrics();
		Display display = ((WindowManager) getSystemService(WINDOW_SERVICE)).getDefaultDisplay();
		display.getMetrics(metrics);
		Matrix mat = new Matrix();
		mat.postRotate(90);
		Bitmap bMapRotate = Bitmap.createBitmap(bMap, 0, 0, bMap.getWidth(), bMap.getHeight(), mat, true);
		bMap = Bitmap.createScaledBitmap(bMapRotate,(int)(display.getHeight()*0.25), (int) (display.getHeight()*0.5), true);

		// show it
		image.setImageBitmap(bMap);
	}

	public void convert_image(String filename_in, String filename_out, Integer warning, boolean append) throws IOException{
		FileInputStream in;
		BufferedInputStream buf;

		in = new FileInputStream(filename_in);
		FileOutputStream out = null;
		buf = new BufferedInputStream(in);
		Bitmap bMap = BitmapFactory.decodeStream(buf);

		if(bMap!=null){			 
			Bitmap bMapScaled = Bitmap.createScaledBitmap(bMap, 128, 64, true);
			int gs=0;
			int left = 0,right =0;

			for(int y=0; y<bMapScaled.getHeight(); y++){
				for(int x=0;x<bMapScaled.getWidth();x++){
					gs=((bMapScaled.getPixel(x,y)>> 16) & 0xFF)*30; // R
					gs+=((bMapScaled.getPixel(x,y)>> 8) & 0xFF)*59; // G
					gs+=(bMapScaled.getPixel(x,y) & 0xFF)*11; // B
					gs/=(30+59+11);
					gs=(int) (Math.floor(gs/16)*16);	// Reduzierung auf 16 Graustufen

					if(x%2==0){
						left=gs/16 & 0xFF;
					}
					if(x%2==1) {
						//Log.i(TAG,"bin bei x%2==1, x="+String.valueOf(x)+" y="+String.valueOf(y));
						// [0..25]/16 = [0..15]

						right=gs/16 & 0xFF;
						converted_image_buffer[(int) (64*y+Math.floor(x/2))]= (byte)((left<<4 | right) & 0xFF); // x=0,y=0 => 0 x=127,y=0 => 63
						//Log.i(TAG,"left="+String.valueOf(left)+" right="+String.valueOf(right)+" kombi: "+String.valueOf((byte)((left<<4 | right) & 0xFF)) +" pos:"+String.valueOf((int) (64*y+Math.floor(x/2))));

					}
				};
			};

			out = new FileOutputStream(filename_out,append);	
			out.write(converted_image_buffer,0,converted_image_buffer.length);
			out.close();

		} else if (warning==1) { // image konnte nicht geöffnet werden
			AlertDialog alertDialog = new AlertDialog.Builder(this).create();
			alertDialog.setTitle("Warning");
			alertDialog.setMessage("Could not open file as image!");
			alertDialog.setButton("OK",new DialogInterface.OnClickListener() {
				@Override
				public void onClick(DialogInterface arg0, int arg1) {	finish();	}});
			alertDialog.show();
		}
		if (in != null) {
			in.close();
		}
		if (buf != null) {
			buf.close();
		}
	}

	public void show_toast(String msg){
		Toast toaster=Toast.makeText(this, msg, Toast.LENGTH_SHORT);
		toaster.show();
	}

	@Override
	public void onClick(View arg0) {
		switch (arg0.getId()){
		case R.id.SaveImageConverter:

			String basedir = getIntent().getStringExtra(INPUT_DIR_PATH);
			// open File
			String result_filename = basedir+image_filename.getText().toString()+".sgf";				// 
			String input_filename = getIntent().getStringExtra(INPUT_FILE_NAME);

			// check if there are more images with the same name
			int animation_frames=0;
			String filename_without_ext=input_filename.substring(0, input_filename.lastIndexOf("."));
			String ext=input_filename.substring(input_filename.lastIndexOf("."));
			Log.i("JKW","Check filename:"+filename_without_ext);
			boolean upload_animation=false;
			if(filename_without_ext.endsWith("0")){


				//File file = getContext().getFileStreamPath(filename.substring(0, filename.length()-2));
				File file = new File(filename_without_ext.substring(0,filename_without_ext.length()-1)+String.valueOf(animation_frames)+ext);

				while(file.exists()){
					animation_frames++;
					file = new File(filename_without_ext.substring(0,filename_without_ext.length()-1)+String.valueOf(animation_frames)+ext);
				}
				animation_frames--; // einmal zurück, letzten gabs nicht mehr

				// show dialog
				//				AlertDialog alertDialog = new AlertDialog.Builder(this).create();
				//				alertDialog.setTitle("Warning");
				//				alertDialog.setMessage("found an animation, upload full animation?");
				//				alertDialog.setButton("OK",new DialogInterface.OnClickListener() {
				//					@Override
				//					public void onClick(DialogInterface arg0, int arg1) {	
				//						//upload_animation=true;	
				//						finish();
				//						}});
				//				alertDialog.show();
				show_toast("Animation found, converting and uploading complete animation");
				upload_animation=true;
				// show dialog
			} 

			if(upload_animation){
				for(int i=0;i<=animation_frames;i++){
					String input_filename_convert=filename_without_ext.substring(0,filename_without_ext.length()-1)+String.valueOf(i)+ext;
					try {						convert_image(input_filename_convert, result_filename, 1,true);	} 
					catch (IOException e) {		e.printStackTrace();								};
				};				
			} else {
				try {						convert_image(input_filename, result_filename, 1,false);	} 
				catch (IOException e) {		e.printStackTrace();								};
			};


			Log.i("JKW","Aus dem ImageEditor gebe ich den Dateinamen "+result_filename+" zurück");
			getIntent().putExtra(OUTPUT_FILE_PATH,result_filename);
			setResult(RESULT_OK, getIntent());
			finish();
			break;
		case R.id.DiscardImageConverter:
			setResult(RESULT_CANCELED, getIntent());
			finish();
			break;
		};
	};



	public static Bitmap convertToMutable(Bitmap imgIn) {
		try {
			//this is the file going to use temporally to save the bytes. 
			// This file will not be a image, it will store the raw image data.
			File file = new File(Environment.getExternalStorageDirectory() + File.separator + "temp.tmp");

			//Open an RandomAccessFile
			//Make sure you have added uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE"
			//into AndroidManifest.xml file
			RandomAccessFile randomAccessFile = new RandomAccessFile(file, "rw");

			// get the width and height of the source bitmap.
			int width = imgIn.getWidth();
			int height = imgIn.getHeight();
			Config type = imgIn.getConfig();

			//Copy the byte to the file
			//Assume source bitmap loaded using options.inPreferredConfig = Config.ARGB_8888;
			FileChannel channel = randomAccessFile.getChannel();
			MappedByteBuffer map = channel.map(MapMode.READ_WRITE, 0, imgIn.getRowBytes()*height);
			imgIn.copyPixelsToBuffer(map);
			//recycle the source bitmap, this will be no longer used.
			imgIn.recycle();
			System.gc();// try to force the bytes from the imgIn to be released

			//Create a new bitmap to load the bitmap again. Probably the memory will be available. 
			imgIn = Bitmap.createBitmap(width, height, type);
			map.position(0);
			//load it back from temporary 
			imgIn.copyPixelsFromBuffer(map);
			//close the temporary file and channel , then delete that also
			channel.close();
			randomAccessFile.close();

			// delete the temp file
			file.delete();

		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		} 

		return imgIn;
	}


}