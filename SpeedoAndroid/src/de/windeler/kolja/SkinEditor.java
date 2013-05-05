package de.windeler.kolja;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.Spinner;
import android.widget.Toast;



public class SkinEditor extends Activity implements OnClickListener, OnItemSelectedListener{
	//# Schriftarten: dejaVuSans5ptBitmaps, sans_big, std_small, visitor_code
	public static final int  STD_SMALL_1X_FONT=1; // OK,std_small
	public static final int  STD_SMALL_2X_FONT=2; // OK,std_small
	public static final int  STD_SMALL_3X_FONT=3; // OK,std_small
	public static final int  STD_SMALL_4X_FONT=4; // OK,std_small
	public static final int  SANS_SMALL_1X_FONT=5;
	public static final int  SANS_SMALL_2X_FONT=6;
	public static final int  SANS_SMALL_3X_FONT=7;
	public static final int  SANS_SMALL_4X_FONT=8;
	public static final int  SANS_BIG_1X_FONT=9;
	public static final int  VISITOR_SMALL_1X_FONT=10; // OK,visitor_code
	public static final int  VISITOR_SMALL_2X_FONT=11; // OK,visitor_code
	public static final int  VISITOR_SMALL_3X_FONT=12; // OK,visitor_code
	public static final int  VISITOR_SMALL_4X_FONT=13; // OK,visitor_code

	private ImageView small_preview_view;
	private Bitmap construction_bitmap;
	private int x_global;
	private int y_global;
	public static final String INPUT_FILE_NAME = "leeer";
	public String global_text="";

	private int kmhchar_x,oil_x,water_x,air_x,arrow_x,kmh_x,dz_x;
	private int fuel_x,gear_x,addinfo_x,addinfo2_x,clock_x,gps_x;
	private int kmhchar_y,oil_y,water_y,air_y,arrow_y,kmh_y,dz_y;
	private int fuel_y,gear_y,addinfo_y,addinfo2_y,clock_y,gps_y;
	private int kmhchar_font,oil_font,water_font,air_font,arrow_font,kmh_font,dz_font;
	private int fuel_font,gear_font,addinfo_font,addinfo2_font,clock_font,gps_font;
	private boolean kmhchar_symbol,oil_symbol,water_symbol,air_symbol,arrow_symbol,kmh_symbol,dz_symbol;
	private boolean fuel_symbol,gear_symbol,addinfo_symbol,addinfo2_symbol,clock_symbol,gps_symbol;

	private Spinner sensor_spinner,font_spinner;
	private String selected_sensor;
	private String input_filename;
	private boolean show_addinfo2;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		// prepare layout
		setContentView(R.layout.skin_editor_main);
		small_preview_view=(ImageView) findViewById(R.id.skinEditor_imageSmallPreview);
		construction_bitmap = Bitmap.createBitmap(128, 64, Bitmap.Config.RGB_565);

		findViewById(R.id.button_up).setOnClickListener(this);
		findViewById(R.id.button_up).setBackgroundResource(R.drawable.arrow_up);
		findViewById(R.id.button_down).setOnClickListener(this);
		findViewById(R.id.button_down).setBackgroundResource(R.drawable.arrow_down);
		findViewById(R.id.button_left).setOnClickListener(this);
		findViewById(R.id.button_left).setBackgroundResource(R.drawable.arrow_left);
		findViewById(R.id.button_right).setOnClickListener(this);
		findViewById(R.id.button_right).setBackgroundResource(R.drawable.arrow_right);
		findViewById(R.id.button_save).setOnClickListener(this);
		findViewById(R.id.checkActive).setOnClickListener(this);
		findViewById(R.id.checkSymbol).setOnClickListener(this);
		findViewById(R.id.checkWarning).setOnClickListener(this);
		((CheckBox)findViewById(R.id.checkWarning)).setChecked(true);
		show_addinfo2=true;
		findViewById(R.id.checkBackground).setOnClickListener(this);


		sensor_spinner=(Spinner) findViewById(R.id.sensor_spinner);
		sensor_spinner.setAdapter(ArrayAdapter.createFromResource(this, R.array.skin_sensor_array, android.R.layout.simple_spinner_item));
		sensor_spinner.setOnItemSelectedListener(this);

		font_spinner=(Spinner) findViewById(R.id.font_spinner);
		font_spinner.setAdapter(ArrayAdapter.createFromResource(this, R.array.skin_font_array, android.R.layout.simple_spinner_item));
		font_spinner.setOnItemSelectedListener(this);

		selected_sensor=sensor_spinner.getSelectedItem().toString();
		//		if(kmh_x!=-1 && kmh_y!=-1){
		//			findViewById(R.id.checkActive).setSelected(true);
		//		}

		input_filename = getIntent().getStringExtra(INPUT_FILE_NAME);

		if(!read_skinfile(input_filename)){
			load_skin();
		}

		// nach load damit der skinner change jetzt hier den draw auftrag geben kann
		set_font_selector(kmh_font-1);
	};

	private void set_font_selector(int pos){
		if(pos>font_spinner.getCount()){
			font_spinner.setSelection(font_spinner.getCount()-1);	
		} else {
			font_spinner.setSelection(pos);
		}

	}

	private void draw_skin(){
		// fuel
		if(fuel_x!=-1 && fuel_y!=-1){
			if(fuel_symbol){
				draw_fuel(fuel_x*3,fuel_y*8); // 7 => 56
			}
			string(fuel_font,"271km",fuel_x+2,fuel_y,0,0,0);
		}

		// oil
		if(oil_x!=-1 && oil_y!=-1){
			if(oil_symbol){
				draw_oil(oil_x*3,oil_y*8); // 7 => 56
			}
			string(oil_font,"102.3{C",oil_x+4,oil_y,0,0,-4);
		}

		// water
		if(water_x!=-1 && water_y!=-1){
			if(water_symbol){
				draw_water(water_x*3,water_y*8); // 7 => 56
			}
			string(water_font,"102.3{C",water_x+4,water_y,0,0,-4);
		}

		// air
		if(air_x!=-1 && air_y!=-1){
			if(air_symbol){
				draw_air(air_x*3,air_y*8); // 7 => 56
			}
			string(air_font,"22.3{C",air_x+1,air_y,0,0,2);
		}

		// clock
		if(clock_x!=-1 && clock_y!=-1){
			if(clock_symbol){
				draw_clock(clock_x*3,clock_y*8); // 7 => 56
			}
			string(clock_font,"21:23:22",clock_x+1,clock_y,0,0,2);
		}

		// kmh
		if(kmh_x!=-1 && kmh_y!=-1){
			string(kmh_font,"213",kmh_x,kmh_y,0,0,0);
		}
		if(kmhchar_x!=-1 && kmhchar_y!=-1){
			string(kmhchar_font,"km/h",kmhchar_x,kmhchar_y,0,0,2);
		}

		// gear
		if(gear_x!=-1 && gear_y!=-1){
			string(gear_font,"6",gear_x,gear_y,0,0,2);
		}

		// dz
		if(dz_x!=-1 && dz_y!=-1){
			string(dz_font,"10650 U/min",dz_x,dz_y,0,0,0);
		}

		// addinfo
		if(addinfo_x!=-1 && addinfo_y!=-1){
			string(addinfo_font," 32799.3km | 111km/h ",addinfo_x,addinfo_y,0,0,0);
		}

		// addinfo2
		if(addinfo2_x!=-1 && addinfo2_y!=-1 && show_addinfo2){
			// highlightbar
			string(addinfo2_font,"oil temp warning",addinfo2_x+2,addinfo2_y,0,0,1);
		}

		// gps
		if(gps_x!=-1 && gps_y!=-1){
			if(gps_symbol){
				draw_gps(gps_x*3,gps_y*8);
			} else {
				// highlightbar
				string(gps_font,"12 GPS",gps_x,gps_y,0,0,0);
			}
		}

		small_preview_view.setImageBitmap(construction_bitmap);
	}

	private float parse_line(String in){
		String out=in.substring(in.indexOf("=")+1);
		out=out.replace("=","");
		out=out.replace(";","");
		float out_float=Float.parseFloat(out);
		return out_float;
	}

	private boolean read_skinfile(String filename){
		FileInputStream fStream = null;
		try {
			fStream = new FileInputStream(filename);
			BufferedReader in = new BufferedReader(new InputStreamReader(fStream));
			int line_nr=0;
			while (in.ready()) {
				String line = in.readLine();
				line_nr++;
				if(line.startsWith("#")){
					if(line_nr==1){
						global_text=line.substring(1);
					}
					continue;
				}

				// ---------- fuel -------------------- //
				else if(line.startsWith("fuel_widget.x")){
					fuel_x=(int)parse_line(line);
				} else if(line.startsWith("fuel_widget.y")){
					fuel_y=(int)parse_line(line);
				}else if(line.startsWith("fuel_widget.symbol")){
					fuel_symbol=false;
					if(parse_line(line)!=0){fuel_symbol=true;};
				}else if(line.startsWith("fuel_widget.font")){
					fuel_font=(int)parse_line(line);
				}				

				// ---------- oil -------------------- //
				else if(line.startsWith("oil_widget.x")){
					oil_x=(int)parse_line(line);
				} else if(line.startsWith("oil_widget.y")){
					oil_y=(int)parse_line(line);
				}else if(line.startsWith("oil_widget.symbol")){
					oil_symbol=false;
					if(parse_line(line)!=0){oil_symbol=true;};
				}else if(line.startsWith("oil_widget.font")){
					oil_font=(int)parse_line(line);
				}

				// ---------- water -------------------- //
				else if(line.startsWith("water_widget.x")){
					water_x=(int)parse_line(line);
				} else if(line.startsWith("water_widget.y")){
					water_y=(int)parse_line(line);
				}else if(line.startsWith("water_widget.symbol")){
					water_symbol=false;
					if(parse_line(line)!=0){water_symbol=true;};
				}else if(line.startsWith("water_widget.font")){
					water_font=(int)parse_line(line);
				}

				// ---------- air -------------------- //
				else if(line.startsWith("air_widget.x")){
					air_x=(int)parse_line(line);
				} else if(line.startsWith("air_widget.y")){
					air_y=(int)parse_line(line);
				}else if(line.startsWith("air_widget.symbol")){
					air_symbol=false;
					if(parse_line(line)!=0){air_symbol=true;};
				}else if(line.startsWith("air_widget.font")){
					air_font=(int)parse_line(line);
				}

				// ---------- clock -------------------- //
				else if(line.startsWith("clock_widget.x")){
					clock_x=(int)parse_line(line);
				} else if(line.startsWith("clock_widget.y")){
					clock_y=(int)parse_line(line);
				}else if(line.startsWith("clock_widget.symbol")){
					clock_symbol=false;
					if(parse_line(line)!=0){clock_symbol=true;};
				}else if(line.startsWith("clock_widget.font")){
					clock_font=(int)parse_line(line);
				}

				// ---------- arrow -------------------- //
				else if(line.startsWith("arrow_widget.x")){
					arrow_x=(int)parse_line(line);
				} else if(line.startsWith("arrow_widget.y")){
					arrow_y=(int)parse_line(line);
				}else if(line.startsWith("arrow_widget.symbol")){
					arrow_symbol=false;
					if(parse_line(line)!=0){arrow_symbol=true;};
				}else if(line.startsWith("arrow_widget.font")){
					arrow_font=(int)parse_line(line);
				}

				// ---------- kmh -------------------- //
				else if(line.startsWith("kmh_widget.x")){
					kmh_x=(int)parse_line(line);
				} else if(line.startsWith("kmh_widget.y")){
					kmh_y=(int)parse_line(line);
				}else if(line.startsWith("kmh_widget.symbol")){
					kmh_symbol=false;
					if(parse_line(line)!=0){kmh_symbol=true;};
				}else if(line.startsWith("kmh_widget.font")){
					kmh_font=(int)parse_line(line);
				}

				// ---------- kmhchar -------------------- //
				else if(line.startsWith("kmhchar_widget.x")){
					kmhchar_x=(int)parse_line(line);
				} else if(line.startsWith("kmhchar_widget.y")){
					kmhchar_y=(int)parse_line(line);
				}else if(line.startsWith("kmhchar_widget.symbol")){
					kmhchar_symbol=false;
					if(parse_line(line)!=0){kmhchar_symbol=true;};
				}else if(line.startsWith("kmhchar_widget.font")){
					kmhchar_font=(int)parse_line(line);
				}

				// ---------- gear -------------------- //
				else if(line.startsWith("gear_widget.x")){
					gear_x=(int)parse_line(line);
				} else if(line.startsWith("gear_widget.y")){
					gear_y=(int)parse_line(line);
				}else if(line.startsWith("gear_widget.symbol")){
					gear_symbol=false;
					if(parse_line(line)!=0){gear_symbol=true;};
				}else if(line.startsWith("gear_widget.font")){
					gear_font=(int)parse_line(line);
				}

				// ---------- dz -------------------- //
				else if(line.startsWith("dz_widget.x")){
					dz_x=(int)parse_line(line);
				} else if(line.startsWith("dz_widget.y")){
					dz_y=(int)parse_line(line);
				}else if(line.startsWith("dz_widget.symbol")){
					dz_symbol=false;
					if(parse_line(line)!=0){dz_symbol=true;};
				}else if(line.startsWith("dz_widget.font")){
					dz_font=(int)parse_line(line);
				}

				// ---------- addinfo -------------------- //
				else if(line.startsWith("addinfo_widget.x")){
					addinfo_x=(int)parse_line(line);
				} else if(line.startsWith("addinfo_widget.y")){
					addinfo_y=(int)parse_line(line);
				}else if(line.startsWith("addinfo_widget.symbol")){
					addinfo_symbol=false;
					if(parse_line(line)!=0){addinfo_symbol=true;};
				}else if(line.startsWith("addinfo_widget.font")){
					addinfo_font=(int)parse_line(line);
				}

				// ---------- addinfo2 -------------------- //
				else if(line.startsWith("addinfo2_widget.x")){
					addinfo2_x=(int)parse_line(line);
				} else if(line.startsWith("addinfo2_widget.y")){
					addinfo2_y=(int)parse_line(line);
				}else if(line.startsWith("addinfo2_widget.symbol")){
					addinfo2_symbol=false;
					if(parse_line(line)!=0){addinfo2_symbol=true;};
				}else if(line.startsWith("addinfo2_widget.font")){
					addinfo2_font=(int)parse_line(line);
				}

				// ---------- gps -------------------- //
				else if(line.startsWith("gps_widget.x")){
					gps_x=(int)parse_line(line);
				} else if(line.startsWith("gps_widget.y")){
					gps_y=(int)parse_line(line);
				}else if(line.startsWith("gps_widget.symbol")){
					gps_symbol=false;
					if(parse_line(line)!=0){gps_symbol=true;};
				}else if(line.startsWith("gps_widget.font")){
					gps_font=(int)parse_line(line);
				}
			}
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		};

		try {
			fStream.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return true;
	}

	private boolean save_skinfile(String filename, String title){
		String output="";
		output+="#"+title+"\r\n";
		output+="# Die erste Zeile wird als Beschreibung genutzt wenn sie mit einem \"#\" anfängt, bitte umlaute vermeiden.\r\n";
		output+="# Kommentarzeilen grundsätzlich mit einem \"#\" beginnen lassen und nicht hinter einem zu parsenden string\r\n";
		output+="# also nicht: \"fuel_widget.x<=14; #damit es passt\"\r\n";
		output+="# symbol= 1 für an, alles andere für aus\")\r\n";
		output+="#\r\n";
		output+="# Schriftarten:\r\n";
		output+="# STD_SMALL_1X_FONT	1\r\n";
		output+="# STD_SMALL_2X_FONT	2\r\n";
		output+="# STD_SMALL_3X_FONT	3\r\n";
		output+="# STD_SMALL_4X_FONT	4\r\n";
		output+="# SANS_SMALL_1X_FONT	5\r\n";
		output+="# SANS_SMALL_2X_FONT	6\r\n";
		output+="# SANS_SMALL_3X_FONT	7\r\n";
		output+="# SANS_SMALL_4X_FONT	8\r\n";
		output+="# SANS_BIG_1X_FONT		9\r\n";
		output+="# VISITOR_SMALL_1X_FONT	10\r\n";
		output+="# VISITOR_SMALL_2X_FONT	11\r\n";
		output+="# VISITOR_SMALL_3X_FONT	12\r\n";
		output+="# VISITOR_SMALL_4X_FONT	13\r\n";

		output+="fuel_widget.x="+String.valueOf(fuel_x)+";\r\n";
		output+="fuel_widget.y="+String.valueOf(fuel_y)+";\r\n";
		if(fuel_symbol){
			output+="fuel_widget.symbol=1;\r\n";
		} else {
			output+="fuel_widget.symbol=0;\r\n";
		}
		output+="fuel_widget.font="+String.valueOf(fuel_font)+";\r\n";


		output+="oil_widget.x="+String.valueOf(oil_x)+";\r\n";
		output+="oil_widget.y="+String.valueOf(oil_y)+";\r\n";
		if(oil_symbol){
			output+="oil_widget.symbol=1;\r\n";
		} else {
			output+="oil_widget.symbol=0;\r\n";
		}
		output+="oil_widget.font="+String.valueOf(oil_font)+";\r\n";


		output+="water_widget.x="+String.valueOf(water_x)+";\r\n";
		output+="water_widget.y="+String.valueOf(water_y)+";\r\n";
		if(water_symbol){
			output+="water_widget.symbol=1;\r\n";
		} else {
			output+="water_widget.symbol=0;\r\n";
		}
		output+="water_widget.font="+String.valueOf(water_font)+";\r\n";


		output+="air_widget.x="+String.valueOf(air_x)+";\r\n";
		output+="air_widget.y="+String.valueOf(air_y)+";\r\n";
		if(air_symbol){
			output+="air_widget.symbol=1;\r\n";
		} else {
			output+="air_widget.symbol=0;\r\n";
		}
		output+="air_widget.font="+String.valueOf(air_font)+";\r\n";


		output+="clock_widget.x="+String.valueOf(clock_x)+";\r\n";
		output+="clock_widget.y="+String.valueOf(clock_y)+";\r\n";
		if(clock_symbol){
			output+="clock_widget.symbol=1;\r\n";
		} else {
			output+="clock_widget.symbol=0;\r\n";
		}
		output+="clock_widget.font="+String.valueOf(clock_font)+";\r\n";


		output+="arrow_widget.x="+String.valueOf(arrow_x)+";\r\n";
		output+="arrow_widget.y="+String.valueOf(arrow_y)+";\r\n";


		output+="kmh_widget.x="+String.valueOf(kmh_x)+";\r\n";
		output+="kmh_widget.y="+String.valueOf(kmh_y)+";\r\n";
		output+="kmh_widget.font="+String.valueOf(kmh_font)+";\r\n";
		output+="kmhchar_widget.x="+String.valueOf(kmhchar_x)+";\r\n";
		output+="kmhchar_widget.y="+String.valueOf(kmhchar_y)+";\r\n";
		output+="kmhchar_widget.font="+String.valueOf(kmhchar_font)+";\r\n";


		output+="gear_widget.x="+String.valueOf(gear_x)+";\r\n";
		output+="gear_widget.y="+String.valueOf(gear_y)+";\r\n";
		output+="gear_widget.font="+String.valueOf(gear_font)+";\r\n";


		output+="dz_widget.x="+String.valueOf(dz_x)+";\r\n";
		output+="dz_widget.y="+String.valueOf(dz_y)+";\r\n";
		output+="dz_widget.font="+String.valueOf(dz_font)+";\r\n";


		output+="addinfo_widget.x="+String.valueOf(addinfo_x)+";\r\n";
		output+="addinfo_widget.y="+String.valueOf(addinfo_y)+";\r\n";
		output+="addinfo_widget.font="+String.valueOf(addinfo_font)+";\r\n";


		output+="addinfo2_widget.x="+String.valueOf(addinfo2_x)+";\r\n";
		output+="addinfo2_widget.y="+String.valueOf(addinfo2_y)+";\r\n";
		output+="addinfo2_widget.font="+String.valueOf(addinfo2_font)+";\r\n";


		output+="gps_widget.x="+String.valueOf(gps_x)+";\r\n";
		output+="gps_widget.y="+String.valueOf(gps_y)+";\r\n";
		output+="gps_widget.font="+String.valueOf(gps_font)+";\r\n";
		if(gps_symbol){
			output+="gps_widget.symbol=1;\r\n";
		} else {
			output+="gps_widget.symbol=0;\r\n";
		}

		output+="default_font=10;\r\n";

		FileOutputStream out = null;
		try {
			out = new FileOutputStream(filename);
			out.write(output.getBytes());
			out.close();
			return true;
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		}
	}



	private void load_skin(){
		fuel_x=14;
		fuel_y=7;
		fuel_symbol=true;
		fuel_font=VISITOR_SMALL_1X_FONT;

		oil_x=-1;
		oil_y=-1;
		oil_symbol=false;
		oil_font=VISITOR_SMALL_1X_FONT;

		water_x=0;
		water_y=0;
		water_symbol=true;
		water_font=VISITOR_SMALL_1X_FONT;

		air_x=14;
		air_y=0;
		air_symbol=true;
		air_font=VISITOR_SMALL_1X_FONT;

		clock_x=0;
		clock_y=7;
		clock_symbol=true;
		clock_font=VISITOR_SMALL_1X_FONT;

		arrow_x=0;
		arrow_y=1;

		kmh_x=6;
		kmh_y=2;
		kmh_font=VISITOR_SMALL_3X_FONT;

		kmhchar_x=15;
		kmhchar_y=4;
		kmhchar_font=VISITOR_SMALL_1X_FONT;

		gear_x=11;
		gear_y=7;
		gear_font=VISITOR_SMALL_1X_FONT;

		dz_x=0;
		dz_y=5;
		dz_font=VISITOR_SMALL_1X_FONT;

		addinfo_x=-1;
		addinfo_y=-1;
		addinfo_font=VISITOR_SMALL_1X_FONT;

		//		addinfo2_x=0;
		//		addinfo2_y=6;
		addinfo2_x=-1;
		addinfo2_y=-1;
		addinfo2_font=VISITOR_SMALL_1X_FONT;

		gps_x=14;
		gps_y=5;
		gps_symbol=true;
		gps_font=VISITOR_SMALL_1X_FONT;
	}

	@Override
	public void onClick(View arg0) {
		String[] tempArray = getResources().getStringArray(R.array.skin_sensor_array);
		// TODO Auto-generated method stub
		if(arg0.getId()==R.id.button_down){
			if(selected_sensor.equals(tempArray[0])){
				kmh_y++;
			} else if(selected_sensor.equals(tempArray[1])){
				kmhchar_y++;
			} else if(selected_sensor.equals(tempArray[2])){
				oil_y++;
			} else if(selected_sensor.equals(tempArray[3])){
				water_y++;
			} else if(selected_sensor.equals(tempArray[4])){
				air_y++;
			} else if(selected_sensor.equals(tempArray[5])){
				dz_y++;
			} else if(selected_sensor.equals(tempArray[6])){
				// arrow
			} else if(selected_sensor.equals(tempArray[7])){
				addinfo_y++;
			} else if(selected_sensor.equals(tempArray[8])){
				addinfo2_y++;
			} else if(selected_sensor.equals(tempArray[9])){
				gps_y++;
			} else if(selected_sensor.equals(tempArray[10])){
				fuel_y++;
			} else if(selected_sensor.equals(tempArray[11])){
				gear_y++;
			} else if(selected_sensor.equals(tempArray[12])){
				clock_y++;
			}

		} else if(arg0.getId()==R.id.button_up){
			if(selected_sensor.equals(tempArray[0])){
				kmh_y--;
			} else if(selected_sensor.equals(tempArray[1])){
				kmhchar_y--;
			} else if(selected_sensor.equals(tempArray[2])){
				oil_y--;
			} else if(selected_sensor.equals(tempArray[3])){
				water_y--;
			} else if(selected_sensor.equals(tempArray[4])){
				air_y--;
			} else if(selected_sensor.equals(tempArray[5])){
				dz_y--;
			} else if(selected_sensor.equals(tempArray[6])){
				// arrow
			} else if(selected_sensor.equals(tempArray[7])){
				addinfo_y--;
			} else if(selected_sensor.equals(tempArray[8])){
				addinfo2_y--;
			} else if(selected_sensor.equals(tempArray[9])){
				gps_y--;
			} else if(selected_sensor.equals(tempArray[10])){
				fuel_y--;
			} else if(selected_sensor.equals(tempArray[11])){
				gear_y--;
			} else if(selected_sensor.equals(tempArray[12])){
				clock_y--;
			}
		} else if(arg0.getId()==R.id.button_right){
			if(selected_sensor.equals(tempArray[0])){
				kmh_x++;
			} else if(selected_sensor.equals(tempArray[1])){
				kmhchar_x++;
			} else if(selected_sensor.equals(tempArray[2])){
				oil_x++;
			} else if(selected_sensor.equals(tempArray[3])){
				water_x++;
			} else if(selected_sensor.equals(tempArray[4])){
				air_x++;
			} else if(selected_sensor.equals(tempArray[5])){
				dz_x++;
			} else if(selected_sensor.equals(tempArray[6])){
				// arrow
			} else if(selected_sensor.equals(tempArray[7])){
				addinfo_x++;
			} else if(selected_sensor.equals(tempArray[8])){
				addinfo2_x++;
			} else if(selected_sensor.equals(tempArray[9])){
				gps_x++;
			} else if(selected_sensor.equals(tempArray[10])){
				fuel_x++;
			} else if(selected_sensor.equals(tempArray[11])){
				gear_x++;
			} else if(selected_sensor.equals(tempArray[12])){
				clock_x++;
			}
		} else if(arg0.getId()==R.id.button_left){
			if(selected_sensor.equals(tempArray[0])){
				kmh_x--;
			} else if(selected_sensor.equals(tempArray[1])){
				kmhchar_x--;
			} else if(selected_sensor.equals(tempArray[2])){
				oil_x--;
			} else if(selected_sensor.equals(tempArray[3])){
				water_x--;
			} else if(selected_sensor.equals(tempArray[4])){
				air_x--;
			} else if(selected_sensor.equals(tempArray[5])){
				dz_x--;
			} else if(selected_sensor.equals(tempArray[6])){
				// arrow
			} else if(selected_sensor.equals(tempArray[7])){
				addinfo_x--;
			} else if(selected_sensor.equals(tempArray[8])){
				addinfo2_x--;
			} else if(selected_sensor.equals(tempArray[9])){
				gps_x--;
			} else if(selected_sensor.equals(tempArray[10])){
				fuel_x--;
			} else if(selected_sensor.equals(tempArray[11])){
				gear_x--;
			} else if(selected_sensor.equals(tempArray[12])){
				clock_x--;
			} 
		} else if(arg0.getId()==R.id.checkSymbol){
			if(selected_sensor.equals(tempArray[0])){
				kmh_symbol=!kmh_symbol;
			} else if(selected_sensor.equals(tempArray[1])){
				kmhchar_symbol=!kmhchar_symbol;
			} else if(selected_sensor.equals(tempArray[2])){
				oil_symbol=!oil_symbol;
			} else if(selected_sensor.equals(tempArray[3])){
				water_symbol=!water_symbol;
			} else if(selected_sensor.equals(tempArray[4])){
				air_symbol=!air_symbol;
			} else if(selected_sensor.equals(tempArray[9])){
				gps_symbol=!gps_symbol;
			} else if(selected_sensor.equals(tempArray[12])){
				clock_symbol=!clock_symbol;
			}
		} else if(arg0.getId()==R.id.checkActive){
			if(selected_sensor.equals(tempArray[0])){
				if(kmh_x!=-1 && kmh_y!=-1){
					kmh_x=-1;
					kmh_y=-1;
				} else {
					kmh_x=0;
					kmh_y=0;
				}
			} else if(selected_sensor.equals(tempArray[1])){
				if(kmhchar_x!=-1 && kmhchar_y!=-1){
					kmhchar_x=-1;
					kmhchar_y=-1;
				} else {
					kmhchar_x=0;
					kmhchar_y=0;
				}
			} else if(selected_sensor.equals(tempArray[2])){
				if(oil_x!=-1 && oil_y!=-1){
					oil_x=-1;
					oil_y=-1;
				} else {
					oil_x=0;
					oil_y=0;
				}
			} else if(selected_sensor.equals(tempArray[3])){
				if(water_x!=-1 && water_y!=-1){
					water_x=-1;
					water_y=-1;
				} else {
					water_x=0;
					water_y=0;
				}
			} else if(selected_sensor.equals(tempArray[4])){
				if(air_x!=-1 && air_y!=-1){
					air_x=-1;
					air_y=-1;
				} else {
					air_x=0;
					air_y=0;
				}
			} else if(selected_sensor.equals(tempArray[5])){
				if(dz_x!=-1 && dz_y!=-1){
					dz_x=-1;
					dz_y=-1;
				} else {
					dz_x=0;
					dz_y=0;
				}
			} else if(selected_sensor.equals(tempArray[6])){
				// arrow
			} else if(selected_sensor.equals(tempArray[7])){
				if(addinfo_x!=-1 && addinfo_y!=-1){
					addinfo_x=-1;
					addinfo_y=-1;
				} else {
					addinfo_x=0;
					addinfo_y=0;
				}
			} else if(selected_sensor.equals(tempArray[8])){
				if(addinfo2_x!=-1 && addinfo2_y!=-1){
					addinfo2_x=-1;
					addinfo2_y=-1;
				} else {
					addinfo2_x=0;
					addinfo2_y=0;
				}
			} else if(selected_sensor.equals(tempArray[9])){
				if(gps_x!=-1 && gps_y!=-1){
					gps_x=-1;
					gps_y=-1;
				} else {
					gps_x=0;
					gps_y=0;
				}
			} else if(selected_sensor.equals(tempArray[10])){
				if(fuel_x!=-1 && fuel_y!=-1){
					fuel_x=-1;
					fuel_y=-1;
				} else {
					fuel_x=0;
					fuel_y=0;
				}
			} else if(selected_sensor.equals(tempArray[11])){
				if(gear_x!=-1 && gear_y!=-1){
					gear_x=-1;
					gear_y=-1;
				} else {
					gear_x=0;
					gear_y=0;
				}
			} else if(selected_sensor.equals(tempArray[12])){
				if(clock_x!=-1 && clock_y!=-1){
					clock_x=-1;
					clock_y=-1;
				} else {
					clock_x=0;
					clock_y=0;
				}
			} 
		} else if(arg0.getId()==R.id.button_save){
			AlertDialog.Builder alert = new AlertDialog.Builder(this);                 
			alert.setTitle("Enter title");  
			alert.setMessage("one line, will be displayed in Speedoino.");                

			// Set an EditText view to get user input   
			final EditText input = new EditText(this); 
			alert.setView(input);
			input.setText(global_text);

			alert.setPositiveButton("OK", new DialogInterface.OnClickListener() {  
				public void onClick(DialogInterface dialog, int whichButton) {  
					String value = input.getText().toString();
					save_skinfile(input_filename,value);
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
		} else if(arg0.getId()==R.id.checkWarning){
			if(show_addinfo2){
				// hier warnung einbauen 			
				Toast toaster=Toast.makeText(this, "Hidding Warning just in the preview, it will be visible on the bike if needed",1000);
				toaster.show();
			}
			show_addinfo2=!show_addinfo2;
		}

		clear_image();
		draw_skin();

	} 

	private void clear_image(){
		int background = Color.rgb(50, 50, 50);
		if(((CheckBox)findViewById(R.id.checkBackground)).isChecked()){
			background = Color.rgb(0, 0, 0);
		}
		construction_bitmap.eraseColor(background);
	}

	void string(int font,String str,int spalte, int zeile,int back, int color,int offset){
		for(int i=0;i<str.length();i++){
			if(
					font==SANS_SMALL_2X_FONT || 
					font==VISITOR_SMALL_2X_FONT || 
					font==STD_SMALL_2X_FONT){
				zeichen_small_2x(font,(char)str.getBytes()[i],(spalte+i*2),zeile,back,color,offset);
			} else if(
					font==SANS_SMALL_3X_FONT || 
					font==VISITOR_SMALL_3X_FONT || 
					font==STD_SMALL_3X_FONT){
				zeichen_small_3x(font,(char)str.getBytes()[i],(spalte+i*3),zeile,back,color,offset);
			} else if(
					font==SANS_SMALL_4X_FONT || 
					font==VISITOR_SMALL_4X_FONT || 
					font==STD_SMALL_4X_FONT){
				zeichen_small_4x(font,(char)str.getBytes()[i],(spalte+i*4),zeile,back,color,offset);
			}else if(
					font==SANS_BIG_1X_FONT){
				zeichen_big_1x(font,(char)str.getBytes()[i],(spalte+i*4),zeile,back,color,offset);
			}else {
				//				if(	font==SANS_SMALL_1X_FONT || 
				//					font==VISITOR_SMALL_1X_FONT || 
				//					font==STD_SMALL_1X_FONT){
				zeichen_small_1x(font,(char)str.getBytes()[i],(spalte+i),zeile,back,color,offset);
			}

		};
	};

	private void zeichen_small_4x(int font,char z, int spalte, int zeile, int back, int color, int offset){
		if(spalte>17) return;
		if(spalte<0) return;
		if(zeile>4) return;

		int stelle = 8*(z-' '); // kleinstes zeichen ist " "
		int x_start=(int)(spalte*3+Math.floor(offset/2));
		x_start*=2; // display adressiert in x richtung nur jeden 2. pixel
		int y_start=8*zeile; 

		// die zeichen sind 5x8 wobei links vom ersten buchstaben immer einer frei bleibt, daher effektiv 6 breite
		for(int y=y_start;y<y_start+32;){
			for(int d=0;d<4;d++){
				int x=x_start;
				char this_line=0x00;

				if(font==STD_SMALL_4X_FONT){
					this_line=std_small[stelle];
				} else if(font==SANS_SMALL_4X_FONT){
					this_line=dejaVuSans5ptBitmaps[stelle];
				} else if(font==VISITOR_SMALL_4X_FONT || true){
					this_line=visitor_code[stelle];
				}


				if((this_line&0x80)!=0){ 
					construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+2, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+3, y, Color.rgb(255, 255, 0));
				} else {
					construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+2, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+3, y, Color.rgb(0, 0, 0));
				}
				x+=4;

				if((this_line&0x40)!=0){ 
					construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+2, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+3, y, Color.rgb(255, 255, 0));
				} else {
					construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+2, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+3, y, Color.rgb(0, 0, 0));
				};
				x+=4;

				if((this_line&0x20)!=0){ 
					construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+2, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+3, y, Color.rgb(255, 255, 0));
				} else {
					construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+2, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+3, y, Color.rgb(0, 0, 0));
				};
				x+=4;

				if((this_line&0x10)!=0){ 
					construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+2, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+3, y, Color.rgb(255, 255, 0));
				} else {
					construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+2, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+3, y, Color.rgb(0, 0, 0));
				};
				x+=4;

				if((this_line&0x08)!=0){ 
					construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+2, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+3, y, Color.rgb(255, 255, 0));
				} else {
					construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+2, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+3, y, Color.rgb(0, 0, 0));
				};
				x+=4;
				construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
				construction_bitmap.setPixel(x+1, y, Color.rgb(0, 0, 0));
				construction_bitmap.setPixel(x+2, y, Color.rgb(0, 0, 0));
				construction_bitmap.setPixel(x+3, y, Color.rgb(0, 0, 0));
				y++;
			};
			stelle++;
		}
	}

	private void zeichen_small_3x(int font,char z, int spalte, int zeile, int back, int color, int offset){
		if(spalte>18) return;
		if(spalte<0) return;
		if(zeile>5) return;
		int stelle = 8*(z-' '); // kleinstes zeichen ist " "
		int x_start=(int)(spalte*3+Math.floor(offset/2));
		x_start*=2; // display adressiert in x richtung nur jeden 2. pixel
		int y_start=8*zeile; 

		// die zeichen sind 5x8 wobei links vom ersten buchstaben immer einer frei bleibt, daher effektiv 6 breite
		for(int y=y_start;y<y_start+24;){
			for(int d=0;d<3;d++){
				int x=x_start;
				char this_line=0x00;

				if(font==STD_SMALL_3X_FONT){
					this_line=std_small[stelle];
				} else if(font==SANS_SMALL_3X_FONT){
					this_line=dejaVuSans5ptBitmaps[stelle];
				} else if(font==VISITOR_SMALL_3X_FONT || true){
					this_line=visitor_code[stelle];
				}


				if((this_line&0x80)!=0){ 
					construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+2, y, Color.rgb(255, 255, 0));
				} else {
					construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+2, y, Color.rgb(0, 0, 0));
				};
				x+=3;

				if((this_line&0x40)!=0){ 
					construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+2, y, Color.rgb(255, 255, 0));
				} else {
					construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+2, y, Color.rgb(0, 0, 0));
				};
				x+=3;

				if((this_line&0x20)!=0){ 
					construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+2, y, Color.rgb(255, 255, 0));
				} else {
					construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+2, y, Color.rgb(0, 0, 0));
				};
				x+=3;

				if((this_line&0x10)!=0){ 
					construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+2, y, Color.rgb(255, 255, 0));
				} else {
					construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+2, y, Color.rgb(0, 0, 0));
				};
				x+=3;

				if((this_line&0x08)!=0){ 
					construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+2, y, Color.rgb(255, 255, 0));
				} else {
					construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+2, y, Color.rgb(0, 0, 0));
				};
				x+=3;
				construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
				construction_bitmap.setPixel(x+1, y, Color.rgb(0, 0, 0));
				construction_bitmap.setPixel(x+2, y, Color.rgb(0, 0, 0));
				y++;
			};
			stelle++;
		}
	}

	private void zeichen_small_2x(int font,char z, int spalte, int zeile, int back, int color, int offset){
		if(spalte>19) return;
		if(spalte<0) return;
		if(zeile>6) return;
		int stelle = 8*(z-' '); // kleinstes zeichen ist " "
		int x_start=(int)(spalte*3+Math.floor(offset/2));
		x_start*=2; // display adressiert in x richtung nur jeden 2. pixel
		int y_start=8*zeile; 

		// die zeichen sind 5x8 wobei links vom ersten buchstaben immer einer frei bleibt, daher effektiv 6 breite
		for(int y=y_start;y<y_start+16;){
			for(int d=0;d<2;d++){
				int x=x_start;
				char this_line=0x00;

				if(font==STD_SMALL_2X_FONT){
					this_line=std_small[stelle];
				} else if(font==SANS_SMALL_2X_FONT){
					this_line=dejaVuSans5ptBitmaps[stelle];
				} else if(font==VISITOR_SMALL_2X_FONT || true){
					this_line=visitor_code[stelle];
				}


				if((this_line&0x80)!=0){ 
					construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(255, 255, 0));
				} else {
					construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(0, 0, 0));
				};
				x+=2;

				if((this_line&0x40)!=0){ 
					construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(255, 255, 0));
				} else {
					construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(0, 0, 0));
				};
				x+=2;

				if((this_line&0x20)!=0){ 
					construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(255, 255, 0));
				} else {
					construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(0, 0, 0));
				};
				x+=2;

				if((this_line&0x10)!=0){ 
					construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(255, 255, 0));
				} else {
					construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(0, 0, 0));
				};
				x+=2;

				if((this_line&0x08)!=0){ 
					construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(255, 255, 0));
				} else {
					construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
					construction_bitmap.setPixel(x+1, y, Color.rgb(0, 0, 0));
				};
				x+=2;
				construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
				construction_bitmap.setPixel(x+1, y, Color.rgb(0, 0, 0));
				y++;
			};
			stelle++;
		}
	}

	private void zeichen_small_1x(int font,char z, int spalte, int zeile, int back, int color, int offset){
		if(spalte>20) return;
		if(spalte<0) return;
		if(zeile>7) return;
		int stelle = 8*(z-' '); // kleinstes zeichen ist " "
		int x_start=(int)(spalte*3+Math.floor(offset/2));
		x_start*=2; // display adressiert in x richtung nur jeden 2. pixel
		int y_start=8*zeile; 

		// die zeichen sind 5x8 wobei links vom ersten buchstaben immer einer frei bleibt, daher effektiv 6 breite
		for(int y=y_start;y<y_start+8;y++){
			int x=x_start;
			char this_line=0x00;

			if(font==STD_SMALL_1X_FONT){
				this_line=std_small[stelle];
			} else if(font==SANS_SMALL_1X_FONT){
				this_line=dejaVuSans5ptBitmaps[stelle];
			} else if(font==VISITOR_SMALL_1X_FONT || true){
				this_line=visitor_code[stelle];
			}
			stelle++;

			if((this_line&0x80)!=0){ 
				construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));	
			} else {
				construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
			};
			x++;

			if((this_line&0x40)!=0){ 
				construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));	
			} else {
				construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
			};
			x++;

			if((this_line&0x20)!=0){ 
				construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));	
			} else {
				construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
			};
			x++;

			if((this_line&0x10)!=0){ 
				construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));	
			} else {
				construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
			};
			x++;

			if((this_line&0x08)!=0){ 
				construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));	
			} else {
				construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
			};
			x++;
			construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
		}
	}

	private void zeichen_big_1x(int font,char z, int spalte, int zeile, int back, int color, int offset){
		if(spalte>17) return;
		if(zeile>4) return;
		if(spalte<0) return;

		int stelle = 96*(z-0x30);		
		int x_start=(int)(spalte*3+Math.floor(offset/2));
		x_start*=2; // display adressiert in x richtung nur jeden 2. pixel
		int y_start=8*zeile; 

		// die zeichen sind 5x8 wobei links vom ersten buchstaben immer einer frei bleibt, daher effektiv 6 breite
		for(int y=y_start;y<y_start+32;y++){
			int x=x_start;
			for(int zz=0;zz<3; zz++){
				char this_line=0x00;
				this_line=sans_big[stelle];			
				stelle++;

				if((this_line&0x80)!=0){ 
					construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));	
				} else {
					construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
				};
				x++;

				if((this_line&0x40)!=0){ 
					construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));	
				} else {
					construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
				};
				x++;

				if((this_line&0x20)!=0){ 
					construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));	
				} else {
					construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
				};
				x++;

				if((this_line&0x10)!=0){ 
					construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));	
				} else {
					construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
				};
				x++;

				if((this_line&0x08)!=0){ 
					construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));	
				} else {
					construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
				};
				x++;

				if((this_line&0x04)!=0){ 
					construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));	
				} else {
					construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
				};
				x++;

				if((this_line&0x02)!=0){ 
					construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));	
				} else {
					construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
				};
				x++;

				if((this_line&0x01)!=0){ 
					construction_bitmap.setPixel(x, y, Color.rgb(255, 255, 0));	
				} else {
					construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
				};
				x++;
			}
			construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
			x++;
			construction_bitmap.setPixel(x, y, Color.rgb(0, 0, 0));
			x++;
		}
	}

	private void draw_oil(int x, int y){
		y_global=y;
		x_global=x*2;
		send_char(0xFF); send_char(0xF0); send_char(0x0F); send_char(0xFF); send_char(0x00); send_char(0x00); send_char(0x00); send_char(0x00); send_char(0x00); send_char(0x00);
		y_global++;
		x_global=x*2;
		send_char(0xF0); send_char(0x0F); send_char(0x00); send_char(0xF0); send_char(0x00); send_char(0x00); send_char(0x0F); send_char(0xFF); send_char(0xF0); send_char(0x00);
		y_global++;
		x_global=x*2;
		send_char(0xF0); send_char(0x0F); send_char(0xFF); send_char(0xFF); send_char(0xFF); send_char(0x0F); send_char(0xF0); send_char(0xF0); send_char(0x00); send_char(0x00);
		y_global++;
		x_global=x*2;
		send_char(0x0F); send_char(0x0F); send_char(0x00); send_char(0x00); send_char(0x00); send_char(0xF0); send_char(0x0F); send_char(0x00); send_char(0xF0); send_char(0x00);
		y_global++;
		x_global=x*2;
		send_char(0x00); send_char(0xFF); send_char(0x00); send_char(0x00); send_char(0x00); send_char(0x00); send_char(0xF0); send_char(0x0F); send_char(0xFF); send_char(0x00);
		y_global++;
		x_global=x*2;
		send_char(0x00); send_char(0x0F); send_char(0x00); send_char(0x00); send_char(0x00); send_char(0x0F); send_char(0x00); send_char(0xFF); send_char(0x70); send_char(0xF0);
		y_global++;
		x_global=x*2;
		send_char(0x00); send_char(0x00); send_char(0xFF); send_char(0xFF); send_char(0xFF); send_char(0xF0); send_char(0x00); send_char(0xFF); send_char(0x77); send_char(0xF0);
		y_global++;
		x_global=x*2;
		send_char(0x00); send_char(0x00); send_char(0x00); send_char(0x00); send_char(0x00); send_char(0x00); send_char(0x00); send_char(0x0F); send_char(0xFF); send_char(0x00);
	}

	private void draw_gps(int x,int y){
		x_global=x*2;
		y_global=y;
		send_char(0x00);  send_char(0x00);  send_char(0xFF);  send_char(0x00);
		y_global++;
		x_global=x*2;
		send_char(0x00);  send_char(0x00);  send_char(0x00);  send_char(0xF0);
		y_global++;
		x_global=x*2;
		send_char(0x0F);  send_char(0x00);  send_char(0xF0);  send_char(0x0F);
		y_global++;
		x_global=x*2;
		send_char(0xF0);  send_char(0xF0);  send_char(0x0F);  send_char(0x0F);
		y_global++;
		x_global=x*2;
		send_char(0xF0);  send_char(0x0F);  send_char(0x00);  send_char(0x00);
		y_global++;
		x_global=x*2;
		send_char(0xF0);  send_char(0x00);  send_char(0xF0);  send_char(0x00);
		y_global++;
		x_global=x*2;
		send_char(0x0F);  send_char(0x00);  send_char(0x0F);  send_char(0x00);
		y_global++;
		x_global=x*2;
		send_char(0x00);  send_char(0xFF);  send_char(0xF0);  send_char(0x00);
	};

	private void draw_water(int x,int y){
		x_global=x*2;
		y_global=y;
		send_char(0x00);  send_char(0x77);  send_char(0x00);  send_char(0xFF);  send_char(0xFF);  send_char(0xFF);  send_char(0xF0);  send_char(0x00);  send_char(0x00);  send_char(0x00);
		y_global++;
		x_global=x*2;
		send_char(0x00);  send_char(0x77);  send_char(0x00);  send_char(0x00);  send_char(0x0F);  send_char(0x00);  send_char(0x00);  send_char(0x00);  send_char(0x00);  send_char(0x00);
		y_global++;
		x_global=x*2;
		send_char(0x00);  send_char(0x77);  send_char(0xF0);  send_char(0x0F);  send_char(0xFF);  send_char(0xFF);  send_char(0x00);  send_char(0x00);  send_char(0x00);  send_char(0x00);
		y_global++;
		x_global=x*2;
		send_char(0x00);  send_char(0x77);  send_char(0xFF);  send_char(0xFF);  send_char(0xFF);  send_char(0xFF);  send_char(0xFF);  send_char(0xFF);  send_char(0x00);  send_char(0x00);
		y_global++;
		x_global=x*2;
		send_char(0x00);  send_char(0x77);  send_char(0xFF);  send_char(0xFF);  send_char(0xFF);  send_char(0xFF);  send_char(0xFF);  send_char(0xFF);  send_char(0xF0);  send_char(0x00);
		y_global++;
		x_global=x*2;
		send_char(0x00);  send_char(0x77);  send_char(0xF0);  send_char(0x0F);  send_char(0xFF);  send_char(0xFF);  send_char(0x00);  send_char(0x0F);  send_char(0xF0);  send_char(0x00);
		y_global++;
		x_global=x*2;
		send_char(0x00);  send_char(0x77);  send_char(0x00);  send_char(0x00);  send_char(0x00);  send_char(0x00);  send_char(0x00);  send_char(0x0F);  send_char(0xF0);  send_char(0x00);
		y_global++;
		x_global=x*2;
		send_char(0x00);  send_char(0x77);  send_char(0x00);  send_char(0x00);  send_char(0x00);  send_char(0x00);  send_char(0x00);  send_char(0x00);  send_char(0x00);  send_char(0x00);
	};

	private void draw_air(int x,int y){
		x_global=x*2;
		y_global=y;
		send_char(0x00);  send_char(0xF0);  send_char(0x00);
		y_global++;
		x_global=x*2;
		send_char(0x00);  send_char(0xFF);  send_char(0xF0);
		y_global++;
		x_global=x*2;
		send_char(0x00);  send_char(0xF0);  send_char(0x00);
		y_global++;
		x_global=x*2;
		send_char(0x00);  send_char(0xFF);  send_char(0xF0);
		y_global++;
		x_global=x*2;
		send_char(0x00);  send_char(0xF0);  send_char(0x00);
		y_global++;
		x_global=x*2;
		send_char(0x0F);  send_char(0xFF);  send_char(0x00);
		y_global++;
		x_global=x*2;
		send_char(0xFF);  send_char(0xFF);  send_char(0xF0);
		y_global++;
		x_global=x*2;
		send_char(0x0F);  send_char(0xFF);  send_char(0x00);
	};

	private void draw_fuel(int x,int y){
		x_global=x*2;
		y_global=y;
		send_char(0x0F);  send_char(0xFF);  send_char(0xF0);  send_char(0x0F);
		y_global++;
		x_global=x*2;
		send_char(0x0F);  send_char(0x00);  send_char(0xF0);  send_char(0xF0);
		y_global++;
		x_global=x*2;
		send_char(0x0F);  send_char(0x00);  send_char(0xF0);  send_char(0xF0);
		y_global++;
		x_global=x*2;
		send_char(0x0F);  send_char(0xFF);  send_char(0xF0);  send_char(0xF0);
		y_global++;
		x_global=x*2;
		send_char(0x0F);  send_char(0xFF);  send_char(0xFF);  send_char(0x00);
		y_global++;
		x_global=x*2;
		send_char(0x0F);  send_char(0xFF);  send_char(0xF0);  send_char(0x00);
		y_global++;
		x_global=x*2;
		send_char(0xFF);  send_char(0xFF);  send_char(0xFF);  send_char(0x00);
	};

	private void draw_clock(int x,int y){
		x_global=x*2;
		y_global=y;
		send_char(0x00);  send_char(0xFF);  send_char(0xF0);  send_char(0x00);
		y_global++;
		x_global=x*2;
		send_char(0x0F);  send_char(0x0F);  send_char(0x0F);  send_char(0x00);
		y_global++;
		x_global=x*2;
		send_char(0xF0);  send_char(0x0F);  send_char(0x00);  send_char(0xF0);
		y_global++;
		x_global=x*2;
		send_char(0xF0);  send_char(0x0F);  send_char(0xF0);  send_char(0xF0);
		y_global++;
		x_global=x*2;
		send_char(0xF0);  send_char(0x00);  send_char(0x00);  send_char(0xF0);
		y_global++;
		x_global=x*2;
		send_char(0x0F);  send_char(0x00);  send_char(0x0F);  send_char(0x00);
		y_global++;
		x_global=x*2;
		send_char(0x00);  send_char(0xFF);  send_char(0xF0);  send_char(0x00);
	};

	private void send_char(int i){
		int color=16*((i&0xf0)>>4);
		construction_bitmap.setPixel(x_global, y_global, Color.rgb(color, color, 0));
		x_global++;
		color=16*(i&0x0f);
		construction_bitmap.setPixel(x_global, y_global, Color.rgb(color, color, 0));
		x_global++;		
	}

	static char std_small[] = {
		// @0 ' ' (5 pixels wide)
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //

		// @9 '!' (5 pixels wide)
		0x10, //    #
		0x10, //    #
		0x10, //    #
		0x10, //    #
		0x10, //    #
		0x00, //
		0x10, //    #
		0x00, //

		// @18 '"' (5 pixels wide)
		0x50, //  # #
		0x50, //  # #
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //

		// @27 '#' (5 pixels wide)
		0x50, //  # #
		0x50, //  # #
		0xF8, // #####
		0x50, //  # #
		0xF8, // #####
		0x50, //  # #
		0x50, //  # #
		0x00, //

		// @36 '$' (5 pixels wide)
		0x20, //   #
		0x78, //  ####
		0xA0, // # #
		0x70, //  ###
		0x28, //   # #
		0xF0, // ####
		0x20, //   #
		0x00, //

		// @45 '%' (5 pixels wide)
		0xC0, // ##
		0xC8, // ##  #
		0x10, //    #
		0x20, //   #
		0x40, //  #
		0x98, // #  ##
		0x18, //    ##
		0x00, //

		// @54 '&' (5 pixels wide)
		0x60, //  ##
		0x90, // #  #
		0xA0, // # #
		0x40, //  #
		0xA8, // # # #
		0x90, // #  #
		0x68, //  ## #
		0x00, //

		// @63 ''' (5 pixels wide)
		0x60, //  ##
		0x20, //   #
		0x40, //  #
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //

		// @72 '(' (5 pixels wide)
		0x10, //    #
		0x20, //   #
		0x40, //  #
		0x40, //  #
		0x40, //  #
		0x20, //   #
		0x10, //    #
		0x00, //

		// @81 ')' (5 pixels wide)
		0x40, //  #
		0x20, //   #
		0x10, //    #
		0x10, //    #
		0x10, //    #
		0x20, //   #
		0x40, //  #
		0x00, //

		// @90 '*' (5 pixels wide)
		0x00, //
		0x20, //   #
		0xA8, // # # #
		0x70, //  ###
		0xA8, // # # #
		0x20, //   #
		0x00, //
		0x00, //

		// @99 '+' (5 pixels wide)
		0x00, //
		0x20, //   #
		0x20, //   #
		0xF8, // #####
		0x20, //   #
		0x20, //   #
		0x00, //
		0x00, //

		// @108 ',' (5 pixels wide)
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x60, //  ##
		0x20, //   #
		0x40, //  #
		0x00, //

		// @117 '-' (5 pixels wide)
		0x00, //
		0x00, //
		0x00, //
		0xF8, // #####
		0x00, //
		0x00, //
		0x00, //
		0x00, //

		// @126 '.' (5 pixels wide)
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x60, //  ##
		0x60, //  ##
		0x00, //

		// @135 '/' (5 pixels wide)
		0x00, //
		0x00, //
		0x08, //     #
		0x10, //    #
		0x20, //   #
		0x40, //  #
		0x80, // #
		0x00, //

		// @144 '0' (5 pixels wide)
		0x70, //  ###
		0x88, // #   #
		0x98, // #  ##
		0xA8, // # # #
		0xC8, // ##  #
		0x88, // #   #
		0x70, //  ###
		0x00, //

		// @153 '1' (5 pixels wide)
		0x20, //   #
		0x60, //  ##
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x70, //  ###
		0x00, //

		// @162 '2' (5 pixels wide)
		0x70, //  ###
		0x88, // #   #
		0x08, //     #
		0x10, //    #
		0x20, //   #
		0x40, //  #
		0xF8, // #####
		0x00, //

		// @171 '3' (5 pixels wide)
		0xF8, // #####
		0x10, //    #
		0x20, //   #
		0x70, //  ###
		0x08, //     #
		0x88, // #   #
		0x70, //  ###
		0x00, //

		// @180 '4' (5 pixels wide)
		0x10, //    #
		0x30, //   ##
		0x50, //  # #
		0x90, // #  #
		0xF8, // #####
		0x10, //    #
		0x10, //    #
		0x00, //

		// @189 '5' (5 pixels wide)
		0xF8, // #####
		0x80, // #
		0xF0, // ####
		0x08, //     #
		0x08, //     #
		0x88, // #   #
		0x70, //  ###
		0x00, //

		// @198 '6' (5 pixels wide)
		0x30, //   ##
		0x40, //  #
		0x80, // #
		0xF0, // ####
		0x88, // #   #
		0x88, // #   #
		0x70, //  ###
		0x00, //

		// @207 '7' (5 pixels wide)
		0xF8, // #####
		0x08, //     #
		0x10, //    #
		0x20, //   #
		0x40, //  #
		0x40, //  #
		0x40, //  #
		0x00, //

		// @216 '8' (5 pixels wide)
		0x70, //  ###
		0x88, // #   #
		0x88, // #   #
		0x70, //  ###
		0x88, // #   #
		0x88, // #   #
		0x70, //  ###
		0x00, //

		// @225 '9' (5 pixels wide)
		0x70, //  ###
		0x88, // #   #
		0x88, // #   #
		0x78, //  ####
		0x08, //     #
		0x10, //    #
		0x60, //  ##
		0x00, //

		// @234 ':' (5 pixels wide)
		0x00, //
		0x60, //  ##
		0x60, //  ##
		0x00, //
		0x60, //  ##
		0x60, //  ##
		0x00, //
		0x00, //

		// @243 ';' (5 pixels wide)
		0x00, //
		0x60, //  ##
		0x60, //  ##
		0x00, //
		0x60, //  ##
		0x20, //   #
		0x40, //  #
		0x00, //

		// @252 '<' (5 pixels wide)
		0x10, //    #
		0x20, //   #
		0x40, //  #
		0x80, // #
		0x40, //  #
		0x20, //   #
		0x10, //    #
		0x00, //

		// @261 '=' (5 pixels wide)
		0x00, //
		0x00, //
		0xF8, // #####
		0x00, //
		0xF8, // #####
		0x00, //
		0x00, //
		0x00, //

		// @270 '>' (5 pixels wide)
		0x80, // #
		0x40, //  #
		0x20, //   #
		0x10, //    #
		0x20, //   #
		0x40, //  #
		0x80, // #
		0x00, //

		// @279 '?' (5 pixels wide)
		0x70, //  ###
		0x88, // #   #
		0x08, //     #
		0x10, //    #
		0x20, //   #
		0x00, //
		0x20, //   #
		0x00, //

		// @288 '@' (5 pixels wide)
		0x70, //  ###
		0x88, // #   #
		0xB8, // # ###
		0xA8, // # # #
		0xB8, // # ###
		0x80, // #
		0x70, //  ###
		0x00, //

		// @297 'A' (5 pixels wide)
		0x70, //  ###
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0xF8, // #####
		0x88, // #   #
		0x88, // #   #
		0x00, //

		// @306 'B' (5 pixels wide)
		0xF0, // ####
		0x88, // #   #
		0x88, // #   #
		0xF0, // ####
		0x88, // #   #
		0x88, // #   #
		0xF0, // ####
		0x00, //

		// @315 'C' (5 pixels wide)
		0x70, //  ###
		0x88, // #   #
		0x80, // #
		0x80, // #
		0x80, // #
		0x88, // #   #
		0x70, //  ###
		0x00, //

		// @324 'D' (5 pixels wide)
		0xE0, // ###
		0x90, // #  #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x90, // #  #
		0xE0, // ###
		0x00, //

		// @333 'E' (5 pixels wide)
		0xF8, // #####
		0x80, // #
		0x80, // #
		0xF0, // ####
		0x80, // #
		0x80, // #
		0xF8, // #####
		0x00, //

		// @342 'F' (5 pixels wide)
		0xF8, // #####
		0x80, // #
		0x80, // #
		0xF0, // ####
		0x80, // #
		0x80, // #
		0x80, // #
		0x00, //

		// @351 'G' (5 pixels wide)
		0x70, //  ###
		0x88, // #   #
		0x80, // #
		0xB8, // # ###
		0x88, // #   #
		0x88, // #   #
		0x78, //  ####
		0x00, //

		// @360 'H' (5 pixels wide)
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0xF8, // #####
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x00, //

		// @369 'I' (5 pixels wide)
		0x70, //  ###
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x70, //  ###
		0x00, //

		// @378 'J' (5 pixels wide)
		0x38, //   ###
		0x10, //    #
		0x10, //    #
		0x10, //    #
		0x10, //    #
		0x90, // #  #
		0x60, //  ##
		0x00, //

		// @387 'K' (5 pixels wide)
		0x88, // #   #
		0x90, // #  #
		0xA0, // # #
		0xC0, // ##
		0xA0, // # #
		0x90, // #  #
		0x88, // #   #
		0x00, //

		// @396 'L' (5 pixels wide)
		0x80, // #
		0x80, // #
		0x80, // #
		0x80, // #
		0x80, // #
		0x80, // #
		0xF8, // #####
		0x00, //

		// @405 'M' (5 pixels wide)
		0x88, // #   #
		0xD8, // ## ##
		0xA8, // # # #
		0xA8, // # # #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x00, //

		// @414 'N' (5 pixels wide)
		0x88, // #   #
		0x88, // #   #
		0xC8, // ##  #
		0xA8, // # # #
		0x98, // #  ##
		0x88, // #   #
		0x88, // #   #
		0x00, //

		// @423 'O' (5 pixels wide)
		0x70, //  ###
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x70, //  ###
		0x00, //

		// @432 'P' (5 pixels wide)
		0xF0, // ####
		0x88, // #   #
		0x88, // #   #
		0xF0, // ####
		0x80, // #
		0x80, // #
		0x80, // #
		0x00, //

		// @441 'Q' (5 pixels wide)
		0x70, //  ###
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0xA8, // # # #
		0x90, // #  #
		0x68, //  ## #
		0x00, //

		// @450 'R' (5 pixels wide)
		0xF0, // ####
		0x88, // #   #
		0x88, // #   #
		0xF0, // ####
		0xA0, // # #
		0x90, // #  #
		0x88, // #   #
		0x00, //

		// @459 'S' (5 pixels wide)
		0x78, //  ####
		0x80, // #
		0x80, // #
		0x70, //  ###
		0x08, //     #
		0x08, //     #
		0xF0, // ####
		0x00, //

		// @468 'T' (5 pixels wide)
		0xF8, // #####
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x00, //

		// @477 'U' (5 pixels wide)
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x70, //  ###
		0x00, //

		// @486 'V' (5 pixels wide)
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x50, //  # #
		0x20, //   #
		0x00, //

		// @495 'W' (5 pixels wide)
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0xA8, // # # #
		0xA8, // # # #
		0xA8, // # # #
		0x50, //  # #
		0x00, //

		// @504 'X' (5 pixels wide)
		0x88, // #   #
		0x88, // #   #
		0x50, //  # #
		0x20, //   #
		0x50, //  # #
		0x88, // #   #
		0x88, // #   #
		0x00, //

		// @513 'Y' (5 pixels wide)
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x50, //  # #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x00, //

		// @522 'Z' (5 pixels wide)
		0xF8, // #####
		0x08, //     #
		0x10, //    #
		0x20, //   #
		0x40, //  #
		0x80, // #
		0xF8, // #####
		0x00, //

		// @531 '[' (5 pixels wide)
		0x70, //  ###
		0x40, //  #
		0x40, //  #
		0x40, //  #
		0x40, //  #
		0x40, //  #
		0x70, //  ###
		0x00, //

		// @540 '\' (5 pixels wide)
		0x00, //
		0x00, //
		0x80, // #
		0x40, //  #
		0x20, //   #
		0x10, //    #
		0x08, //     #
		0x00, //

		// @549 ']' (5 pixels wide)
		0x70, //  ###
		0x10, //    #
		0x10, //    #
		0x10, //    #
		0x10, //    #
		0x10, //    #
		0x70, //  ###
		0x00, //

		// @558 '^' (5 pixels wide)
		0x20, //   #
		0x50, //  # #
		0x88, // #   #
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //

		// @567 '_' (5 pixels wide)
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0xF8, // #####
		0x00, //

		// @576 '`' (5 pixels wide)
		0x30, //   ##
		0x20, //   #
		0x10, //    #
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //

		// @585 'a' (5 pixels wide)
		0x00, //
		0x00, //
		0x70, //  ###
		0x08, //     #
		0x78, //  ####
		0x88, // #   #
		0x78, //  ####
		0x00, //

		// @594 'b' (5 pixels wide)
		0x80, // #
		0x80, // #
		0xB0, // # ##
		0xC8, // ##  #
		0x88, // #   #
		0x88, // #   #
		0xF0, // ####
		0x00, //

		// @603 'c' (5 pixels wide)
		0x00, //
		0x00, //
		0x70, //  ###
		0x80, // #
		0x80, // #
		0x88, // #   #
		0x70, //  ###
		0x00, //

		// @612 'd' (5 pixels wide)
		0x08, //     #
		0x08, //     #
		0x68, //  ## #
		0x98, // #  ##
		0x88, // #   #
		0x88, // #   #
		0x78, //  ####
		0x00, //

		// @621 'e' (5 pixels wide)
		0x00, //
		0x00, //
		0x70, //  ###
		0x88, // #   #
		0xF8, // #####
		0x80, // #
		0x70, //  ###
		0x00, //

		// @630 'f' (5 pixels wide)
		0x30, //   ##
		0x48, //  #  #
		0x40, //  #
		0xF0, // ####
		0x40, //  #
		0x40, //  #
		0x40, //  #
		0x00, //

		// @639 'g' (5 pixels wide)
		0x00, //
		0x00, //
		0x70, //  ###
		0x88, // #   #
		0xF8, // #####
		0x08, //     #
		0x70, //  ###
		0x00, //

		// @648 'h' (5 pixels wide)
		0x80, // #
		0x80, // #
		0xB0, // # ##
		0xC8, // ##  #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x00, //

		// @657 'i' (5 pixels wide)
		0x20, //   #
		0x00, //
		0x60, //  ##
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x70, //  ###
		0x00, //

		// @666 'j' (5 pixels wide)
		0x10, //    #
		0x00, //
		0x30, //   ##
		0x10, //    #
		0x10, //    #
		0x90, // #  #
		0x60, //  ##
		0x00, //

		// @675 'k' (5 pixels wide)
		0x80, // #
		0x80, // #
		0x90, // #  #
		0xA0, // # #
		0xC0, // ##
		0xA0, // # #
		0x90, // #  #
		0x00, //

		// @684 'l' (5 pixels wide)
		0x60, //  ##
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x70, //  ###
		0x00, //

		// @693 'm' (5 pixels wide)
		0x00, //
		0x00, //
		0xD0, // ## #
		0xA8, // # # #
		0xA8, // # # #
		0x88, // #   #
		0x88, // #   #
		0x00, //

		// @702 'n' (5 pixels wide)
		0x00, //
		0x00, //
		0xB0, // # ##
		0xC8, // ##  #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x00, //

		// @711 'o' (5 pixels wide)
		0x00, //
		0x00, //
		0x70, //  ###
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x70, //  ###
		0x00, //

		// @720 'p' (5 pixels wide)
		0x00, //
		0x00, //
		0xF0, // ####
		0x88, // #   #
		0xF0, // ####
		0x80, // #
		0x80, // #
		0x00, //

		// @729 'q' (5 pixels wide)
		0x00, //
		0x00, //
		0x68, //  ## #
		0x98, // #  ##
		0x78, //  ####
		0x08, //     #
		0x08, //     #
		0x00, //

		// @738 'r' (5 pixels wide)
		0x00, //
		0x00, //
		0xB0, // # ##
		0xC8, // ##  #
		0x80, // #
		0x80, // #
		0x80, // #
		0x00, //

		// @747 's' (5 pixels wide)
		0x00, //
		0x00, //
		0x70, //  ###
		0x80, // #
		0x70, //  ###
		0x08, //     #
		0xF0, // ####
		0x00, //

		// @756 't' (5 pixels wide)
		0x40, //  #
		0xE0, // ###
		0x40, //  #
		0x40, //  #
		0x40, //  #
		0x48, //  #  #
		0x30, //   ##
		0x00, //

		// @765 'u' (5 pixels wide)
		0x00, //
		0x00, //
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x98, // #  ##
		0x68, //  ## #
		0x00, //

		// @774 'v' (5 pixels wide)
		0x00, //
		0x00, //
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x50, //  # #
		0x20, //   #
		0x00, //

		// @783 'w' (5 pixels wide)
		0x00, //
		0x00, //
		0x88, // #   #
		0x88, // #   #
		0xA8, // # # #
		0xA8, // # # #
		0x50, //  # #
		0x00, //

		// @792 'x' (5 pixels wide)
		0x00, //
		0x00, //
		0x88, // #   #
		0x50, //  # #
		0x20, //   #
		0x50, //  # #
		0x88, // #   #
		0x00, //

		// @801 'y' (5 pixels wide)
		0x00, //
		0x00, //
		0x88, // #   #
		0x88, // #   #
		0x78, //  ####
		0x08, //     #
		0x70, //  ###
		0x00, //

		// @810 'z' (5 pixels wide)
		0x00, //
		0x00, //
		0xF8, // #####
		0x10, //    #
		0x20, //   #
		0x40, //  #
		0xF8, // #####
		0x00, //

		// @819 '�' (5 pixels wide)
		0x10, //    #
		0x28, //   # #
		0x10, //    #
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //

		// @828 '|' (5 pixels wide)
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x00, //

		// @837 'Pfeil hoch' (5 pixels wide)
		0x20, //   #
		0x70, //  ###
		0xF8, // #####
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x00, //

		// @846 'Pfeil links' (5 pixels wide)
		0x20, //   #
		0x60, //  ##
		0xF8, // #####
		0x68, //  ## #
		0x28, //   # #
		0x08, //     #
		0x08, //     #
		0x00, //

		// @855 'Pfeil rechts' (5 pixels wide)
		0x20, //   #
		0x30, //   ##
		0xF8, // #####
		0xB0, // # ##
		0xA0, // # #
		0x80, // #
		0x80, // #
		0x00  //
	};

	static char visitor_code[] = {
		// @0 ' ' (5 pixels wide)
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //

		// @9 '!' (5 pixels wide)
		0x10, //    #
		0x10, //    #
		0x10, //    #
		0x10, //    #
		0x10, //    #
		0x00, //
		0x10, //    #
		0x00, //

		// @18 '"' (5 pixels wide)
		0x50, //  # #
		0x50, //  # #
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //

		// @27 '#' (5 pixels wide)
		0x00, //
		0x50, //  # #
		0xF8, // #####
		0x50, //  # #
		0xF8, // #####
		0x50, //  # #
		0x00, //
		0x00, //

		// @36 '$' (5 pixels wide)
		0x20, //   #
		0xF8, // #####
		0x80, // #
		0xF8, // #####
		0x08, //     #
		0xF8, // #####
		0x20, //   #
		0x00, //

		// @45 '%' (5 pixels wide)
		0x00, //
		0xC8, // ##  #
		0xD0, // ## #
		0x20, //   #
		0x20, //   #
		0x58, //  # ##
		0x98, // #  ##
		0x00, //

		// @54 '&' (5 pixels wide)
		0x60, //  ##
		0x90, // #  #
		0xA0, // # #
		0x78, //  ####
		0x90, // #  #
		0x98, // #  ##
		0xE8, // ### #
		0x00, //

		// @63 ''' (5 pixels wide)
		0x20, //   #
		0x20, //   #
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //

		// @72 '(' (5 pixels wide)
		0x10, //    #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x10, //    #
		0x00, //

		// @81 ')' (5 pixels wide)
		0x20, //   #
		0x10, //    #
		0x10, //    #
		0x10, //    #
		0x10, //    #
		0x10, //    #
		0x20, //   #
		0x00, //

		// @90 '*' (5 pixels wide)
		0x00, //
		0x20, //   #
		0xF8, // #####
		0x20, //   #
		0x50, //  # #
		0x00, //
		0x00, //
		0x00, //

		// @99 '+' (5 pixels wide)
		0x00, //
		0x20, //   #
		0x20, //   #
		0xF8, // #####
		0x20, //   #
		0x20, //   #
		0x00, //
		0x00, //

		// @108 ',' (5 pixels wide)
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x20, //   #
		0x20, //   #
		0x00, //

		// @117 '-' (5 pixels wide)
		0x00, //
		0x00, //
		0x00, //
		0xF8, // #####
		0x00, //
		0x00, //
		0x00, //
		0x00, //

		// @126 '.' (5 pixels wide)
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x20, //   #
		0x00, //

		// @135 '/' (5 pixels wide)
		0x00, //
		0x00, //
		0x08, //     #
		0x10, //    #
		0x20, //   #
		0x40, //  #
		0x80, // #
		0x00, //

		// @144 '0' (5 pixels wide)
		0x00, //
		0xF8, // #####
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0xF8, // #####
		0x00, //

		// @153 '1' (5 pixels wide)
		0x00, //
		0x30, //   ##
		0x10, //    #
		0x10, //    #
		0x10, //    #
		0x10, //    #
		0x10, //    #
		0x00, //

		// @162 '2' (5 pixels wide)
		0x00, //
		0xF8, // #####
		0x08, //     #
		0xF8, // #####
		0x80, // #
		0x80, // #
		0xF8, // #####
		0x00, //

		// @171 '3' (5 pixels wide)
		0x00, //
		0xF8, // #####
		0x08, //     #
		0x38, //   ###
		0x08, //     #
		0x08, //     #
		0xF8, // #####
		0x00, //

		// @180 '4' (5 pixels wide)
		0x00, //
		0x88, // #   #
		0x88, // #   #
		0xF8, // #####
		0x08, //     #
		0x08, //     #
		0x08, //     #
		0x00, //

		// @189 '5' (5 pixels wide)
		0x00, //
		0xF8, // #####
		0x80, // #
		0xF8, // #####
		0x08, //     #
		0x08, //     #
		0xF8, // #####
		0x00, //

		// @198 '6' (5 pixels wide)
		0x00, //
		0xF8, // #####
		0x80, // #
		0xF8, // #####
		0x88, // #   #
		0x88, // #   #
		0xF8, // #####
		0x00, //

		// @207 '7' (5 pixels wide)
		0x00, //
		0xF8, // #####
		0x08, //     #
		0x08, //     #
		0x08, //     #
		0x08, //     #
		0x08, //     #
		0x00, //

		// @216 '8' (5 pixels wide)
		0x00, //
		0xF8, // #####
		0x88, // #   #
		0xF8, // #####
		0x88, // #   #
		0x88, // #   #
		0xF8, // #####
		0x00, //

		// @225 '9' (5 pixels wide)
		0x00, //
		0xF8, // #####
		0x88, // #   #
		0xF8, // #####
		0x08, //     #
		0x08, //     #
		0x08, //     #
		0x00, //

		// @234 ':' (5 pixels wide)
		0x00, //
		0x00, //
		0x20, //   #
		0x00, //
		0x00, //
		0x20, //   #
		0x00, //
		0x00, //

		// @243 ';' (5 pixels wide)
		0x00, //
		0x00, //
		0x20, //   #
		0x00, //
		0x00, //
		0x20, //   #
		0x20, //   #
		0x00, //

		// @252 '<' (5 pixels wide)
		0x00, //
		0x10, //    #
		0x20, //   #
		0x40, //  #
		0x20, //   #
		0x10, //    #
		0x00, //
		0x00, //

		// @261 '=' (5 pixels wide)
		0x00, //
		0x00, //
		0xF8, // #####
		0x00, //
		0xF8, // #####
		0x00, //
		0x00, //
		0x00, //

		// @270 '>' (5 pixels wide)
		0x00, //
		0x40, //  #
		0x20, //   #
		0x10, //    #
		0x20, //   #
		0x40, //  #
		0x00, //
		0x00, //

		// @279 '?' (5 pixels wide)
		0x00, //
		0xF8, // #####
		0x08, //     #
		0x08, //     #
		0x38, //   ###
		0x00, //
		0x20, //   #
		0x00, //

		// @288 '@' (5 pixels wide)
		0x00, //
		0xF8, // #####
		0x88, // #   #
		0xB8, // # ###
		0xA8, // # # #
		0xB8, // # ###
		0x00, //
		0x00, //

		// @297 'A' (5 pixels wide)
		0x00, //
		0xF8, // #####
		0x88, // #   #
		0xF8, // #####
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x00, //

		// @306 'B' (5 pixels wide)
		0x00, //
		0xF0, // ####
		0x88, // #   #
		0xF8, // #####
		0x88, // #   #
		0x88, // #   #
		0xF0, // ####
		0x00, //

		// @315 'C' (5 pixels wide)
		0x00, //
		0xF8, // #####
		0x80, // #
		0x80, // #
		0x80, // #
		0x80, // #
		0xF8, // #####
		0x00, //

		// @324 'D' (5 pixels wide)
		0x00, //
		0xF0, // ####
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0xF0, // ####
		0x00, //

		// @333 'E' (5 pixels wide)
		0x00, //
		0xF8, // #####
		0x80, // #
		0xE0, // ###
		0x80, // #
		0x80, // #
		0xF8, // #####
		0x00, //

		// @342 'F' (5 pixels wide)
		0x00, //
		0xF8, // #####
		0x80, // #
		0xE0, // ###
		0x80, // #
		0x80, // #
		0x80, // #
		0x00, //

		// @351 'G' (5 pixels wide)
		0x00, //
		0xF8, // #####
		0x80, // #
		0xB8, // # ###
		0x88, // #   #
		0x88, // #   #
		0xF8, // #####
		0x00, //

		// @360 'H' (5 pixels wide)
		0x00, //
		0x88, // #   #
		0x88, // #   #
		0xF8, // #####
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x00, //

		// @369 'I' (5 pixels wide)
		0x00, //
		0xF8, // #####
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0xF8, // #####
		0x00, //

		// @378 'J' (5 pixels wide)
		0x00, //
		0x08, //     #
		0x08, //     #
		0x08, //     #
		0x08, //     #
		0x88, // #   #
		0xF8, // #####
		0x00, //

		// @387 'K' (5 pixels wide)
		0x00, //
		0x88, // #   #
		0x90, // #  #
		0xE0, // ###
		0x90, // #  #
		0x88, // #   #
		0x88, // #   #
		0x00, //

		// @396 'L' (5 pixels wide)
		0x00, //
		0x80, // #
		0x80, // #
		0x80, // #
		0x80, // #
		0x80, // #
		0xF8, // #####
		0x00, //

		// @405 'M' (5 pixels wide)
		0x00, //
		0x88, // #   #
		0xD8, // ## ##
		0xA8, // # # #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x00, //

		// @414 'N' (5 pixels wide)
		0x00, //
		0x88, // #   #
		0xC8, // ##  #
		0xA8, // # # #
		0x98, // #  ##
		0x88, // #   #
		0x88, // #   #
		0x00, //

		// @423 'O' (5 pixels wide)
		0x00, //
		0xF8, // #####
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0xF8, // #####
		0x00, //

		// @432 'P' (5 pixels wide)
		0x00, //
		0xF0, // ####
		0x88, // #   #
		0xF8, // #####
		0x80, // #
		0x80, // #
		0x80, // #
		0x00, //

		// @441 'Q' (5 pixels wide)
		0x00, //
		0xF8, // #####
		0x88, // #   #
		0x88, // #   #
		0xA8, // # # #
		0xF8, // #####
		0x20, //   #
		0x00, //

		// @450 'R' (5 pixels wide)
		0x00, //
		0xF0, // ####
		0x88, // #   #
		0xF8, // #####
		0x90, // #  #
		0x88, // #   #
		0x88, // #   #
		0x00, //

		// @459 'S' (5 pixels wide)
		0x00, //
		0xF8, // #####
		0x80, // #
		0xF8, // #####
		0x08, //     #
		0x08, //     #
		0xF8, // #####
		0x00, //

		// @468 'T' (5 pixels wide)
		0x00, //
		0xF8, // #####
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x00, //

		// @477 'U' (5 pixels wide)
		0x00, //
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0xF8, // #####
		0x00, //

		// @486 'V' (5 pixels wide)
		0x00, //
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x50, //  # #
		0x20, //   #
		0x00, //

		// @495 'W' (5 pixels wide)
		0x00, //
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0xA8, // # # #
		0xD8, // ## ##
		0x88, // #   #
		0x00, //

		// @504 'X' (5 pixels wide)
		0x00, //
		0x88, // #   #
		0x50, //  # #
		0x20, //   #
		0x20, //   #
		0x50, //  # #
		0x88, // #   #
		0x00, //

		// @513 'Y' (5 pixels wide)
		0x00, //
		0x88, // #   #
		0x88, // #   #
		0xF8, // #####
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x00, //

		// @522 'Z' (5 pixels wide)
		0x00, //
		0xF8, // #####
		0x10, //    #
		0x20, //   #
		0x40, //  #
		0x80, // #
		0xF8, // #####
		0x00, //

		// @531 '[' (5 pixels wide)
		0x00, //
		0x60, //  ##
		0x40, //  #
		0x40, //  #
		0x40, //  #
		0x40, //  #
		0x60, //  ##
		0x00, //

		// @540 '\' (5 pixels wide)
		0x00, //
		0x00, //
		0x80, // #
		0x40, //  #
		0x20, //   #
		0x10, //    #
		0x08, //     #
		0x00, //

		// @549 ']' (5 pixels wide)
		0x00, //
		0x60, //  ##
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x60, //  ##
		0x00, //

		// @558 '^' (5 pixels wide)
		0x20, //   #
		0x50, //  # #
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //

		// @567 '_' (5 pixels wide)
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0xF8, // #####
		0x00, //

		// @576 '`' (5 pixels wide)
		0x20, //   #
		0x20, //   #
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //

		// @585 'a' (5 pixels wide)
		0x00, //
		0x00, //
		0xF8, // #####
		0x88, // #   #
		0xF8, // #####
		0x88, // #   #
		0x88, // #   #
		0x00, //

		// @594 'b' (5 pixels wide)
		0x00, //
		0x00, //
		0xF8, // #####
		0x88, // #   #
		0xF0, // ####
		0x88, // #   #
		0xF8, // #####
		0x00, //

		// @603 'c' (5 pixels wide)
		0x00, //
		0x00, //
		0xF8, // #####
		0x80, // #
		0x80, // #
		0x80, // #
		0xF8, // #####
		0x00, //

		// @612 'd' (5 pixels wide)
		0x00, //
		0x00, //
		0xF0, // ####
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0xF0, // ####
		0x00, //

		// @621 'e' (5 pixels wide)
		0x00, //
		0x00, //
		0xF8, // #####
		0x80, // #
		0xE0, // ###
		0x80, // #
		0xF8, // #####
		0x00, //

		// @630 'f' (5 pixels wide)
		0x00, //
		0x00, //
		0xF8, // #####
		0x80, // #
		0xE0, // ###
		0x80, // #
		0x80, // #
		0x00, //

		// @639 'g' (5 pixels wide)
		0x00, //
		0x00, //
		0xF8, // #####
		0x80, // #
		0xB8, // # ###
		0x88, // #   #
		0xF8, // #####
		0x00, //

		// @648 'h' (5 pixels wide)
		0x00, //
		0x00, //
		0x88, // #   #
		0x88, // #   #
		0xF8, // #####
		0x88, // #   #
		0x88, // #   #
		0x00, //

		// @657 'i' (5 pixels wide)
		0x00, //
		0x00, //
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x00, //

		// @666 'j' (5 pixels wide)
		0x00, //
		0x00, //
		0x08, //     #
		0x08, //     #
		0x08, //     #
		0x88, // #   #
		0xF8, // #####
		0x00, //

		// @675 'k' (5 pixels wide)
		0x00, //
		0x00, //
		0x88, // #   #
		0x90, // #  #
		0xE0, // ###
		0x90, // #  #
		0x88, // #   #
		0x00, //

		// @684 'l' (5 pixels wide)
		0x00, //
		0x00, //
		0x80, // #
		0x80, // #
		0x80, // #
		0x80, // #
		0xF8, // #####
		0x00, //

		// @693 'm' (5 pixels wide)
		0x00, //
		0x00, //
		0xF8, // #####
		0xA8, // # # #
		0xA8, // # # #
		0xA8, // # # #
		0xA8, // # # #
		0x00, //

		// @702 'n' (5 pixels wide)
		0x00, //
		0x00, //
		0xF8, // #####
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x00, //

		// @711 'o' (5 pixels wide)
		0x00, //
		0x00, //
		0xF8, // #####
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0xF8, // #####
		0x00, //

		// @720 'p' (5 pixels wide)
		0x00, //
		0x00, //
		0xF8, // #####
		0x88, // #   #
		0xF8, // #####
		0x80, // #
		0x80, // #
		0x00, //

		// @729 'q' (5 pixels wide)
		0x00, //
		0x00, //
		0xF8, // #####
		0x88, // #   #
		0xA8, // # # #
		0xF8, // #####
		0x20, //   #
		0x00, //

		// @738 'r' (5 pixels wide)
		0x00, //
		0x00, //
		0xF8, // #####
		0x88, // #   #
		0xF8, // #####
		0x90, // #  #
		0x88, // #   #
		0x00, //

		// @747 's' (5 pixels wide)
		0x00, //
		0x00, //
		0xF8, // #####
		0x80, // #
		0xF8, // #####
		0x08, //     #
		0xF8, // #####
		0x00, //

		// @756 't' (5 pixels wide)
		0x00, //
		0x00, //
		0xF8, // #####
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x00, //

		// @765 'u' (5 pixels wide)
		0x00, //
		0x00, //
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0xF8, // #####
		0x00, //

		// @774 'v' (5 pixels wide)
		0x00, //
		0x00, //
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x50, //  # #
		0x20, //   #
		0x00, //

		// @783 'w' (5 pixels wide)
		0x00, //
		0x00, //
		0xA8, // # # #
		0xA8, // # # #
		0xA8, // # # #
		0xA8, // # # #
		0xF8, // #####
		0x00, //

		// @792 'x' (5 pixels wide)
		0x00, //
		0x00, //
		0x88, // #   #
		0x50, //  # #
		0x20, //   #
		0x50, //  # #
		0x88, // #   #
		0x00, //

		// @801 'y' (5 pixels wide)
		0x00, //
		0x00, //
		0x88, // #   #
		0x50, //  # #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x00, //

		// @810 'z' (5 pixels wide)
		0x00, //
		0x00, //
		0xF8, // #####
		0x10, //    #
		0x20, //   #
		0x40, //  #
		0xF8, // #####
		0x00, //

		// @819 '�' (5 pixels wide)
		0x38, //   ###
		0x28, //   # #
		0x38, //   ###
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //

		// @828 '|' (5 pixels wide)
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x00, //

		// @837 'Pfeil hoch' (5 pixels wide)
		0x20, //   #
		0x70, //  ###
		0xF8, // #####
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x00, //

		// @846 'Pfeil links' (5 pixels wide)
		0x20, //   #
		0x60, //  ##
		0xF8, // #####
		0x68, //  ## #
		0x28, //   # #
		0x08, //     #
		0x08, //     #
		0x00, //

		// @855 'Pfeil rechts' (5 pixels wide)
		0x20, //   #
		0x30, //   ##
		0xF8, // #####
		0xB0, // # ##
		0xA0, // # #
		0x80, // #
		0x80, // #
		0x00  //
	};

	static char sans_big[] = {
		// @96 '0' (23 pixels wide)
		0x00, 0x7E, 0x00, //          ######
		0x03, 0xFF, 0xC0, //       ############
		0x07, 0xFF, 0xE0, //      ##############
		0x0F, 0xC3, 0xF0, //     ######    ######
		0x0F, 0x00, 0xF0, //     ####        ####
		0x1E, 0x00, 0x78, //    ####          ####
		0x1E, 0x00, 0x78, //    ####          ####
		0x3C, 0x00, 0x3C, //   ####            ####
		0x3C, 0x00, 0x3C, //   ####            ####
		0x3C, 0x00, 0x3C, //   ####            ####
		0x78, 0x00, 0x1E, //  ####              ####
		0x78, 0x00, 0x1E, //  ####              ####
		0x78, 0x00, 0x1E, //  ####              ####
		0x78, 0x00, 0x1E, //  ####              ####
		0x78, 0x00, 0x1E, //  ####              ####
		0x78, 0x00, 0x1E, //  ####              ####
		0x78, 0x00, 0x1E, //  ####              ####
		0x78, 0x00, 0x1E, //  ####              ####
		0x78, 0x00, 0x1E, //  ####              ####
		0x78, 0x00, 0x1E, //  ####              ####
		0x78, 0x00, 0x1E, //  ####              ####
		0x78, 0x00, 0x1E, //  ####              ####
		0x3C, 0x00, 0x3C, //   ####            ####
		0x3C, 0x00, 0x3C, //   ####            ####
		0x3C, 0x00, 0x3C, //   ####            ####
		0x1E, 0x00, 0x78, //    ####          ####
		0x1E, 0x00, 0x78, //    ####          ####
		0x0F, 0x00, 0xF0, //     ####        ####
		0x0F, 0xC3, 0xF0, //     ######    ######
		0x07, 0xFF, 0xE0, //      ##############
		0x03, 0xFF, 0xC0, //       ############
		0x00, 0x7E, 0x00, //          ######

		// @192 '1' (23 pixels wide)
		0x00, 0x00, 0x00, //
		0x00, 0xFC, 0x00, //         ######
		0x0F, 0xFC, 0x00, //     ##########
		0x3F, 0xFC, 0x00, //   ############
		0x3F, 0x3C, 0x00, //   ######  ####
		0x30, 0x3C, 0x00, //   ##      ####
		0x00, 0x3C, 0x00, //           ####
		0x00, 0x3C, 0x00, //           ####
		0x00, 0x3C, 0x00, //           ####
		0x00, 0x3C, 0x00, //           ####
		0x00, 0x3C, 0x00, //           ####
		0x00, 0x3C, 0x00, //           ####
		0x00, 0x3C, 0x00, //           ####
		0x00, 0x3C, 0x00, //           ####
		0x00, 0x3C, 0x00, //           ####
		0x00, 0x3C, 0x00, //           ####
		0x00, 0x3C, 0x00, //           ####
		0x00, 0x3C, 0x00, //           ####
		0x00, 0x3C, 0x00, //           ####
		0x00, 0x3C, 0x00, //           ####
		0x00, 0x3C, 0x00, //           ####
		0x00, 0x3C, 0x00, //           ####
		0x00, 0x3C, 0x00, //           ####
		0x00, 0x3C, 0x00, //           ####
		0x00, 0x3C, 0x00, //           ####
		0x00, 0x3C, 0x00, //           ####
		0x00, 0x3C, 0x00, //           ####
		0x00, 0x3C, 0x00, //           ####
		0x00, 0x3C, 0x00, //           ####
		0x1F, 0xFF, 0xF8, //    ##################
		0x1F, 0xFF, 0xF8, //    ##################
		0x1F, 0xFF, 0xF8, //    ##################

		// @288 '2' (23 pixels wide)
		0x03, 0xFC, 0x00, //       ########
		0x3F, 0xFF, 0x80, //   ###############
		0x7F, 0xFF, 0xC0, //  #################
		0x7E, 0x07, 0xE0, //  ######      ######
		0x70, 0x01, 0xF0, //  ###           #####
		0x40, 0x00, 0xF0, //  #              ####
		0x00, 0x00, 0x78, //                  ####
		0x00, 0x00, 0x78, //                  ####
		0x00, 0x00, 0x78, //                  ####
		0x00, 0x00, 0x78, //                  ####
		0x00, 0x00, 0x78, //                  ####
		0x00, 0x00, 0xF8, //                 #####
		0x00, 0x00, 0xF8, //                 #####
		0x00, 0x01, 0xF0, //                #####
		0x00, 0x01, 0xF0, //                #####
		0x00, 0x03, 0xE0, //               #####
		0x00, 0x07, 0xC0, //              #####
		0x00, 0x0F, 0xC0, //             ######
		0x00, 0x1F, 0x80, //            ######
		0x00, 0x3F, 0x00, //           ######
		0x00, 0x7E, 0x00, //          ######
		0x00, 0xFC, 0x00, //         ######
		0x01, 0xF8, 0x00, //        ######
		0x03, 0xF0, 0x00, //       ######
		0x07, 0xE0, 0x00, //      ######
		0x0F, 0xC0, 0x00, //     ######
		0x1F, 0x80, 0x00, //    ######
		0x3F, 0x00, 0x00, //   ######
		0x7E, 0x00, 0x00, //  ######
		0x7F, 0xFF, 0xF8, //  ####################
		0x7F, 0xFF, 0xF8, //  ####################
		0x7F, 0xFF, 0xF8, //  ####################

		// @384 '3' (23 pixels wide)
		0x07, 0xFE, 0x00, //      ##########
		0x3F, 0xFF, 0x80, //   ###############
		0x3F, 0xFF, 0xE0, //   #################
		0x3C, 0x03, 0xF0, //   ####        ######
		0x20, 0x01, 0xF0, //   #            #####
		0x00, 0x00, 0xF8, //                 #####
		0x00, 0x00, 0x78, //                  ####
		0x00, 0x00, 0x78, //                  ####
		0x00, 0x00, 0x78, //                  ####
		0x00, 0x00, 0x78, //                  ####
		0x00, 0x00, 0xF0, //                 ####
		0x00, 0x00, 0xF0, //                 ####
		0x00, 0x03, 0xE0, //               #####
		0x03, 0xFF, 0xC0, //       ############
		0x03, 0xFF, 0x00, //       ##########
		0x03, 0xFF, 0xC0, //       ############
		0x00, 0x03, 0xF0, //               ######
		0x00, 0x00, 0xF8, //                 #####
		0x00, 0x00, 0x78, //                  ####
		0x00, 0x00, 0x7C, //                  #####
		0x00, 0x00, 0x3C, //                   ####
		0x00, 0x00, 0x3C, //                   ####
		0x00, 0x00, 0x3C, //                   ####
		0x00, 0x00, 0x3C, //                   ####
		0x00, 0x00, 0x3C, //                   ####
		0x00, 0x00, 0x7C, //                  #####
		0x40, 0x00, 0x78, //  #               ####
		0x60, 0x00, 0xF8, //  ##             #####
		0x7C, 0x07, 0xF0, //  #####       #######
		0x7F, 0xFF, 0xE0, //  ##################
		0x3F, 0xFF, 0x80, //   ###############
		0x07, 0xFC, 0x00, //      #########

		// @480 '4' (23 pixels wide)
		0x00, 0x00, 0x00, //
		0x00, 0x0F, 0xC0, //             ######
		0x00, 0x0F, 0xC0, //             ######
		0x00, 0x1F, 0xC0, //            #######
		0x00, 0x3F, 0xC0, //           ########
		0x00, 0x3B, 0xC0, //           ### ####
		0x00, 0x7B, 0xC0, //          #### ####
		0x00, 0xF3, 0xC0, //         ####  ####
		0x00, 0xE3, 0xC0, //         ###   ####
		0x01, 0xE3, 0xC0, //        ####   ####
		0x03, 0xC3, 0xC0, //       ####    ####
		0x03, 0x83, 0xC0, //       ###     ####
		0x07, 0x83, 0xC0, //      ####     ####
		0x0F, 0x03, 0xC0, //     ####      ####
		0x0E, 0x03, 0xC0, //     ###       ####
		0x1E, 0x03, 0xC0, //    ####       ####
		0x3C, 0x03, 0xC0, //   ####        ####
		0x38, 0x03, 0xC0, //   ###         ####
		0x78, 0x03, 0xC0, //  ####         ####
		0xF0, 0x03, 0xC0, // ####          ####
		0xF0, 0x03, 0xC0, // ####          ####
		0xFF, 0xFF, 0xFC, // ######################
		0xFF, 0xFF, 0xFC, // ######################
		0xFF, 0xFF, 0xFC, // ######################
		0x00, 0x03, 0xC0, //               ####
		0x00, 0x03, 0xC0, //               ####
		0x00, 0x03, 0xC0, //               ####
		0x00, 0x03, 0xC0, //               ####
		0x00, 0x03, 0xC0, //               ####
		0x00, 0x03, 0xC0, //               ####
		0x00, 0x03, 0xC0, //               ####
		0x00, 0x03, 0xC0, //               ####

		// @576 '5' (23 pixels wide)
		0x00, 0x00, 0x00, //
		0x3F, 0xFF, 0xE0, //   #################
		0x3F, 0xFF, 0xE0, //   #################
		0x3F, 0xFF, 0xE0, //   #################
		0x3C, 0x00, 0x00, //   ####
		0x3C, 0x00, 0x00, //   ####
		0x3C, 0x00, 0x00, //   ####
		0x3C, 0x00, 0x00, //   ####
		0x3C, 0x00, 0x00, //   ####
		0x3C, 0x00, 0x00, //   ####
		0x3C, 0x00, 0x00, //   ####
		0x3F, 0xFC, 0x00, //   ############
		0x3F, 0xFF, 0x00, //   ##############
		0x3F, 0xFF, 0xC0, //   ################
		0x38, 0x0F, 0xE0, //   ###       #######
		0x20, 0x03, 0xE0, //   #           #####
		0x00, 0x01, 0xF0, //                #####
		0x00, 0x00, 0xF0, //                 ####
		0x00, 0x00, 0x78, //                  ####
		0x00, 0x00, 0x78, //                  ####
		0x00, 0x00, 0x78, //                  ####
		0x00, 0x00, 0x78, //                  ####
		0x00, 0x00, 0x78, //                  ####
		0x00, 0x00, 0x78, //                  ####
		0x00, 0x00, 0x78, //                  ####
		0x00, 0x00, 0xF0, //                 ####
		0x00, 0x01, 0xF0, //                #####
		0x40, 0x03, 0xE0, //  #            #####
		0x78, 0x0F, 0xE0, //  ####       #######
		0x7F, 0xFF, 0xC0, //  #################
		0x7F, 0xFF, 0x00, //  ###############
		0x1F, 0xF8, 0x00, //    ##########

		// @672 '6' (23 pixels wide)
		0x00, 0x3F, 0xE0, //           #########
		0x00, 0xFF, 0xF8, //         #############
		0x03, 0xFF, 0xF8, //       ###############
		0x07, 0xE0, 0x38, //      ######       ###
		0x0F, 0x80, 0x08, //     #####           #
		0x0F, 0x00, 0x00, //     ####
		0x1E, 0x00, 0x00, //    ####
		0x1E, 0x00, 0x00, //    ####
		0x3C, 0x00, 0x00, //   ####
		0x3C, 0x00, 0x00, //   ####
		0x3C, 0x00, 0x00, //   ####
		0x78, 0x7F, 0x00, //  ####    #######
		0x79, 0xFF, 0xC0, //  ####  ###########
		0x7B, 0xFF, 0xF0, //  #### ##############
		0x7F, 0xC1, 0xF8, //  #########     ######
		0x7F, 0x00, 0x7C, //  #######         #####
		0x7E, 0x00, 0x3C, //  ######           ####
		0x7E, 0x00, 0x3C, //  ######           ####
		0x7C, 0x00, 0x1E, //  #####             ####
		0x7C, 0x00, 0x1E, //  #####             ####
		0x7C, 0x00, 0x1E, //  #####             ####
		0x7C, 0x00, 0x1E, //  #####             ####
		0x3C, 0x00, 0x1E, //   ####             ####
		0x3C, 0x00, 0x1E, //   ####             ####
		0x3C, 0x00, 0x1E, //   ####             ####
		0x1E, 0x00, 0x3C, //    ####           ####
		0x1E, 0x00, 0x3C, //    ####           ####
		0x0F, 0x00, 0x78, //     ####         ####
		0x0F, 0xC1, 0xF8, //     ######     ######
		0x07, 0xFF, 0xF0, //      ###############
		0x01, 0xFF, 0xC0, //        ###########
		0x00, 0x7F, 0x00, //          #######

		// @768 '7' (23 pixels wide)
		0x00, 0x00, 0x00, //
		0x7F, 0xFF, 0xF8, //  ####################
		0x7F, 0xFF, 0xF8, //  ####################
		0x7F, 0xFF, 0xF8, //  ####################
		0x00, 0x00, 0xF0, //                 ####
		0x00, 0x00, 0xF0, //                 ####
		0x00, 0x01, 0xF0, //                #####
		0x00, 0x01, 0xE0, //                ####
		0x00, 0x03, 0xE0, //               #####
		0x00, 0x03, 0xC0, //               ####
		0x00, 0x03, 0xC0, //               ####
		0x00, 0x07, 0xC0, //              #####
		0x00, 0x07, 0x80, //              ####
		0x00, 0x0F, 0x80, //             #####
		0x00, 0x0F, 0x00, //             ####
		0x00, 0x0F, 0x00, //             ####
		0x00, 0x1E, 0x00, //            ####
		0x00, 0x1E, 0x00, //            ####
		0x00, 0x3E, 0x00, //           #####
		0x00, 0x3C, 0x00, //           ####
		0x00, 0x3C, 0x00, //           ####
		0x00, 0x78, 0x00, //          ####
		0x00, 0x78, 0x00, //          ####
		0x00, 0xF8, 0x00, //         #####
		0x00, 0xF0, 0x00, //         ####
		0x00, 0xF0, 0x00, //         ####
		0x01, 0xE0, 0x00, //        ####
		0x01, 0xE0, 0x00, //        ####
		0x03, 0xE0, 0x00, //       #####
		0x03, 0xC0, 0x00, //       ####
		0x03, 0xC0, 0x00, //       ####
		0x07, 0x80, 0x00, //      ####

		// @864 '8' (23 pixels wide)
		0x00, 0xFF, 0x00, //         ########
		0x03, 0xFF, 0xE0, //       #############
		0x0F, 0xFF, 0xF0, //     ################
		0x1F, 0x81, 0xF8, //    ######      ######
		0x1E, 0x00, 0x78, //    ####          ####
		0x3E, 0x00, 0x7C, //   #####          #####
		0x3C, 0x00, 0x3C, //   ####            ####
		0x3C, 0x00, 0x3C, //   ####            ####
		0x3C, 0x00, 0x3C, //   ####            ####
		0x3C, 0x00, 0x3C, //   ####            ####
		0x1E, 0x00, 0x78, //    ####          ####
		0x1E, 0x00, 0x78, //    ####          ####
		0x0F, 0x81, 0xF0, //     #####      #####
		0x07, 0xFF, 0xE0, //      ##############
		0x01, 0xFF, 0x80, //        ##########
		0x07, 0xFF, 0xE0, //      ##############
		0x1F, 0x81, 0xF8, //    ######      ######
		0x1E, 0x00, 0x78, //    ####          ####
		0x3C, 0x00, 0x3C, //   ####            ####
		0x7C, 0x00, 0x3E, //  #####            #####
		0x78, 0x00, 0x1E, //  ####              ####
		0x78, 0x00, 0x1E, //  ####              ####
		0x78, 0x00, 0x1E, //  ####              ####
		0x78, 0x00, 0x1E, //  ####              ####
		0x78, 0x00, 0x1E, //  ####              ####
		0x7C, 0x00, 0x3E, //  #####            #####
		0x3C, 0x00, 0x3C, //   ####            ####
		0x3E, 0x00, 0x7C, //   #####          #####
		0x1F, 0x81, 0xF8, //    ######      ######
		0x0F, 0xFF, 0xF0, //     ################
		0x07, 0xFF, 0xE0, //      ##############
		0x00, 0xFF, 0x00, //         ########

		// @960 '9' (23 pixels wide)
		0x00, 0xFE, 0x00, //         #######
		0x03, 0xFF, 0x80, //       ###########
		0x0F, 0xFF, 0xE0, //     ###############
		0x1F, 0x83, 0xF0, //    ######     ######
		0x1E, 0x00, 0xF0, //    ####         ####
		0x3C, 0x00, 0x78, //   ####           ####
		0x3C, 0x00, 0x78, //   ####           ####
		0x78, 0x00, 0x3C, //  ####             ####
		0x78, 0x00, 0x3C, //  ####             ####
		0x78, 0x00, 0x3C, //  ####             ####
		0x78, 0x00, 0x3E, //  ####             #####
		0x78, 0x00, 0x3E, //  ####             #####
		0x78, 0x00, 0x3E, //  ####             #####
		0x78, 0x00, 0x3E, //  ####             #####
		0x3C, 0x00, 0x7E, //   ####           ######
		0x3C, 0x00, 0x7E, //   ####           ######
		0x3E, 0x00, 0xFE, //   #####         #######
		0x1F, 0x83, 0xFE, //    ######     #########
		0x0F, 0xFF, 0xDE, //     ############## ####
		0x03, 0xFF, 0x9E, //       ###########  ####
		0x00, 0xFE, 0x1E, //         #######    ####
		0x00, 0x00, 0x3C, //                   ####
		0x00, 0x00, 0x3C, //                   ####
		0x00, 0x00, 0x3C, //                   ####
		0x00, 0x00, 0x78, //                  ####
		0x00, 0x00, 0x78, //                  ####
		0x00, 0x00, 0xF0, //                 ####
		0x10, 0x01, 0xF0, //    #           #####
		0x1C, 0x07, 0xE0, //    ###       ######
		0x1F, 0xFF, 0xC0, //    ###############
		0x1F, 0xFF, 0x00, //    #############
		0x07, 0xFC, 0x00 //      #########
	};

	static char dejaVuSans5ptBitmaps[] = {
		// @0 ' ' (5 pixels wide)
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //

		// @9 '!' (5 pixels wide)
		0x10, //    #
		0x10, //    #
		0x10, //    #
		0x10, //    #
		0x10, //    #
		0x00, //
		0x10, //    #
		0x00, //

		// @18 '"' (5 pixels wide)
		0x50, //  # #
		0x50, //  # #
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //

		// @27 '#' (5 pixels wide)
		0x00, //
		0x50, //  # #
		0xF8, // #####
		0x50, //  # #
		0xF8, // #####
		0x50, //  # #
		0x00, //
		0x00, //

		// @36 '$' (5 pixels wide)
		0x20, //   #
		0x78, //  ####
		0xA0, // # #
		0x70, //  ###
		0x28, //   # #
		0xF8, // #####
		0x20, //   #
		0x00, //

		// @45 '%' (5 pixels wide)
		0x00, //
		0xC8, // ##  #
		0xD0, // ## #
		0x20, //   #
		0x20, //   #
		0x58, //  # ##
		0x98, // #  ##
		0x00, //

		// @54 '&' (5 pixels wide)
		0x60, //  ##
		0x90, // #  #
		0xA0, // # #
		0x78, //  ####
		0x90, // #  #
		0x98, // #  ##
		0xE8, // ### #
		0x00, //

		// @63 ''' (5 pixels wide)
		0x20, //   #
		0x20, //   #
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //

		// @72 '(' (5 pixels wide)
		0x10, //    #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x10, //    #
		0x00, //

		// @81 ')' (5 pixels wide)
		0x20, //   #
		0x10, //    #
		0x10, //    #
		0x10, //    #
		0x10, //    #
		0x10, //    #
		0x20, //   #
		0x00, //

		// @90 '*' (5 pixels wide)
		0x20, //   #
		0xA8, // # # #
		0x70, //  ###
		0x70, //  ###
		0xA8, // # # #
		0x20, //   #
		0x00, //
		0x00, //

		// @99 '+' (5 pixels wide)
		0x00, //
		0x20, //   #
		0x20, //   #
		0xF8, // #####
		0x20, //   #
		0x20, //   #
		0x00, //
		0x00, //

		// @108 ',' (5 pixels wide)
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x20, //   #
		0x20, //   #
		0x00, //

		// @117 '-' (5 pixels wide)
		0x00, //
		0x00, //
		0x00, //
		0xF8, // #####
		0x00, //
		0x00, //
		0x00, //
		0x00, //

		// @126 '.' (5 pixels wide)
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x20, //   #
		0x00, //

		// @135 '/' (5 pixels wide)
		0x00, //
		0x00, //
		0x08, //     #
		0x10, //    #
		0x20, //   #
		0x40, //  #
		0x80, // #
		0x00, //

		// @144 '0' (5 pixels wide)
		0x70, //  ###
		0xD8, // ## ##
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0xD8, // ## ##
		0x70, //  ###
		0x00, //

		// @153 '1' (5 pixels wide)
		0xE0, // ###
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0xF8, // #####
		0x00, //

		// @162 '2' (5 pixels wide)
		0x70, //  ###
		0x88, // #   #
		0x08, //     #
		0x10, //    #
		0x20, //   #
		0x40, //  #
		0xF8, // #####
		0x00, //

		// @171 '3' (5 pixels wide)
		0x70, //  ###
		0x88, // #   #
		0x08, //     #
		0x70, //  ###
		0x08, //     #
		0x88, // #   #
		0x70, //  ###
		0x00, //

		// @180 '4' (5 pixels wide)
		0x10, //    #
		0x30, //   ##
		0x50, //  # #
		0x50, //  # #
		0x90, // #  #
		0xF8, // #####
		0x10, //    #
		0x00, //

		// @189 '5' (5 pixels wide)
		0xF0, // ####
		0x80, // #
		0x80, // #
		0xF0, // ####
		0x08, //     #
		0x08, //     #
		0xF0, // ####
		0x00, //


		// @198 '6' (5 pixels wide)
		0x38, //   ###
		0x40, //  #
		0x80, // #
		0xF0, // ####
		0x88, // #   #
		0x88, // #   #
		0x70, //  ###
		0x00, //

		// @207 '7' (5 pixels wide)
		0xF8, // #####
		0x08, //     #
		0x10, //    #
		0x10, //    #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x00, //

		// @216 '8' (5 pixels wide)
		0x70, //  ###
		0x88, // #   #
		0x88, // #   #
		0x70, //  ###
		0x88, // #   #
		0x88, // #   #
		0x70, //  ###
		0x00, //

		// @225 '9' (5 pixels wide)
		0x70, //  ###
		0x88, // #   #
		0x88, // #   #
		0x78, //  ####
		0x08, //     #
		0x10, //    #
		0xE0, // ###
		0x00, //

		// @234 ':' (5 pixels wide)
		0x00, //
		0x00, //
		0x20, //   #
		0x00, //
		0x00, //
		0x20, //   #
		0x00, //
		0x00, //

		// @243 ';' (5 pixels wide)
		0x00, //
		0x00, //
		0x20, //   #
		0x00, //
		0x00, //
		0x20, //   #
		0x20, //   #
		0x00, //

		// @252 '<' (5 pixels wide)
		0x00, //
		0x18, //    ##
		0x60, //  ##
		0xC0, // ##
		0x60, //  ##
		0x18, //    ##
		0x00, //
		0x00, //

		// @261 '=' (5 pixels wide)
		0x00, //
		0x00, //
		0xF8, // #####
		0x00, //
		0xF8, // #####
		0x00, //
		0x00, //
		0x00, //

		// @270 '>' (5 pixels wide)
		0x00, //
		0xC0, // ##
		0x30, //   ##
		0x18, //    ##
		0x30, //   ##
		0xC0, // ##
		0x00, //
		0x00, //

		// @279 '?' (5 pixels wide)
		0x70, //  ###
		0x88, // #   #
		0x08, //     #
		0x10, //    #
		0x20, //   #
		0x00, //
		0x20, //   #
		0x00, //

		// @288 '@' (5 pixels wide)
		0x70, //  ###
		0x88, // #   #
		0xB8, // # ###
		0xA8, // # # #
		0xB8, // # ###
		0x80, // #
		0x60, //  ##
		0x00, //

		// @297 'A' (5 pixels wide)
		0x20, //   #
		0x70, //  ###
		0xD8, // ## ##
		0x88, // #   #
		0xF8, // #####
		0x88, // #   #
		0x88, // #   #
		0x00, //

		// @306 'B' (5 pixels wide)
		0xF0, // ####
		0x88, // #   #
		0x88, // #   #
		0xF8, // #####
		0x88, // #   #
		0x88, // #   #
		0xF0, // ####
		0x00, //

		// @315 'C' (5 pixels wide)
		0x70, //  ###
		0x88, // #   #
		0x80, // #
		0x80, // #
		0x80, // #
		0x88, // #   #
		0x70, //  ###
		0x00, //

		// @324 'D' (5 pixels wide)
		0xE0, // ###
		0x90, // #  #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x90, // #  #
		0xE0, // ###
		0x00, //

		// @333 'E' (5 pixels wide)
		0xF8, // #####
		0x80, // #
		0x80, // #
		0xF8, // #####
		0x80, // #
		0x80, // #
		0xF8, // #####
		0x00, //

		// @342 'F' (5 pixels wide)
		0xF8, // #####
		0x80, // #
		0x80, // #
		0xF8, // #####
		0x80, // #
		0x80, // #
		0x80, // #
		0x00, //

		// @351 'G' (5 pixels wide)
		0x70, //  ###
		0x88, // #   #
		0x80, // #
		0x98, // #  ##
		0x88, // #   #
		0x88, // #   #
		0x70, //  ###
		0x00, //

		// @360 'H' (5 pixels wide)
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0xF8, // #####
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x00, //

		// @369 'I' (5 pixels wide)
		0x70, //  ###
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x70, //  ###
		0x00, //

		// @378 'J' (5 pixels wide)
		0x38, //   ###
		0x08, //     #
		0x08, //     #
		0x08, //     #
		0x08, //     #
		0x88, // #   #
		0x70, //  ###
		0x00, //

		// @387 'K' (5 pixels wide)
		0x88, // #   #
		0x90, // #  #
		0xA0, // # #
		0xC0, // ##
		0xA0, // # #
		0x90, // #  #
		0x88, // #   #
		0x00, //

		// @396 'L' (5 pixels wide)
		0x80, // #
		0x80, // #
		0x80, // #
		0x80, // #
		0x80, // #
		0x80, // #
		0xF8, // #####
		0x00, //

		// @405 'M' (5 pixels wide)
		0x88, // #   #
		0xD8, // ## ##
		0xF8, // #####
		0xA8, // # # #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x00, //

		// @414 'N' (5 pixels wide)
		0x88, // #   #
		0xC8, // ##  #
		0xC8, // ##  #
		0xA8, // # # #
		0x98, // #  ##
		0x98, // #  ##
		0x88, // #   #
		0x00, //

		// @423 'O' (5 pixels wide)
		0x70, //  ###
		0xD8, // ## ##
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0xD8, // ## ##
		0x70, //  ###
		0x00, //

		// @432 'P' (5 pixels wide)
		0xF0, // ####
		0x88, // #   #
		0x88, // #   #
		0xF0, // ####
		0x80, // #
		0x80, // #
		0x80, // #
		0x00, //

		// @441 'Q' (5 pixels wide)
		0x70, //  ###
		0xD8, // ## ##
		0x88, // #   #
		0x88, // #   #
		0xD8, // ## ##
		0x70, //  ###
		0x18, //    ##
		0x00, //

		// @450 'R' (5 pixels wide)
		0xF0, // ####
		0x88, // #   #
		0x88, // #   #
		0xF0, // ####
		0xA0, // # #
		0x90, // #  #
		0x88, // #   #
		0x00, //

		// @459 'S' (5 pixels wide)
		0x70, //  ###
		0x88, // #   #
		0x80, // #
		0x70, //  ###
		0x08, //     #
		0x88, // #   #
		0x70, //  ###
		0x00, //

		// @468 'T' (5 pixels wide)
		0xF8, // #####
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x00, //

		// @477 'U' (5 pixels wide)
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x70, //  ###
		0x00, //

		// @486 'V' (5 pixels wide)
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x50, //  # #
		0x50, //  # #
		0x20, //   #
		0x20, //   #
		0x00, //

		// @495 'W' (5 pixels wide)
		0x88, // #   #
		0xA8, // # # #
		0xA8, // # # #
		0xA8, // # # #
		0xA8, // # # #
		0xD8, // ## ##
		0x50, //  # #
		0x00, //

		// @504 'X' (5 pixels wide)
		0x88, // #   #
		0x50, //  # #
		0x20, //   #
		0x20, //   #
		0x50, //  # #
		0x88, // #   #
		0x88, // #   #
		0x00, //

		// @513 'Y' (5 pixels wide)
		0x88, // #   #
		0x50, //  # #
		0x50, //  # #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x00, //

		// @522 'Z' (5 pixels wide)
		0xF8, // #####
		0x08, //     #
		0x10, //    #
		0x20, //   #
		0x40, //  #
		0x80, // #
		0xF8, // #####
		0x00, //

		// @531 '[' (5 pixels wide)
		0x60, //  ##
		0x40, //  #
		0x40, //  #
		0x40, //  #
		0x40, //  #
		0x40, //  #
		0x60, //  ##
		0x00, //

		// @540 '\' (5 pixels wide)
		0x00, //
		0x00, //
		0x80, // #
		0x40, //  #
		0x20, //   #
		0x10, //    #
		0x08, //     #
		0x00, //

		// @549 ']' (5 pixels wide)
		0x60, //  ##
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x60, //  ##
		0x00, //

		// @558 '^' (5 pixels wide)
		0x30, //   ##
		0x48, //  #  #
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //

		// @567 '_' (5 pixels wide)
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0xF8, // #####
		0x00, //

		// @576 '`' (5 pixels wide)
		0x20, //   #
		0x20, //   #
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //

		// @585 'a' (5 pixels wide)
		0x00, //
		0x00, //
		0x70, //  ###
		0x08, //     #
		0xF8, // #####
		0x98, // #  ##
		0xE8, // ### #
		0x00, //

		// @594 'b' (5 pixels wide)
		0x80, // #
		0x80, // #
		0xB0, // # ##
		0xC8, // ##  #
		0x88, // #   #
		0xC8, // ##  #
		0xB0, // # ##
		0x00, //

		// @603 'c' (5 pixels wide)
		0x00, //
		0x00, //
		0x70, //  ###
		0x88, // #   #
		0x80, // #
		0x88, // #   #
		0x70, //  ###
		0x00, //

		// @612 'd' (5 pixels wide)
		0x08, //     #
		0x08, //     #
		0x68, //  ## #
		0x98, // #  ##
		0x88, // #   #
		0x98, // #  ##
		0x68, //  ## #
		0x00, //

		// @621 'e' (5 pixels wide)
		0x00, //
		0x00, //
		0x70, //  ###
		0x88, // #   #
		0xF8, // #####
		0x80, // #
		0x78, //  ####
		0x00, //

		// @630 'f' (5 pixels wide)
		0x18, //    ##
		0x20, //   #
		0x20, //   #
		0xF8, // #####
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x00, //

		// @639 'g' (5 pixels wide)
		0x00, //
		0x78, //  ####
		0x88, // #   #
		0x88, // #   #
		0x78, //  ####
		0x08, //     #
		0x70, //  ###
		0x00, //

		// @648 'h' (5 pixels wide)
		0x80, // #
		0x80, // #
		0xB0, // # ##
		0xC8, // ##  #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x00, //

		// @657 'i' (5 pixels wide)
		0x20, //   #
		0x00, //
		0x60, //  ##
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x70, //  ###
		0x00, //

		// @666 'j' (5 pixels wide)
		0x10, //    #
		0x00, //
		0x30, //   ##
		0x10, //    #
		0x10, //    #
		0x50, //  # #
		0x70, //  ###
		0x00, //

		// @675 'k' (5 pixels wide)
		0x80, // #
		0x80, // #
		0x90, // #  #
		0xA0, // # #
		0xC0, // ##
		0xA0, // # #
		0x90, // #  #
		0x00, //

		// @684 'l' (5 pixels wide)
		0xC0, // ##
		0x40, //  #
		0x40, //  #
		0x40, //  #
		0x40, //  #
		0x40, //  #
		0x30, //   ##
		0x00, //

		// @693 'm' (5 pixels wide)
		0x00, //
		0x00, //
		0xF8, // #####
		0xA8, // # # #
		0xA8, // # # #
		0xA8, // # # #
		0xA8, // # # #
		0x00, //

		// @702 'n' (5 pixels wide)
		0x00, //
		0x00, //
		0xB0, // # ##
		0xC8, // ##  #
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x00, //

		// @711 'o' (5 pixels wide)
		0x00, //
		0x00, //
		0x70, //  ###
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x70, //  ###
		0x00, //

		// @720 'p' (5 pixels wide)
		0x00, //
		0x00, //
		0xF0, // ####
		0x88, // #   #
		0xF0, // ####
		0x80, // #
		0x80, // #
		0x00, //

		// @729 'q' (5 pixels wide)
		0x00, //
		0x00, //
		0x78, //  ####
		0x88, // #   #
		0x78, //  ####
		0x08, //     #
		0x08, //     #
		0x00, //

		// @738 'r' (5 pixels wide)
		0x00, //
		0x00, //
		0xB0, // # ##
		0xC8, // ##  #
		0x80, // #
		0x80, // #
		0x80, // #
		0x00, //

		// @747 's' (5 pixels wide)
		0x00, //
		0x00, //
		0x78, //  ####
		0x80, // #
		0x70, //  ###
		0x08, //     #
		0xF0, // ####
		0x00, //

		// @756 't' (5 pixels wide)
		0x40, //  #
		0x40, //  #
		0xF0, // ####
		0x40, //  #
		0x40, //  #
		0x40, //  #
		0x30, //   ##
		0x00, //

		// @765 'u' (5 pixels wide)
		0x00, //
		0x00, //
		0x88, // #   #
		0x88, // #   #
		0x88, // #   #
		0x98, // #  ##
		0x68, //  ## #
		0x00, //

		// @774 'v' (5 pixels wide)
		0x00, //
		0x00, //
		0x88, // #   #
		0x88, // #   #
		0x50, //  # #
		0x50, //  # #
		0x20, //   #
		0x00, //

		// @783 'w' (5 pixels wide)
		0x00, //
		0x00, //
		0x88, // #   #
		0xA8, // # # #
		0xA8, // # # #
		0xF8, // #####
		0x50, //  # #
		0x00, //

		// @792 'x' (5 pixels wide)
		0x00, //
		0x00, //
		0x88, // #   #
		0x50, //  # #
		0x20, //   #
		0x50, //  # #
		0x88, // #   #
		0x00, //

		// @801 'y' (5 pixels wide)
		0x00, //
		0x00, //
		0x88, // #   #
		0x50, //  # #
		0x20, //   #
		0x20, //   #
		0x40, //  #
		0x00, //

		// @810 'z' (5 pixels wide)
		0x00, //
		0x00, //
		0xF8, // #####
		0x10, //    #
		0x20, //   #
		0x40, //  #
		0xF8, // #####
		0x00, //

		// @819 '�' (5 pixels wide)
		0x38, //   ###
		0x28, //   # #
		0x38, //   ###
		0x00, //
		0x00, //
		0x00, //
		0x00, //
		0x00, //

		// @828 '|' (5 pixels wide)
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x00, //

		// @837 'Pfeil hoch' (5 pixels wide)
		0x20, //   #
		0x70, //  ###
		0xF8, // #####
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x20, //   #
		0x00, //

		// @846 'Pfeil links' (5 pixels wide)
		0x20, //   #
		0x60, //  ##
		0xF8, // #####
		0x68, //  ## #
		0x28, //   # #
		0x08, //     #
		0x08, //     #
		0x00, //

		// @855 'Pfeil rechts' (5 pixels wide)
		0x20, //   #
		0x30, //   ##
		0xF8, // #####
		0xB0, // # ##
		0xA0, // # #
		0x80, // #
		0x80, // #
		0x00  //
	};

	@Override
	public void onItemSelected(AdapterView<?> arg0, View arg1, int arg2,
			long arg3) {
		// TODO Auto-generated method stub
		if(arg0.getId()==R.id.sensor_spinner){
			String[] tempArray = getResources().getStringArray(R.array.skin_sensor_array);
			if(arg2<tempArray.length){
				selected_sensor=tempArray[arg2];
			};

			if(selected_sensor.equals(tempArray[0])){
				if(kmh_x!=-1 && kmh_y!=-1){
					((CheckBox)findViewById(R.id.checkActive)).setChecked(true);
				} else {
					((CheckBox)findViewById(R.id.checkActive)).setChecked(false); 
				}
				if(kmh_symbol){
					((CheckBox)findViewById(R.id.checkSymbol)).setChecked(true);
				} else {
					((CheckBox)findViewById(R.id.checkSymbol)).setChecked(false); 
				}
				set_font_selector(kmh_font-1);
			} else if(selected_sensor.equals(tempArray[1])){
				if(kmhchar_x!=-1 && kmhchar_y!=-1){
					((CheckBox)findViewById(R.id.checkActive)).setChecked(true);
				} else {
					((CheckBox)findViewById(R.id.checkActive)).setChecked(false); 
				}
				if(kmhchar_symbol){
					((CheckBox)findViewById(R.id.checkSymbol)).setChecked(true);
				} else {
					((CheckBox)findViewById(R.id.checkSymbol)).setChecked(false); 
				}
				set_font_selector(kmhchar_font-1);
			} else if(selected_sensor.equals(tempArray[2])){
				if(oil_x!=-1 && oil_y!=-1){
					((CheckBox)findViewById(R.id.checkActive)).setChecked(true);
				} else {
					((CheckBox)findViewById(R.id.checkActive)).setChecked(false); 
				}
				if(oil_symbol){
					((CheckBox)findViewById(R.id.checkSymbol)).setChecked(true);
				} else {
					((CheckBox)findViewById(R.id.checkSymbol)).setChecked(false); 
				}
				set_font_selector(oil_font-1);
			} else if(selected_sensor.equals(tempArray[3])){
				if(water_x!=-1 && water_y!=-1){
					((CheckBox)findViewById(R.id.checkActive)).setChecked(true);
				} else {
					((CheckBox)findViewById(R.id.checkActive)).setChecked(false); 
				}
				if(water_symbol){
					((CheckBox)findViewById(R.id.checkSymbol)).setChecked(true);
				} else {
					((CheckBox)findViewById(R.id.checkSymbol)).setChecked(false); 
				}
				set_font_selector(water_font-1);
			} else if(selected_sensor.equals(tempArray[4])){
				if(air_x!=-1 && air_y!=-1){
					((CheckBox)findViewById(R.id.checkActive)).setChecked(true);
				} else {
					((CheckBox)findViewById(R.id.checkActive)).setChecked(false); 
				}
				if(air_symbol){
					((CheckBox)findViewById(R.id.checkSymbol)).setChecked(true);
				} else {
					((CheckBox)findViewById(R.id.checkSymbol)).setChecked(false); 
				}
				set_font_selector(air_font-1);
			} else if(selected_sensor.equals(tempArray[5])){
				if(dz_x!=-1 && dz_y!=-1){
					((CheckBox)findViewById(R.id.checkActive)).setChecked(true);
				} else {
					((CheckBox)findViewById(R.id.checkActive)).setChecked(false); 
				}
				if(dz_symbol){
					((CheckBox)findViewById(R.id.checkSymbol)).setChecked(true);
				} else {
					((CheckBox)findViewById(R.id.checkSymbol)).setChecked(false); 
				}
				set_font_selector(dz_font-1);
			} else if(selected_sensor.equals(tempArray[6])){
				// arrow
			} else if(selected_sensor.equals(tempArray[7])){
				if(addinfo_x!=-1 && addinfo_y!=-1){
					((CheckBox)findViewById(R.id.checkActive)).setChecked(true);
				} else {
					((CheckBox)findViewById(R.id.checkActive)).setChecked(false); 
				}
				if(addinfo_symbol){
					((CheckBox)findViewById(R.id.checkSymbol)).setChecked(true);
				} else {
					((CheckBox)findViewById(R.id.checkSymbol)).setChecked(false); 
				}
				set_font_selector(addinfo_font-1);
			} else if(selected_sensor.equals(tempArray[8])){
				if(addinfo2_x!=-1 && addinfo2_y!=-1){
					((CheckBox)findViewById(R.id.checkActive)).setChecked(true);
				} else {
					((CheckBox)findViewById(R.id.checkActive)).setChecked(false); 
				}
				if(addinfo2_symbol){
					((CheckBox)findViewById(R.id.checkSymbol)).setChecked(true);
				} else {
					((CheckBox)findViewById(R.id.checkSymbol)).setChecked(false); 
				}
				set_font_selector(addinfo2_font-1);
			} else if(selected_sensor.equals(tempArray[9])){
				if(gps_x!=-1 && gps_y!=-1){
					((CheckBox)findViewById(R.id.checkActive)).setChecked(true);
				} else {
					((CheckBox)findViewById(R.id.checkActive)).setChecked(false); 
				}
				if(gps_symbol){
					((CheckBox)findViewById(R.id.checkSymbol)).setChecked(true);
				} else {
					((CheckBox)findViewById(R.id.checkSymbol)).setChecked(false); 
				}
				set_font_selector(gps_font-1);
			} else if(selected_sensor.equals(tempArray[10])){
				if(fuel_x!=-1 && fuel_y!=-1){
					((CheckBox)findViewById(R.id.checkActive)).setChecked(true);
				} else {
					((CheckBox)findViewById(R.id.checkActive)).setChecked(false); 
				}
				if(fuel_symbol){
					((CheckBox)findViewById(R.id.checkSymbol)).setChecked(true);
				} else {
					((CheckBox)findViewById(R.id.checkSymbol)).setChecked(false); 
				}
				set_font_selector(fuel_font-1);
			} else if(selected_sensor.equals(tempArray[11])){
				if(gear_x!=-1 && gear_y!=-1){
					((CheckBox)findViewById(R.id.checkActive)).setChecked(true);
				} else {
					((CheckBox)findViewById(R.id.checkActive)).setChecked(false); 
				}
				if(gear_symbol){
					((CheckBox)findViewById(R.id.checkSymbol)).setChecked(true);
				} else {
					((CheckBox)findViewById(R.id.checkSymbol)).setChecked(false); 
				}
				set_font_selector(gear_font-1);
			} else if(selected_sensor.equals(tempArray[12])){
				if(clock_x!=-1 && clock_y!=-1){
					((CheckBox)findViewById(R.id.checkActive)).setChecked(true);
				} else {
					((CheckBox)findViewById(R.id.checkActive)).setChecked(false); 
				}
				if(clock_symbol){
					((CheckBox)findViewById(R.id.checkSymbol)).setChecked(true);
				} else {
					((CheckBox)findViewById(R.id.checkSymbol)).setChecked(false); 
				}
				set_font_selector(clock_font-1);
			}

		} else if(arg0.getId()==R.id.font_spinner){
			String[] tempArray = getResources().getStringArray(R.array.skin_sensor_array);
			if(selected_sensor.equals(tempArray[0])){
				kmh_font=arg2+1;
			} else if(selected_sensor.equals(tempArray[1])){
				kmhchar_font=arg2+1;
			} else if(selected_sensor.equals(tempArray[2])){
				oil_font=arg2+1;
			} else if(selected_sensor.equals(tempArray[3])){
				water_font=arg2+1;
			} else if(selected_sensor.equals(tempArray[4])){
				air_font=arg2+1;
			} else if(selected_sensor.equals(tempArray[5])){
				dz_font=arg2+1;
			} else if(selected_sensor.equals(tempArray[6])){
				// arrow
			} else if(selected_sensor.equals(tempArray[7])){
				addinfo_font=arg2+1;
			} else if(selected_sensor.equals(tempArray[8])){
				addinfo2_font=arg2+1;
			} else if(selected_sensor.equals(tempArray[9])){
				gps_font=arg2+1;
			} else if(selected_sensor.equals(tempArray[10])){
				fuel_font=arg2+1;
			} else if(selected_sensor.equals(tempArray[11])){
				gear_font=arg2+1;
			} else if(selected_sensor.equals(tempArray[12])){
				clock_font=arg2+1;
			}

			clear_image();
			draw_skin();
		}

	}


	@Override
	public void onNothingSelected(AdapterView<?> arg0) {
		// TODO Auto-generated method stub

	}
}
