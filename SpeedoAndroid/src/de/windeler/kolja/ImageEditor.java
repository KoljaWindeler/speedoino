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
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.util.concurrent.Semaphore;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.Matrix;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.Spinner;
import android.widget.Toast;

public class ImageEditor extends Activity implements OnClickListener{ 

	/* image converter
	 * we need to have the following functions
	 * 1) show_preview(String input_filename) showing the image on screen, input_filename must be a convertered file
	 * 2) convert_file(String input_filename, String output_filename, boolean append) converting one image
	 * 3) prepare_image(String input_filename) if sgf or graphic file just copy, otherwise extract gif file. Function returns filename
	 * 
	 * 
	 * Than do the following on start of application:
	 * temp_filename=prepare_image(input_filename);
	 * temp_filename_converted=temp_filename.SGF
	 * if(ext!=SGF)
	 * 	convert_file(temp_filename,temp_filename_converted,false) 
	 * show_preview(temp_filename_converted)
	 * 
	 * 
	 * And if you have a animation and want to see the next frame
	 * temp_filename_converted=temp_filename_1.SGF
	 * convert_file(temp_filename,temp_filename_converted,false)
	 * show_preview(temp_filename_converted)
	 * 
	 * 
	 * And if you have changed the scaling mode
	 * change var and 
	 * convert_file(temp_filename,temp_filename_converted,false)
	 * show_preview(temp_filename_converted)
	 * 
	 * 
	 * And on upload:
	 * if(multiple_file_just_different_ending) 
	 * 		count_frames
	 * 		delete_destination if exists
	 * 		for(
	 * 			temp_filename_converted=temp_filename_i.SGF
	 * 			convert_file(temp_filename,temp_filename_converted,true)
	 * 		}
	 * } else {
	 * 	convert_file(temp_filename,temp_filename_converted,false)
	 * }
	 * 
	 * finish and let the bluetooth class upload the file
	 */


	public static final String INPUT_FILE_NAME = "leeer"; // vollständinger input filename hoffe ich
	public static final String OUTPUT_FILE_PATH = "RESULT_PATH";
	private static final String TAG = "imgconv";
	public static String filename_of_file_ready_to_convert="";
	public static String filename_of_input_file="";

	private Button mButten;
	private EditText image_filename;

	private int scale_mode=0;
	private int invert_color_mode=0;
	private int background_color_mode=0;
	private int show_frame=0;
	private int max_frame=0;
	private int interframe_time=40;
	private byte[] converted_image_buffer = new byte[64*64];
	private convertImageDialog _convertImageDialog;
	private prepareImageDialog _prepareImageDialog;
	private boolean changing_text = false;
	private List<String> garbageList = new ArrayList<String>();
	private Handler mTimerHandle = new Handler();
	Toast toaster;

	SharedPreferences settings;
	public static final int REQUEST_SETTINGS=1;
	public static final String PREFS_NAME = "SpeedoAndroidImageEditorSettings";
	public static final String PREFS_SCALE = "scale";
	public static final String PREFS_INVERT = "invert";
	public static final String PREFS_BACK_COLOR = "back_color";



	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		// prepare layout
		setContentView(R.layout.image_editor_main);
		settings = getSharedPreferences(PREFS_NAME, 0);

		mButten = (Button) findViewById(R.id.SaveImageConverter);
		mButten.setOnClickListener(this);

		mButten = (Button) findViewById(R.id.DiscardImageConverter);
		mButten.setOnClickListener(this);

		mButten = (Button) findViewById(R.id.SettingsImageConverter);
		mButten.setOnClickListener(this);

		mButten = (Button) findViewById(R.id.RunAnimationImageConverter);
		mButten.setOnClickListener(this);
		mButten.setEnabled(false); // assuming its NOT a animation

		mButten = (Button) findViewById(R.id.StopAnimationImageConverter);
		mButten.setOnClickListener(this);
		mButten.setEnabled(false); // assuming its NOT a animation

		mButten = (Button) findViewById(R.id.LeftImageConverter);
		mButten.setOnClickListener(this);
		mButten.setEnabled(false); // assuming its NOT a animation
		mButten.setBackgroundResource(R.drawable.arrow_left_gray);

		mButten = (Button) findViewById(R.id.RightImageConverter);
		mButten.setOnClickListener(this);
		mButten.setEnabled(false); // assuming its NOT a animation
		mButten.setBackgroundResource(R.drawable.arrow_right_gray);

		// save info to member file
		filename_of_input_file = getIntent().getStringExtra(INPUT_FILE_NAME);
		String cleaned_filename = filename_of_input_file.substring(filename_of_input_file.lastIndexOf('/')+1).replaceAll("(?:[^a-z0-9A-Z]|(?<=['\"])s)","");

		int length_of_substring=cleaned_filename.length()-3; // "IAVjpg"
		if(length_of_substring>8){
			length_of_substring=8;
		}

		///////////////////////////////////////////////////////// TEXT FIELD PROCESS /////////////////////////////////////
		image_filename = (EditText)findViewById(R.id.image_filename);
		image_filename.setText(cleaned_filename.substring(0,length_of_substring));
		image_filename.addTextChangedListener(new TextWatcher() {
			@Override
			public void afterTextChanged(Editable arg0) {	};

			@Override
			public void beforeTextChanged(CharSequence arg0, int arg1,	int arg2, int arg3) { };

			@Override
			public void onTextChanged(CharSequence arg0, int start,int befor, int count) {
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
		///////////////////////////////////////////////////////// TEXT FIELD PROCESS /////////////////////////////////////
		// set vars to default values
		show_frame=0;
		scale_mode=settings.getInt(PREFS_SCALE, 0);
		invert_color_mode=settings.getInt(PREFS_INVERT, 0);
		background_color_mode=settings.getInt(PREFS_BACK_COLOR, 0);

		////////////////////////////////////////////////////// allright, now convert it ////////////////////////////
		// idea: start prepareImage,
		// if the file is an ordinary image, the preview will be shown
		// if its not openable the exeption will be catched
		// and if its a gif, the gif class will split it in temp images, show the first and
		// redirect the input filename to the temp_files
		try {		
			_prepareImageDialog = new prepareImageDialog(this);
			_prepareImageDialog.execute(); // warum müssen das alles strings sein?
		} catch (Exception e) {
			Log.e("Error reading file", e.toString());
			AlertDialog alertDialog = new AlertDialog.Builder(this).create();
			alertDialog.setTitle("Warning");
			alertDialog.setMessage("Could not open file as image! Upload anyway?");
			alertDialog.setButton("Yes",new DialogInterface.OnClickListener() {
				@Override
				public void onClick(DialogInterface arg0, int arg1) {	 
					getIntent().putExtra(OUTPUT_FILE_PATH,getIntent().getStringExtra(INPUT_FILE_NAME));
					cleanUp(getIntent().getStringExtra(INPUT_FILE_NAME));
					setResult(RESULT_OK, getIntent());
					finish();
				}});
			alertDialog.setButton2("No",new DialogInterface.OnClickListener() {
				@Override
				public void onClick(DialogInterface arg0, int arg1) {	
					cleanUp(""); 
					finish();	
				}});
			alertDialog.show();
		}
		//////////////// conversion and preview done
	}

	////////////////////////// prepareImageDialog ////////////////////////
	// prepareImageDialog will cut gif files  
	// convert the first frame OR the image (if its a static image)
	// and shows the preview of it
	protected class prepareImageDialog extends AsyncTask<String, Integer, String> {
		private Context context;
		ProgressDialog dialog;
		private String ext="";

		// just grap the content and prepare the dialog which will be updated
		public prepareImageDialog(Context cxt) {
			context = cxt;
			dialog = new ProgressDialog(context);
		}

		@Override
		protected void onPreExecute() {
			dialog.setMessage("Loading image...");
			dialog.show();
		};

		@Override
		protected String doInBackground(String... params) {
			//get the suffix of the filename
			ext=filename_of_input_file.substring(filename_of_input_file.lastIndexOf(".")).toLowerCase();
			// its a animation, return the string to the first image
			if(ext.equals(".gif")){
				// gif decoder vars 
				GifDecoder mGifDecoder;
				Bitmap mTmpBitmap;
				int status;

				// get the place where we store the temp files (frames) 
				String outputDir = getBaseContext().getCacheDir().getAbsolutePath(); // context being the Activity pointer
				String filename_without_ext=filename_of_input_file.substring(0, filename_of_input_file.lastIndexOf("."));				

				try {
					mGifDecoder = new GifDecoder();
					// read the gif file to the decoder... might take some time
					status=mGifDecoder.read(new FileInputStream(filename_of_input_file));
					if(status==0){
						for (int i = 0; i < mGifDecoder.getFrameCount(); i++) {
							// send message
							Message msg = mHandlerUpdate.obtainMessage();
							Bundle bundle = new Bundle();
							bundle.putInt("current", i+1);
							bundle.putInt("total", mGifDecoder.getFrameCount());
							msg.setData(bundle);
							mHandlerUpdate.sendMessage(msg);

							// write image to file
							mTmpBitmap = mGifDecoder.getFrame(i);
							String filename=outputDir+filename_without_ext.substring(filename_without_ext.lastIndexOf("/")+1)+"_"+String.valueOf(i)+".png";
							FileOutputStream out = new FileOutputStream(filename);
							mTmpBitmap.compress(Bitmap.CompressFormat.PNG, 96, out); // 4% compression
							garbageList.add(filename);

							// copy the filename of the first frame
							if(i==0){
								filename_of_file_ready_to_convert=filename;
							}
						}
					}
					// save the time if its slower than expected, and remember the frame count
					max_frame=mGifDecoder.getFrameCount();
					interframe_time=mGifDecoder.delay;

				} catch (FileNotFoundException e) {
					show_toast("Sorry, the app crashed,please report this!");
					e.printStackTrace();
				}
				// save the filename of the first frame, because thats the one we want to show in show_preview()
				// its already saved in the loop
			} else {
				// just copy the filename, it could be a already converted file or a simple image. Post execute should handle that
				filename_of_file_ready_to_convert=filename_of_input_file;
			}
			return "";
		}

		@Override
		protected void onPostExecute(String result) {
			// if the file is NOT a SGF file, convert it
			if(!ext.equals(".sgf")){
				String output_filename=filename_of_input_file.substring(0,filename_of_input_file.lastIndexOf("."))+".sgf";
				try {
					convert_image(filename_of_file_ready_to_convert, output_filename, 0, false);
					show_preview(output_filename);
				} catch (IOException e) {
					show_toast("Sorry, the app crashed,please report this!");
					e.printStackTrace();
				}
			} 
			// was already a SGF file
			else {
				show_preview(filename_of_file_ready_to_convert);
			}
			// remove the dialog, to show the original layout
			dialog.dismiss();
			// activate buttons if neccesary
			if(max_frame>1){
				((Button) findViewById(R.id.RunAnimationImageConverter)).setEnabled(true);
				((Button) findViewById(R.id.StopAnimationImageConverter)).setEnabled(true);
				((Button) findViewById(R.id.RightImageConverter)).setEnabled(true);
				((Button) findViewById(R.id.RightImageConverter)).setBackgroundResource(R.drawable.arrow_right);
			}
		}

		// the dialog updater
		private final Handler mHandlerUpdate = new Handler() {
			@Override
			public void handleMessage(Message msg) {
				dialog.setMessage("Cutting image from animation\n"+msg.getData().getInt("current")+" of "+msg.getData().getInt("total")+" images cutted");
				dialog.setProgress(100*msg.getData().getInt("current")/msg.getData().getInt("total"));				
			};
		};
	}


	// this just shows a already converted image
	public void show_preview(String filename_of_converted_file){
		// it doesn't matter if the input was a gif, a regular image, or a speedoino file .. the 
		// resulting_inputfilename holds the Path to a converted image, ready to display it now
		ImageView image = (ImageView) findViewById(R.id.imageSpeedoPreview);
		// open it to read the result
		FileInputStream in;
		Bitmap bMap=null;
		try {
			in = new FileInputStream(filename_of_converted_file);
			bMap = Bitmap.createBitmap(128, 64, Bitmap.Config.RGB_565);
			// read datablock
			in.read(converted_image_buffer, 0, (int)(64*128*0.5)); // 64 lines, 128 cols, but just a half byte per px
			in.close();
		} catch (FileNotFoundException e) {
			show_toast("Sorry, the app crashed,please report this!");
			e.printStackTrace();
		} catch (IOException e) {
			show_toast("Sorry, the app crashed,please report this!");
			e.printStackTrace();
		}

		// copy converted raw values to bitmap
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

	// this routine gets a input and a output filename, absolute path
	// warning could be one of 0=no warning or 1=warning if source does not exists
	// the boolean value append, switches the file write option. set true for animations, but delete it in advance
	// int scale_mode: 0) scale it without keeping aspect 1) scale and keep aspect 2) crop it
	public void convert_image(String filename_in, String filename_out, Integer warning, boolean append) throws IOException{
		FileInputStream in,in_size;
		BufferedInputStream buf,buf_size;

		// open it twice, once to get the size
		in_size = new FileInputStream(filename_in);
		in = new FileInputStream(filename_in);
		buf_size = new BufferedInputStream(in_size);
		buf = new BufferedInputStream(in);

		// checkout sizes
		BitmapFactory.Options options = new BitmapFactory.Options();
		options.inJustDecodeBounds = true;
		BitmapFactory.decodeStream(buf_size,null,options);
		int imageHeight = options.outHeight;
		int imageWidth = options.outWidth;
		buf_size.close();
		in_size.close();


		// load full oder downsized image
		Bitmap bMap;
		if(imageHeight > 256 && imageWidth> 512){
			BitmapFactory.Options opts= new BitmapFactory.Options();
			if(imageHeight>imageWidth*2){
				opts.inSampleSize=imageWidth*4/128; // 4 fach zu groß
			} else {
				opts.inSampleSize=imageHeight*4/6; // 4 fach zu groß
			}
			opts.inSampleSize=4;
			bMap = BitmapFactory.decodeStream(buf,null,opts);
		}    else {
			bMap = BitmapFactory.decodeStream(buf);
		}


		//if bitmap was openable start conversion
		if(bMap!=null){
			// now we have to decide the mode to compress the image in a suiteable size (128x64)
			// 0) Scale it without keeping aspect radio
			// 1) Scale it but keep the aspect. therefor we have to fill the shorter border with "fitting" pixels, and center the scaled image
			// 2) Crop it from the existing picture. simply cut a rect 128x64 from the center of the source image

			// the resulting image
			Bitmap bMapScaled = null; 

			// backup, scale without aspect
			if(scale_mode==0 || scale_mode>2){
				bMapScaled= Bitmap.createScaledBitmap(bMap, 128, 64, true);
				//				bMapScaled=convertToMutable(bMapScaled);
			} 
			// scale it and keep aspect
			else if(scale_mode==1){
				Bitmap bMapScaledTemp;
				bMapScaled= Bitmap.createBitmap(128,64,Bitmap.Config.RGB_565);
				bMapScaled=convertToMutable(bMapScaled);

				// get limiting factor
				float factor_y=bMap.getHeight()*100/64;
				float factor_x=bMap.getWidth()*100/128;

				// image scaling
				int width=0, height=0;
				if(factor_x>factor_y){
					width=(int)(bMap.getWidth()*100/factor_x);
					height=(int)(bMap.getHeight()*100/factor_x);
				} else {
					width=(int)(bMap.getWidth()*100/factor_y);
					height=(int)(bMap.getHeight()*100/factor_y);
				}
				bMapScaledTemp=Bitmap.createScaledBitmap(bMap,width, height, true);

				// fill result image with color
				int fill_color=0;
				if(background_color_mode==0){ // as settings told ya, autocalc the background color
					long avg_color=0;
					for(int y=0; y<bMapScaledTemp.getHeight(); y++){
						for(int x=0;x<2;x++){
							avg_color+=bMapScaledTemp.getPixel(x,y)& 0xFF; // blue channel, but in grayscale just grayscale
						}
					}
					avg_color/=(bMapScaledTemp.getHeight()*2);
					// if its brighter than half ... turn it on
					if(avg_color>128){
						fill_color=255;
					}
				} else if(background_color_mode==1){ // as settings told ya, overwrite it with BLACK
					fill_color=0;
				} else { // as settings told ya, overwrite it with WHITE
					fill_color=255;
				}

				// color background 
				bMapScaled.eraseColor(Color.rgb(fill_color,fill_color,fill_color));

				// copy source image to this bitmap now
				// is there a better way to copy images into another image in a centered position?
				int dest_x=0,dest_y=0;
				int offset_x=0,offset_y=0;
				offset_x=(128-bMapScaledTemp.getWidth())/2;
				offset_y=(64-bMapScaledTemp.getHeight())/2;
				for(int x=0; x<bMapScaledTemp.getWidth(); x++){
					for(int y=0; y<bMapScaledTemp.getHeight(); y++){
						dest_x=offset_x+x;
						dest_y=offset_y+y;
						bMapScaled.setPixel(dest_x,dest_y,bMapScaledTemp.getPixel(x,y));
					};
				};
			} 
			// crop it
			else if(scale_mode==2) { 
				// create bitmap
				bMapScaled= Bitmap.createBitmap(128,64,Bitmap.Config.RGB_565);
				bMapScaled=convertToMutable(bMapScaled);

				int source_x=(bMap.getWidth()-128)/2;
				int source_y=(bMap.getHeight()-64)/2;
				int width=128;
				int height=64;
				boolean filling_needed=false;
				if(source_x<0){ // the source is just higher, but not wider
					source_x=0;
					width=bMap.getWidth();
					filling_needed=true;
				}
				if(source_y<0){ // sourch just no as high es needed
					source_y=0;
					height=bMap.getHeight();
					filling_needed=true;
				}

				if(filling_needed){
					// fill result image with color
					Bitmap bMapScaledTemp=Bitmap.createBitmap(bMap, source_x, source_y, width, height); // this one is smaller than 128x64
					long avg_color=0;
					int fill_color=0;
					for(int y=0; y<bMapScaledTemp.getHeight(); y++){
						for(int x=0;x<2;x++){
							avg_color+=bMapScaledTemp.getPixel(x,y)& 0xFF; // blue channel, but in grayscale just grayscale
						}
					}
					avg_color/=(bMapScaledTemp.getHeight()*2);
					// if its brighter than half ... turn it on
					if(avg_color>128){
						fill_color=255;
					}
					// color background  
					bMapScaled.eraseColor(Color.rgb(fill_color,fill_color,fill_color));

					// copy source image to this bitmap now
					int dest_x=0,dest_y=0;
					int offset_x=0,offset_y=0;
					offset_x=(128-bMapScaledTemp.getWidth())/2;
					offset_y=(64-bMapScaledTemp.getHeight())/2;
					for(int x=0; x<bMapScaledTemp.getWidth(); x++){
						for(int y=0; y<bMapScaledTemp.getHeight(); y++){
							dest_x=offset_x+x;
							dest_y=offset_y+y;
							bMapScaled.setPixel(dest_x,dest_y,bMapScaledTemp.getPixel(x,y));
						};
					};
				} else {
					// crop out a temp image
					bMapScaled= Bitmap.createBitmap(bMap, source_x, source_y, width, height);
				}
			}


			// convertion to color shema
			int gs=0;
			int left = 0,right =0;
			for(int y=0; y<bMapScaled.getHeight(); y++){
				for(int x=0;x<bMapScaled.getWidth();x++){
					gs=((bMapScaled.getPixel(x,y)>> 16) & 0xFF)*30; // R
					gs+=((bMapScaled.getPixel(x,y)>> 8) & 0xFF)*59; // G
					gs+=(bMapScaled.getPixel(x,y) & 0xFF)*11; // B
					gs/=(30+59+11);
					gs=(int) (Math.floor(gs/16)*16);	// Reduzierung auf 16 Graustufen
					if(invert_color_mode==1){
						gs=255-gs;
					}

					if(x%2==0){
						left=(gs/16) & 0xFF;
					}
					if(x%2==1) {
						//Log.i(TAG,"bin bei x%2==1, x="+String.valueOf(x)+" y="+String.valueOf(y));
						// [0..25]/16 = [0..15]

						right=(gs/16) & 0xFF;
						converted_image_buffer[(int) (64*y+Math.floor(x/2))]= (byte)((left<<4 | right) & 0xFF); // x=0,y=0 => 0 x=127,y=0 => 63
						//Log.i(TAG,"left="+String.valueOf(left)+" right="+String.valueOf(right)+" kombi: "+String.valueOf((byte)((left<<4 | right) & 0xFF)) +" pos:"+String.valueOf((int) (64*y+Math.floor(x/2))));

					}
				};
			};

			// write to file
			FileOutputStream out = new FileOutputStream(filename_out,append);
			garbageList.add(filename_out);
			out.write(converted_image_buffer,0,converted_image_buffer.length);
			out.close();

		} else if (warning==1) { // image konnte nicht geöffnet werden
			AlertDialog alertDialog = new AlertDialog.Builder(this).create();
			alertDialog.setTitle("Warning");
			alertDialog.setMessage("Could not open file as image!");
			alertDialog.setButton("OK",new DialogInterface.OnClickListener() {
				@Override
				public void onClick(DialogInterface arg0, int arg1) {	
					cleanUp(""); 
					finish();	
				}});
			alertDialog.show();
		}
		if (buf != null) {
			buf.close();
		}
		if (in != null) {
			in.close();
		}
	}

	// just to have only one toast
	public void show_toast(String msg){
		toaster=Toast.makeText(this, msg, Toast.LENGTH_SHORT);
		toaster.show();
	}

	// possible buttons: left,right, play, stop for the animation
	// cancle (discard) and upload for ending the dialog
	// settings to open up the dialog
	@Override
	public void onClick(View arg0) {
		if(arg0.getId()==R.id.SaveImageConverter){
			// open File
			//String result_filename = basedir+image_filename.getText().toString()+".sgf";
			String input_filename = filename_of_file_ready_to_convert;
			String output_filename=filename_of_input_file.substring(0,filename_of_input_file.lastIndexOf("/")+1)+((EditText)findViewById(R.id.image_filename)).getText()+".sgf";

			// check if there are more images with the same name
			String filename_without_ext=input_filename.substring(0, input_filename.lastIndexOf("."));
			String ext=input_filename.substring(input_filename.lastIndexOf("."));

			// not jet converted .. do it
			if(!ext.toLowerCase(Locale.US).equals(".sgf")){
				Log.i(TAG,"Check filename:"+filename_without_ext);
				// prepare filename
				String convert_filename;

				// animation
				if(max_frame>1){
					convert_filename=filename_without_ext.substring(0,filename_without_ext.length()-1); // -1 to remove the "0"
				} 
				// single image
				else {
					convert_filename=input_filename;
					ext=""; // set extention to empty to signalize that this is a single image
				};

				// run conversion
				getIntent().putExtra(OUTPUT_FILE_PATH,output_filename);
				_convertImageDialog = new convertImageDialog(this);
				_convertImageDialog.execute(convert_filename,ext,String.valueOf(max_frame),output_filename); // warum müssen das alles strings sein?
			} else {
				// it was already converted .. tell it to the upload
				cleanUp(output_filename);
				getIntent().putExtra(OUTPUT_FILE_PATH,output_filename);
				setResult(RESULT_OK, getIntent());
				finish();
			}
			// dont write anything down here, because it will be executed WHILE the conversion is running .. exept you want that!

		} else if(arg0.getId()==R.id.DiscardImageConverter){
			setResult(RESULT_CANCELED, getIntent());
			cleanUp(""); // empty argument means dont prevent any file from beeing deleted
			finish();

		} else if(arg0.getId()==R.id.RightImageConverter){
			show_frame++;
			String filename_of_frame=filename_of_file_ready_to_convert.substring(0,filename_of_file_ready_to_convert.lastIndexOf("_")+1);

			try {
				convert_image(filename_of_frame+String.valueOf(show_frame)+".png", filename_of_frame+String.valueOf(show_frame)+".sgf", 0, false);
				show_preview(filename_of_frame+String.valueOf(show_frame)+".sgf");
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

			// take a look if we should deactivate the buttons or even activate one
			if(show_frame>=max_frame){
				mButten = (Button) findViewById(R.id.RightImageConverter);
				mButten.setEnabled(false);
				mButten.setBackgroundResource(R.drawable.arrow_right_gray);
			}
			if(show_frame>0){	
				mButten = (Button) findViewById(R.id.LeftImageConverter);
				mButten.setEnabled(true);
				mButten.setBackgroundResource(R.drawable.arrow_left);
			}

		} else if(arg0.getId()==R.id.LeftImageConverter){
			if(show_frame>0){
				show_frame--;
			}
			String filename_of_frame=filename_of_file_ready_to_convert.substring(0,filename_of_file_ready_to_convert.lastIndexOf("_")+1);

			try {
				// copy from png to sgf, and than show it
				convert_image(filename_of_frame+String.valueOf(show_frame)+".png", filename_of_frame+String.valueOf(show_frame)+".sgf", 0, false);
				show_preview(filename_of_frame+String.valueOf(show_frame)+".sgf");
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

			// take a look if we should deactivate the buttons or even activate one, || if we are one 0 -> no left || if we are on max -> no right ||
			if(show_frame<1){
				mButten = (Button) findViewById(R.id.LeftImageConverter);
				mButten.setEnabled(false);
				mButten.setBackgroundResource(R.drawable.arrow_left_gray);
			}
			if(show_frame<max_frame){	
				mButten = (Button) findViewById(R.id.RightImageConverter);
				mButten.setEnabled(true);
				mButten.setBackgroundResource(R.drawable.arrow_right);
			}
		} else if(arg0.getId()==R.id.SettingsImageConverter){
			Intent intent = new Intent(getBaseContext(), ImageEditorSettings.class);
			startActivityForResult(intent, REQUEST_SETTINGS);
		} else if(arg0.getId()==R.id.RunAnimationImageConverter){
			mTimerHandle.removeCallbacks(mShowAnimationTimeTask);
			show_frame=0; // this will jump to the start
			mTimerHandle.postDelayed(mShowAnimationTimeTask, 50);
		} else if(arg0.getId()==R.id.StopAnimationImageConverter){
			mTimerHandle.removeCallbacks(mShowAnimationTimeTask);
			show_frame=max_frame; // this will jump to the end
			mShowAnimationTimeTask.run();
		}
	};

	// return from the settings dialog .. 
	// recall settings and save it to the members
	// and after that, redraw it, so the users see's the change
	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		Log.d(TAG, "onActivityResult " + resultCode + " " + requestCode);
		switch (requestCode) {
		case REQUEST_SETTINGS:
			scale_mode=settings.getInt(PREFS_SCALE, 0);
			invert_color_mode=settings.getInt(PREFS_INVERT, 0);
			background_color_mode=settings.getInt(PREFS_BACK_COLOR, 0);
			String output_filename=filename_of_file_ready_to_convert.substring(0,filename_of_file_ready_to_convert.lastIndexOf("."))+".sgf";
			try {
				convert_image(filename_of_file_ready_to_convert, output_filename, 0, false);
				show_preview(output_filename);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			break;
		default:
			Log.i(TAG, "nicht gut, keine ActivityResultHandle gefunden");
			break;
		}
	}

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


	// This class should convert the images for us
	// calling paramter:
	// params[0] is the filename
	// params[1] indicates if its a animation or a single image. IF its a animation, it holds the extension. Otherwise its empty 
	// params[2] String casted number of frames, if animation
	// params[3] resulting filename
	protected class convertImageDialog extends AsyncTask<String, Integer, String> {
		private Context context;
		ProgressDialog dialog;
		String stage_1="";
		String stage_2="";

		// just grap the content and prepare the dialog which will be updated
		public convertImageDialog(Context cxt) {
			context = cxt;
			dialog = new ProgressDialog(context);
		}

		@Override
		protected String doInBackground(String... params) {
			Log.i(TAG,"starte convertierung");
			try {
				Message msg;
				Bundle bundle;
				String filename=params[0];
				String extension=params[1];
				int frames=Integer.parseInt(params[2]);
				String result_filename=params[3];

				// wenn er nicht leer ist, ists eine animation
				if(!extension.equals("")){  

					// check if file exists and delete it if so
					File target_file = new File(result_filename);
					if(target_file.exists()){
						target_file.delete();
					}

					// convert all files, one by one and append it
					for(int i=0;i<frames;i++){
						String input_filename_convert=filename+String.valueOf(i)+extension;

						msg=mHandlerUpdate.obtainMessage();
						bundle = new Bundle();
						bundle.putInt("current", i);
						bundle.putInt("total", frames);
						bundle.putInt("stage", 1);
						msg.setData(bundle);
						mHandlerUpdate.sendMessage(msg);

						convert_image(input_filename_convert, result_filename, 1,true);
					};
					// a simple single image
				} else {
					convert_image(filename, result_filename, 1,false); 
				}
				// clean all temp files
				msg=mHandlerUpdate.obtainMessage();
				bundle = new Bundle();
				bundle.putInt("stage", 2);
				msg.setData(bundle);
				mHandlerUpdate.sendMessage(msg);
				cleanUp(result_filename);

				// close the hole app, because we are done
				Log.i(TAG,"Aus dem ImageEditor gebe ich den Dateinamen "+result_filename+" zurück");

			}
			catch (IOException e) {		
				e.printStackTrace();								
			};

			// now its converted .. tell it to upload
			setResult(RESULT_OK, getIntent());
			return "japp";
		}

		@Override
		protected void onPreExecute() {
			dialog.setMessage("converting image ...");
			dialog.show();
		};

		@Override
		protected void onPostExecute(String result) {
			dialog.dismiss();
			// close imageEditor
			finish();
		}

		// the dialog updater
		private final Handler mHandlerUpdate = new Handler() {
			@Override
			public void handleMessage(Message msg) {
				if(msg.getData().getInt("stage")==1){
					stage_1="";
					if(msg.getData().getInt("total")>1){
						stage_1="Animation found\n";
					}
					stage_1+="Converting frame "+msg.getData().getInt("current")+" of "+msg.getData().getInt("total");
					dialog.setMessage(stage_1);
					dialog.setProgress(100*msg.getData().getInt("current")/msg.getData().getInt("total"));				
				} else {
					stage_2="Deleting temp files...";
					if(!stage_1.equals("")){
						stage_2=stage_1+"\n"+stage_2;
					}
					dialog.setMessage(stage_2);
				}
			};
		};
	}

	// delete all files we have just generated ..
	public void cleanUp(String preventThisFileFromBeingDeleted){
		File tempFile=null;
		for(int i=0; i<garbageList.size(); i++){
			if(!garbageList.get(i).equals(preventThisFileFromBeingDeleted)){
				tempFile=new File(garbageList.get(i));
				if(tempFile.exists()){
					tempFile.delete();
				};
			}
		}
	}

	// thread to run the animation
	private Runnable mShowAnimationTimeTask = new Runnable() {
		public void run() {
			boolean show_another_frame=false;
			mTimerHandle.removeCallbacks(mShowAnimationTimeTask);
			// show_frame 0...10, max_frame = 11 
			if(show_frame<(max_frame-1)){
				show_frame++;				
				show_another_frame=true;
			} else {
				show_frame=0;
			}

			// convert and show it
			String filename_of_frame=filename_of_file_ready_to_convert.substring(0,filename_of_file_ready_to_convert.lastIndexOf("_")+1);

			// show the next frame
			try {
				convert_image(filename_of_frame+String.valueOf(show_frame)+".png", filename_of_frame+String.valueOf(show_frame)+".sgf", 0, false);
				show_preview(filename_of_frame+String.valueOf(show_frame)+".sgf");
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

			//recall me
			if(show_another_frame){
				mTimerHandle.postDelayed(mShowAnimationTimeTask, interframe_time); //25fps
			}
		}
	};


}