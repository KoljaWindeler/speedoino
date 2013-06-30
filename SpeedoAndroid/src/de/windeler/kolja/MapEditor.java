package de.windeler.kolja;

// MAP editor => zum ertellen von Touren!

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.HashMap;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.AdapterView.OnItemClickListener;

public class MapEditor extends Activity implements OnClickListener{
	public static final String INPUT_FILE_NAME = "leeer";
	public static final String INPUT_DIR_PATH = "leer";
	public static final String OUTPUT_FILE_PATH = "RESULT_PATH";
	private static final String TAG = "MAPconv";
	private String cleaned_filename;
	private Toast toaster;
	private int reject_toaster=1;
	private int alert_result=0;

	private ArrayList<HashMap<String, Object>> mList = new ArrayList<HashMap<String, Object>>();
	private static final String ITEM_KEY = "key";
	private static final String ITEM_KEY_LOWER = "key_lower";
	private static final String ITEM_IMAGE = "image";

	private boolean changing_text = false;
	ArrayList<String> turnpoints_unmodified = new ArrayList<String>();
	ArrayList<String> turnpoints_modified = new ArrayList<String>();
	ArrayList<String> turnpoints_coordinats_unmodified = new ArrayList<String>();
	ArrayList<String> turnpoints_coordinats_modified = new ArrayList<String>();
	ArrayList<Integer> turnpoints_isconverted = new ArrayList<Integer>();

	private Button mSave;
	private Button mcancel;
	private Button mLeft;
	private Button mRight;
	private Button mStraight;
	private Button mNext;
	private Button mRem;
	private ListView mMapListView;
	private EditText direction_text;
	private EditText commando_text; 

	private int global_edit_index=0;
	//private byte[] converted_image_buffer = new byte[64*64];
	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.map_editor_main);		
		String filename = getIntent().getStringExtra(INPUT_FILE_NAME);


		mNext = (Button) findViewById(R.id.mapEditor_next);
		mNext.setOnClickListener(this);
		mSave = (Button) findViewById(R.id.mapEditor_save);
		mSave.setOnClickListener(this);
		mcancel = (Button) findViewById(R.id.mapEditor_cancel);
		mcancel.setOnClickListener(this);
		mLeft = (Button) findViewById(R.id.mapEditor_left);
		mLeft.setOnClickListener(this);
		mRight = (Button) findViewById(R.id.mapEditor_right);
		mRight.setOnClickListener(this);
		mStraight = (Button) findViewById(R.id.mapEditor_straight);
		mStraight.setOnClickListener(this);
		mRem = (Button) findViewById(R.id.mapEditor_remove);
		mRem.setOnClickListener(this);
		mMapListView = (ListView) findViewById(R.id.mapEditor_list);
		direction_text = (EditText) findViewById(R.id.mapEditor_direction);
		commando_text = (EditText) findViewById(R.id.mapEditor_text);


		cleaned_filename = filename.substring(filename.lastIndexOf('/')+1).replaceAll("(?:[^a-z0-9A-Z.]|(?<=['\"])s)","");
		commando_text.addTextChangedListener(new TextWatcher() {

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
						commando_text.setText("");
						commando_text.append(clean);
						changing_text=false;
						if(reject_toaster==0){
							show_toast("Please avoid special chars as well as spaces in filename, extension will be added automaticly");
						};
					}

					if(clean.length()>10){
						changing_text=true;
						commando_text.setText("");
						commando_text.append(clean.subSequence(0, 10));
						changing_text=false;
						if(reject_toaster==0){
							show_toast("Max 10 Chars allowed");
						};
					}
				} // changing text
			} // on text change
		}); // add text change listener

		// hier erstmal array vorbereiten
		turnpoints_isconverted.clear();
		turnpoints_unmodified.clear();
		turnpoints_modified.clear();
		turnpoints_coordinats_unmodified.clear();
		turnpoints_coordinats_modified.clear();


		try {					read_kml_file(filename);	} 
		catch (IOException e) {	e.printStackTrace();	}

		parse_kml();
		Log.i(TAG,"hinter parse");


		// fill tree
		// send to display



		update_listview();
		int status=interpret_this(0,0);
		if(status==-3){
			AlertDialog alertDialog = new AlertDialog.Builder(this).create();
			alertDialog.setTitle("Warning");
			alertDialog.setMessage("Could not open file as map!");
			alertDialog.setButton("OK",new DialogInterface.OnClickListener() {
				@Override
				public void onClick(DialogInterface arg0, int arg1) {	setResult(RESULT_CANCELED, getIntent()); finish();	}});
			alertDialog.show();
		} else if(status==-2){
			interpret_this(0,1);
			show_toast("All points entered, ready to upload");
		}
	}


	private void update_listview(){
		SimpleAdapter fileList = new SimpleAdapter(getApplicationContext(), mList, R.layout.map_dialog_row,
				new String[] { ITEM_KEY, ITEM_KEY_LOWER, ITEM_IMAGE }, new int[] { R.id.mdrowtext, R.id.mdrowtext_lower, R.id.mdrowimage });

		mList.clear();
		for(int i=0;i<turnpoints_isconverted.size();i++){
			HashMap<String, Object> item = new HashMap<String, Object>();
			if(turnpoints_isconverted.get(i)==1){
				item.put(ITEM_KEY, turnpoints_modified.get(i));
				item.put(ITEM_IMAGE, R.drawable.success);
				item.put(ITEM_KEY_LOWER, turnpoints_unmodified.get(i));
			} else {
				item.put(ITEM_KEY, turnpoints_modified.get(i));
				item.put(ITEM_IMAGE, R.drawable.fail);
				item.put(ITEM_KEY_LOWER, turnpoints_unmodified.get(i));
			}

			mList.add(item);
		}


		mMapListView.setAdapter(fileList);
		mMapListView.setOnItemClickListener(new OnItemClickListener(){
			public void onItemClick(AdapterView<?> arg0, View arg1,int arg2, long arg3){
				reject_toaster=1;
				interpret_this(arg2,1);
				reject_toaster=0;
				arg1.setSelected(true);
			};
		});

	}

	private void parse_kml() {
		for(int i=0;i<turnpoints_unmodified.size();i++){
			if(turnpoints_isconverted.get(i)==0){
				// Koordinaten
				String converted_command=parse_point(turnpoints_coordinats_unmodified.get(i),turnpoints_unmodified.get(i));
				turnpoints_modified.set(i, converted_command.substring(20));
				if(converted_command.length()>20){
					turnpoints_isconverted.set(i, 1);
				} else {
					turnpoints_isconverted.set(i, 0);
				}
				turnpoints_coordinats_modified.set(i, converted_command.substring(0, 20));
				Log.i(TAG,converted_command);
			}
		};
	};

	private String parse_point(String turnpoint_coordinate,String commando) {
		Log.i(TAG,"parse Point gestart mit "+turnpoint_coordinate+" und "+commando);
		String turnpoint_coordinate_clean=turnpoint_coordinate.replaceAll("(?:[^0-9.,]|(?<=['\"])s)","");
		int index1=turnpoint_coordinate_clean.indexOf(',');
		int index2=turnpoint_coordinate_clean.indexOf(',',index1+1);

		if(index1>turnpoint_coordinate_clean.length() || index2>turnpoint_coordinate_clean.length()){
			Log.e(TAG,"index1:"+String.valueOf(index1)+" / Index2:"+String.valueOf(index2)+" length: "+String.valueOf(turnpoint_coordinate_clean.length()));
			return "";
		}

		Float latitude=Float.valueOf(turnpoint_coordinate_clean.substring(index1+1, index2-1));
		long long_latitude=Math.round(latitude*1000000);
		Float longitude=Float.valueOf(turnpoint_coordinate_clean.substring(0, index1-1));
		long long_longitude=Math.round(longitude*1000000);
		String turnpoint_coordinate_converted="";
		if(latitude<1){
			turnpoint_coordinate_converted="000";
		} else if(latitude<10) {
			turnpoint_coordinate_converted="00";
		} else if(latitude<100) {
			turnpoint_coordinate_converted="0";
		}
		turnpoint_coordinate_converted=turnpoint_coordinate_converted+String.valueOf(long_latitude)+",";

		if(longitude<1){
			turnpoint_coordinate_converted=turnpoint_coordinate_converted+"000";
		} else if(longitude<10) {
			turnpoint_coordinate_converted=turnpoint_coordinate_converted+"00";
		} else if(longitude<100) {
			turnpoint_coordinate_converted=turnpoint_coordinate_converted+"0";
		}
		turnpoint_coordinate_converted=turnpoint_coordinate_converted+String.valueOf(long_longitude)+",";


		// right now, try to convert string
		String commando_clean_char=commando.replaceAll("ß","ss");
		commando_clean_char=commando_clean_char.replaceAll("ä","a");
		commando_clean_char=commando_clean_char.replaceAll("ö","o");
		commando_clean_char=commando_clean_char.replaceAll("ü","u");
		commando=commando_clean_char;


		boolean got_direction=false;
		ArrayList<String> command_right_left = new ArrayList<String>();
		command_right_left.clear();
		command_right_left.add("xxx auf ");
		command_right_left.add("xxx abbiegen auf ");
		command_right_left.add("Nach xxx abbiegen, um auf ");
		command_right_left.add("xxx halten auf ");
		command_right_left.add("Bei Gabelung xxx halten Weiter auf ");
		command_right_left.add("Bei Gabelung xxx halten ");


		if(!got_direction){
			for(int i=0;i<command_right_left.size();i++){
				String search_string=command_right_left.get(i).toLowerCase().replace("xxx", "rechts");
				if(commando.toLowerCase().indexOf(search_string)>-1){
					String commando_clean=commando.substring(commando.toLowerCase().indexOf(search_string)+search_string.length());
					int max_chars=10;
					String spaces=".................";
					if(commando_clean.length()<10)
						max_chars=commando_clean.length();
					turnpoint_coordinate_converted=turnpoint_coordinate_converted+"→,"+commando_clean.substring(0, max_chars)+spaces.substring(max_chars,12);
					got_direction=true;
					break;
				};
			};
		};

		if(!got_direction){
			for(int i=0;i<command_right_left.size();i++){
				String search_string=command_right_left.get(i).toLowerCase().replace("xxx", "links");
				if(commando.toLowerCase().indexOf(search_string)>-1){

					String commando_clean=commando.substring(commando.toLowerCase().indexOf(search_string)+search_string.length());
					int max_chars=10;
					String spaces=".................";
					if(commando_clean.length()<10)
						max_chars=commando_clean.length();
					turnpoint_coordinate_converted=turnpoint_coordinate_converted+"←,"+commando_clean.substring(0,max_chars)+spaces.substring(max_chars,12);
					got_direction=true;
					break;
				};
			};
		};


		ArrayList<String> command_straight = new ArrayList<String>();
		command_straight.clear();
		command_straight.add("Weiter auf ");
		command_straight.add("Geradeaus auf ");
		command_straight.add("Von ");

		if(!got_direction){
			for(int i=0;i<command_straight.size();i++){
				String search_string=command_straight.get(i).toLowerCase();
				if(commando.toLowerCase().indexOf(search_string)>-1){
					String commando_clean=commando.substring(commando.toLowerCase().indexOf(search_string)+search_string.length());
					int max_chars=10;
					String spaces=".................";
					if(commando_clean.length()<10){
						max_chars=commando_clean.length();
					}
					turnpoint_coordinate_converted=turnpoint_coordinate_converted+"↑,"+commando_clean.substring(0,max_chars)+spaces.substring(max_chars,12);

					got_direction=true;
					break;
				};
			};
		};

		return turnpoint_coordinate_converted;
	}




	private void read_kml_file(String filename) throws IOException {
		FileInputStream fStream = new FileInputStream(filename);
		BufferedReader in = new BufferedReader(new InputStreamReader(fStream));
		boolean in_point=false;
		while (in.ready()) {
			String line = in.readLine();
			// 052123456,009123456,0,text10zeic
			if(line.length()<33 && line.length()>21){	
				if(line.substring(0,8).replaceAll("(?:[^0-9]|(?<=['\"])s)","").equals(line.substring(0, 8))){
					if(line.substring(10,18).replaceAll("(?:[^0-9]|(?<=['\"])s)","").equals(line.substring(10, 18))){
						String k1,k2,k3;
						k1=line.substring(9,10);
						k2=line.substring(19,20);
						k3=line.substring(21,22);
						if(k1.equals(",") && k2.equals(",") && k3.equals(",")){
							turnpoints_coordinats_modified.add(line.substring(0, 20));
							turnpoints_coordinats_unmodified.add(line.substring(0, 20));
							turnpoints_isconverted.add(1);
							String dir_converted=line.substring(20);
							if(dir_converted.substring(0,1).equals("0")){
								dir_converted="↑"+dir_converted.substring(1);
							} else if(dir_converted.substring(0,1).equals("1")){
								dir_converted="←"+dir_converted.substring(1);
							} else if(dir_converted.substring(0,1).equals("2")){
								dir_converted="→"+dir_converted.substring(1);
							};
							turnpoints_unmodified.add(dir_converted);
							turnpoints_modified.add(dir_converted);
						}
					}
				}
			}
			if(line.indexOf("<Placemark>", 0)>-1){
				in_point=true;
			} else if(line.indexOf("</Placemark>", 0)>-1){
				in_point=false;
			};

			if(in_point){
				String search_me_start="<name>";
				String search_me_end="</name>";

				if(line.indexOf(search_me_start, 0)>-1){
					int start,end;
					start=line.indexOf(search_me_start)+search_me_start.length();
					end=line.indexOf(search_me_end);
					String line_clean=line.substring(start,end);
					turnpoints_unmodified.add(line_clean);
					turnpoints_modified.add("");
					turnpoints_isconverted.add(0);
					turnpoints_coordinats_modified.add("");
					turnpoints_coordinats_unmodified.add("");
				}

				search_me_start="<coordinates>";
				search_me_end="</coordinates>";
				if(line.indexOf(search_me_start, 0)>-1){
					int start,end;
					start=line.indexOf(search_me_start)+search_me_start.length();
					end=line.indexOf(search_me_end);
					String line_clean=line.substring(start,end);
					turnpoints_coordinats_unmodified.set(turnpoints_coordinats_unmodified.size()-1,line_clean);
				}
			}
		}
		in.close();
	}
	@Override
	public void onClick(View arg0) {
		// TODO Auto-generated method stub
		switch (arg0.getId()){
		case R.id.mapEditor_cancel:
			setResult(RESULT_CANCELED, getIntent());
			finish();
			break;
		case R.id.mapEditor_next:
			if(direction_text.getText().length()>0 && commando_text.getText().length()>0){
				String commando=direction_text.getText()+","+commando_text.getText();
				turnpoints_modified.set(global_edit_index, commando);
				turnpoints_isconverted.set(global_edit_index,1);
				update_listview();
				edit_next_point();
			} else {
				AlertDialog alertDialog = new AlertDialog.Builder(this).create();
				alertDialog.setTitle("Hmm?");
				alertDialog.setMessage("You haven't set a direction or the title is empty, loose changes and go to next point?");
				alertDialog.setButton("OK", new DialogInterface.OnClickListener(){
					@Override
					public void onClick(DialogInterface arg0, int arg1) {	edit_next_point();	}});
				alertDialog.setButton2("Cancle",new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface arg0, int arg1) { }});
				alertDialog.show();
			}
			break;

		case R.id.mapEditor_remove:
			turnpoints_coordinats_modified.remove(global_edit_index);
			turnpoints_coordinats_unmodified.remove(global_edit_index);
			turnpoints_isconverted.remove(global_edit_index);
			turnpoints_modified.remove(global_edit_index);
			turnpoints_unmodified.remove(global_edit_index);
			update_listview();
			int count_unmodified_points=0;
			for(int i=0;i<turnpoints_coordinats_modified.size();i++){
				if(turnpoints_isconverted.get(i)==0)
					count_unmodified_points++;
			};
			if(count_unmodified_points==0){
				show_toast("All points entered, ready to upload");
			} else {
				direction_text.setText("");
				reject_toaster=1;
				interpret_this(global_edit_index+1,0);
				reject_toaster=0;
				mMapListView.smoothScrollToPosition(global_edit_index);
			}
			break;
		case R.id.mapEditor_left:
			direction_text.setText("←");
			break;
		case R.id.mapEditor_right:
			direction_text.setText("→");
			break;
		case R.id.mapEditor_straight:
			direction_text.setText("↑");
			break;
		case R.id.mapEditor_save:
			int count_unmodified_points1=0;
			for(int i=0;i<turnpoints_coordinats_modified.size();i++){
				if(turnpoints_isconverted.get(i)==0)
					count_unmodified_points1++;
			};
			if(count_unmodified_points1==0){

				////////// save
				AlertDialog.Builder alert = new AlertDialog.Builder(this);                 
				alert.setTitle("Enter title");  
				alert.setMessage("one line, will be displayed in Speedoino.");                

				// Set an EditText view to get user input   
				final EditText input = new EditText(this); 
				alert.setView(input);
				//input.setText(global_text);

				alert.setPositiveButton("OK", new DialogInterface.OnClickListener() {  
					public void onClick(DialogInterface dialog, int whichButton) {  
						String value = input.getText().toString();
						try {
							String filename="";
							filename=save_to_file(value);
							getIntent().putExtra(OUTPUT_FILE_PATH,filename);
							setResult(RESULT_OK, getIntent());
						} catch (IOException e) {	
							e.printStackTrace();
							setResult(RESULT_CANCELED,getIntent());
						}
						
						finish();
						return;                  
					}  
				});  

				alert.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {

					public void onClick(DialogInterface dialog, int which) {
						// TODO Auto-generated method stub
						return;   
					}
				});
				alert.show();
				////////// save			

				Log.i(TAG,"Bis hier save");
			} else {
				Toast mToast = new Toast(this);
				mToast = Toast.makeText(getApplicationContext(), "Please complete the last "+String.valueOf(count_unmodified_points1)+" points and set their direction", Toast.LENGTH_LONG);
				mToast.show();
			}
			break;
		}

	}

	private void edit_next_point(){
		int count_unmodified_points=0;
		for(int i=0;i<turnpoints_coordinats_modified.size();i++){
			if(turnpoints_isconverted.get(i)==0)
				count_unmodified_points++;
		};
		if(count_unmodified_points==0){
			show_toast("All points entered, ready to upload");
		} else {
			direction_text.setText("");
			reject_toaster=1;
			interpret_this(global_edit_index+1,0);
			reject_toaster=0;
			mMapListView.smoothScrollToPosition(global_edit_index);
		}	
	}


	private String save_to_file(String title) throws IOException {
		// --------- generate filename ----------------//
		String basedir = getIntent().getStringExtra(INPUT_DIR_PATH);
		int max_lenght=8;
		cleaned_filename="navi0.bla";
		if(cleaned_filename.indexOf(".")<8 && cleaned_filename.indexOf(".")>0)
			max_lenght=cleaned_filename.indexOf(".");
		else if(max_lenght>cleaned_filename.length())
			max_lenght=cleaned_filename.length();
		String result_filename = basedir+cleaned_filename.substring(0, max_lenght)+".smf";				// 
		// --------- generate filename ----------------//

		//---------- save file ---------------//
		FileOutputStream out = null;
		out = new FileOutputStream(result_filename);
		String write_buffer="#d"+title+"\n";//+ description !!
		out.write(write_buffer.getBytes());
		for(int i=0;i<turnpoints_coordinats_modified.size();i++){
			write_buffer=turnpoints_coordinats_modified.get(i)+turnpoints_modified.get(i)+"\n";
			// 0=G, 1=L, 2=R
			write_buffer=write_buffer.replace('↑', '0');
			write_buffer=write_buffer.replace('←', '1');
			write_buffer=write_buffer.replace('→', '2');

			out.write(write_buffer.getBytes());
		}
		out.close();
		return result_filename;
	}


	public void show_toast(String msg){

		toaster=Toast.makeText(this, msg,100);
		toaster.show();
	}

	public int interpret_this(int edit_this, int overwrite){
		if(direction_text.getText().length()>0 && commando_text.getText().length()>0){
			String commando=direction_text.getText()+","+commando_text.getText();
			if(!commando.equals(turnpoints_modified.get(global_edit_index))){
				show_toast("changes save");
				direction_text.setText("");
				turnpoints_modified.set(global_edit_index, commando);
				turnpoints_isconverted.set(global_edit_index,1);
				update_listview(); // das ist eigentlich doof da er dann wegscrollt
				mMapListView.smoothScrollToPosition(edit_this);
			}
		}



		if(edit_this>=turnpoints_isconverted.size()){	edit_this=0;	};
		if(edit_this>=turnpoints_isconverted.size()){	return -3;		};

		if(overwrite==0){
			if(turnpoints_isconverted.get(edit_this)==1){ // wenn der schon convertiert ist, such einen anderen

				for(int i=edit_this;i<turnpoints_isconverted.size();i++){
					if(turnpoints_isconverted.get(i)==0){
						edit_this=i;
						break;
					}
				} 

				if(turnpoints_isconverted.get(edit_this)==1){ // wenn wir nach hinten hin keine mehr gefunden haben, dann nochmal von vorne gucken
					for(int i=0;i<turnpoints_isconverted.size();i++){
						if(turnpoints_isconverted.get(i)==0){
							edit_this=i;
							break;
						}
					}
				}
			}
			if(turnpoints_isconverted.get(edit_this)==1){ // wenn wir immernoch bei dem konvertierten hängen: abbruch
				return -2;
			}
		}


		// else: bearbeiten
		TextView question = (TextView)findViewById(R.id.mapEditor_question);
		EditText best_guess = (EditText)findViewById(R.id.mapEditor_text);
		EditText direction = (EditText)findViewById(R.id.mapEditor_direction);
		if(turnpoints_modified.get(edit_this).length()==0){
			best_guess.setText(turnpoints_unmodified.get(edit_this));
		} else {
			best_guess.setText(turnpoints_modified.get(edit_this));
			direction.setText(turnpoints_modified.get(edit_this).subSequence(0, 1));
		}
		//question.setText(turnpoints_unmodified.get(edit_this));

		global_edit_index=edit_this;
		return 0;
	}
}
